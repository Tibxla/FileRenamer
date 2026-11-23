#include "mainwindow.h"

#include "renamecore.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>

#include <algorithm>

namespace {

RenameCore::DirectoryPaths directoryPaths(const QString &path)
{
    const QString absolute = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    return {absolute, QFileInfo(absolute).canonicalFilePath()};
}

QString skipReasonText(RenameCore::SkipReason reason)
{
    switch (reason) {
    case RenameCore::SkipReason::NoNumber:
        return MainWindow::tr("Aucun numéro dans le nom.");
    case RenameCore::SkipReason::AlreadyExists:
        return MainWindow::tr("Une entrée du même nom existe déjà dans le dossier d'arrivée.");
    case RenameCore::SkipReason::DuplicateOutput:
        return MainWindow::tr("Un fichier source précédent produit le même nom de copie.");
    }
    Q_UNREACHABLE();
}

QString ignoredFileDetail(const QString &name, const QString &reason)
{
    QString line = QStringLiteral("%1 → %2").arg(name, reason);
    return line.replace(QLatin1Char('\n'), QStringLiteral("\\n"))
               .replace(QLatin1Char('\r'), QStringLiteral("\\r"));
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , nomenclatures(settings)
{
    ui->setupUi(this);

    ui->sourceDirLineEdit->setText(settings.value(QStringLiteral("lastSourceDir")).toString());
    ui->destDirLineEdit->setText(settings.value(QStringLiteral("lastDestDir")).toString());
    const QByteArray geometry = settings.value(QStringLiteral("geometry")).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    auto *combo = ui->nomenclatureComboBox;
    combo->addItems(nomenclatures.entries());
    combo->setCompleter(nullptr);
    combo->lineEdit()->setPlaceholderText(QStringLiteral("PARIS_P{n}"));
    combo->lineEdit()->setTextMargins(8, 0, 0, 0);
    const QString lastUsed = nomenclatures.lastUsed();
    combo->setCurrentIndex(combo->findText(lastUsed));
    combo->setEditText(lastUsed);
    updateNomenclatureButtons();

    const bool light = settings.value(QStringLiteral("theme")).toString() == QStringLiteral("light");
    ui->lightThemeRadioButton->setChecked(light);
    ui->darkThemeRadioButton->setChecked(!light);
    applyTheme(light);

    connect(ui->browseSourceButton, &QPushButton::clicked, this, [this] {
        browseDirectory(ui->sourceDirLineEdit, tr("Sélectionner le dossier source"));
    });
    connect(ui->browseDestButton, &QPushButton::clicked, this, [this] {
        browseDirectory(ui->destDirLineEdit, tr("Sélectionner le dossier d'arrivée"));
    });
    connect(ui->renameButton, &QPushButton::clicked, this, &MainWindow::renameFiles);
    connect(ui->saveNomenclatureButton, &QPushButton::clicked, this, &MainWindow::saveNomenclature);
    connect(ui->removeNomenclatureButton, &QPushButton::clicked, this, &MainWindow::removeNomenclature);
    connect(combo, &QComboBox::currentTextChanged, this, &MainWindow::updateNomenclatureButtons);
    connect(ui->lightThemeRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        applyTheme(checked);
        settings.setValue(QStringLiteral("theme"), checked ? QStringLiteral("light") : QStringLiteral("dark"));
    });
}

MainWindow::~MainWindow()
{
    settings.setValue(QStringLiteral("lastSourceDir"), ui->sourceDirLineEdit->text());
    settings.setValue(QStringLiteral("lastDestDir"), ui->destDirLineEdit->text());
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
}

void MainWindow::applyTheme(bool light)
{
    QFile style(light ? QStringLiteral(":/light.qss") : QStringLiteral(":/style.qss"));
    if (style.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(QString::fromUtf8(style.readAll()));
    }
}

void MainWindow::browseDirectory(QLineEdit *field, const QString &title)
{
    const QString initialPath = field->text().isEmpty() ? QDir::homePath() : field->text();
    const QString selected = QFileDialog::getExistingDirectory(this, title, initialPath);
    if (!selected.isEmpty()) {
        field->setText(selected);
    }
}

void MainWindow::saveNomenclature()
{
    auto *combo = ui->nomenclatureComboBox;
    const QString text = combo->currentText();
    const QString error = RenameCore::validateNomenclature(text);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Nomenclature invalide"), error);
        return;
    }

    if (nomenclatures.add(text)) {
        combo->addItem(text);
        combo->setCurrentIndex(combo->count() - 1);
        updateNomenclatureButtons();
    }
}

void MainWindow::removeNomenclature()
{
    auto *combo = ui->nomenclatureComboBox;
    const int index = combo->findText(combo->currentText());
    if (index >= 0 && nomenclatures.remove(combo->itemText(index))) {
        combo->removeItem(index);
        combo->setCurrentIndex(-1);
        combo->clearEditText();
        updateNomenclatureButtons();
    }
}

void MainWindow::updateNomenclatureButtons()
{
    const auto *combo = ui->nomenclatureComboBox;
    const bool saved = combo->findText(combo->currentText()) >= 0;
    ui->saveNomenclatureButton->setEnabled(!saved);
    ui->removeNomenclatureButton->setEnabled(saved);
}

void MainWindow::renameFiles()
{
    const QString nomenclature = ui->nomenclatureComboBox->currentText();
    const QString error = RenameCore::validateNomenclature(nomenclature);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, tr("Nomenclature invalide"), error);
        return;
    }

    const QString sourceText = ui->sourceDirLineEdit->text();
    const QString destText = ui->destDirLineEdit->text();
    if (sourceText.isEmpty() || destText.isEmpty()) {
        QMessageBox::warning(this, tr("Dossiers requis"),
                             tr("Renseignez le dossier source et le dossier d'arrivée."));
        return;
    }

    const auto sourcePaths = directoryPaths(sourceText);
    const auto destPaths = directoryPaths(destText);
    const QDir source(sourcePaths.absolutePath);
    const QDir destination(destPaths.absolutePath);
    if (!source.exists()) {
        QMessageBox::warning(this, tr("Dossier source introuvable"), tr("Choisissez un dossier source existant."));
        return;
    }

    if (RenameCore::sameDirectory(sourcePaths, destPaths)) {
        QMessageBox::warning(this, tr("Dossiers identiques"),
                             tr("Choisissez un dossier d'arrivée différent du dossier source."));
        return;
    }
    if (!destination.exists() && !destination.mkpath(QStringLiteral("."))) {
        QMessageBox::critical(this, tr("Dossier d'arrivée inaccessible"),
                              tr("Impossible de créer le dossier d'arrivée."));
        return;
    }

    QStringList sourceNames = source.entryList(QDir::Files, QDir::NoSort);
    std::sort(sourceNames.begin(), sourceNames.end(), [](const QString &left, const QString &right) {
        const int order = left.compare(right, Qt::CaseInsensitive);
        return order == 0 ? left < right : order < 0;
    });
    const QStringList existingNames = destination.entryList(
        QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot, QDir::NoSort);
    const auto plan = RenameCore::plan(sourceNames, existingNames, nomenclature);
    nomenclatures.setLastUsed(nomenclature);

    QStringList ignored;
    for (const auto &file : plan.skipped) {
        ignored.append(ignoredFileDetail(file.sourceName, skipReasonText(file.reason)));
    }

    qsizetype copied = 0;
    qsizetype attempted = 0;
    if (!plan.copies.isEmpty()) {
        QProgressDialog progress(tr("Création des copies renommées…"), tr("Annuler"), 0,
                                 static_cast<int>(plan.copies.size()), this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setAutoReset(false);
        progress.setValue(0);
        progress.show();
        QCoreApplication::processEvents();

        for (const auto &copy : plan.copies) {
            if (progress.wasCanceled()) {
                break;
            }

            QFile file(source.filePath(copy.sourceName));
            if (file.copy(destination.filePath(copy.outputName))) {
                ++copied;
            } else {
                ignored.append(ignoredFileDetail(copy.sourceName,
                                                 tr("La copie a échoué (%1).").arg(file.errorString())));
            }
            progress.setValue(static_cast<int>(++attempted));
        }
    }

    const bool cancelled = attempted < plan.copies.size();
    for (qsizetype index = attempted; index < plan.copies.size(); ++index) {
        ignored.append(ignoredFileDetail(plan.copies.at(index).sourceName, tr("Opération annulée.")));
    }

    const QString copyCount = (copied == 1 ? tr("%1 copie renommée") : tr("%1 copies renommées")).arg(copied);
    const QString skipCount = (ignored.size() == 1 ? tr("%1 fichier ignoré") : tr("%1 fichiers ignorés"))
                                  .arg(ignored.size());
    QMessageBox summary(QMessageBox::Information, tr("Résultat"),
                        tr("%1, %2").arg(copyCount, skipCount), QMessageBox::Ok, this);
    summary.setTextFormat(Qt::PlainText);
    if (cancelled) {
        summary.setInformativeText(tr("Opération annulée."));
    }
    if (!ignored.isEmpty()) {
        summary.setDetailedText(ignored.join(QLatin1Char('\n')));
    }
    summary.exec();
}
