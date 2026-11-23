#include "mainwindow.h"

#include "renamecore.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStyle>
#include <QTextDocument>
#include <QUrl>

#include <algorithm>

namespace {

RenameCore::DirectoryPaths directoryPaths(const QString &path)
{
    const QString absolute = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    return {absolute, QFileInfo(absolute).canonicalFilePath()};
}

QStringList readSourceNames(const QDir &source)
{
    QStringList names = source.entryList(QDir::Files, QDir::NoSort);
    std::sort(names.begin(), names.end(), [](const QString &left, const QString &right) {
        const int order = left.compare(right, Qt::CaseInsensitive);
        return order == 0 ? left < right : order < 0;
    });
    return names;
}

QStringList readExistingNames(const QDir &destination)
{
    return destination.entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot,
                                 QDir::NoSort);
}

QString sameDirectoryMessage()
{
    return MainWindow::tr("Choisissez un dossier d'arrivée différent du dossier source.");
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

    ui->renamePreview->installEventFilter(this);
    updatePreview();
    previewRefreshTimer.setSingleShot(true);
    previewRefreshTimer.setTimerType(Qt::PreciseTimer);
    previewRefreshTimer.setInterval(250);
    connect(&previewRefreshTimer, &QTimer::timeout, this, &MainWindow::refreshPreviewDirectories);
    connect(ui->sourceDirLineEdit, &QLineEdit::textChanged,
            &previewRefreshTimer, qOverload<>(&QTimer::start));
    connect(ui->destDirLineEdit, &QLineEdit::textChanged,
            &previewRefreshTimer, qOverload<>(&QTimer::start));
    if (!ui->sourceDirLineEdit->text().isEmpty()) {
        previewRefreshTimer.start();
    }

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
    connect(combo, &QComboBox::currentTextChanged, this, &MainWindow::updatePreview);
}

MainWindow::~MainWindow()
{
    settings.setValue(QStringLiteral("lastSourceDir"), ui->sourceDirLineEdit->text());
    settings.setValue(QStringLiteral("lastDestDir"), ui->destDirLineEdit->text());
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->renamePreview &&
        (event->type() == QEvent::Resize || event->type() == QEvent::FontChange)) {
        fitPreview();
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::browseDirectory(QLineEdit *field, const QString &title)
{
    const QString initialPath = field->text().isEmpty() ? QDir::homePath() : field->text();
    const QString selected = QFileDialog::getExistingDirectory(
        this, title, initialPath, QFileDialog::ShowDirsOnly);
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

void MainWindow::refreshPreviewDirectories()
{
    const QString sourceText = ui->sourceDirLineEdit->text();
    const QString destText = ui->destDirLineEdit->text();
    const auto sourcePaths = directoryPaths(sourceText);
    const QDir source(sourcePaths.absolutePath);
    previewSourceExists = !sourceText.isEmpty() && source.exists();
    previewSameDirectory = false;
    previewSourceNames.clear();
    previewExistingNames.clear();

    if (previewSourceExists) {
        const auto destPaths = directoryPaths(destText);
        previewSameDirectory = !destText.isEmpty() && RenameCore::sameDirectory(sourcePaths, destPaths);
        if (!previewSameDirectory) {
            previewSourceNames = readSourceNames(source);
            if (!destText.isEmpty()) {
                previewExistingNames = readExistingNames(QDir(destPaths.absolutePath));
            }
        }
    }
    updatePreview();
}

void MainWindow::updatePreview()
{
    if (previewRefreshTimer.isActive()) {
        return;
    }
    if (!previewSourceExists) {
        setPreview(tr("Choisissez un dossier source pour voir un exemple."));
        return;
    }
    if (previewSameDirectory) {
        setPreview(sameDirectoryMessage(), true);
        return;
    }

    const QString nomenclature = ui->nomenclatureComboBox->currentText();
    if (nomenclature.isEmpty()) {
        setPreview(tr("Saisissez une nomenclature, avec {n} à l'endroit du numéro."));
        return;
    }
    const QString error = RenameCore::validateNomenclature(nomenclature);
    if (!error.isEmpty()) {
        setPreview(error, true);
        return;
    }

    const auto plan = RenameCore::plan(previewSourceNames, previewExistingNames, nomenclature);
    setPreview(RenameCore::previewText(plan));
}

void MainWindow::setPreview(const QString &text, bool error)
{
    auto *label = ui->renamePreview;
    if (label->property("previewError").toBool() != error) {
        label->setProperty("previewError", error);
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
    previewText = text;
    label->setAccessibleName(text);
    fitPreview();
}

void MainWindow::fitPreview()
{
    auto *label = ui->renamePreview;
    const auto metrics = label->fontMetrics();
    label->setMinimumHeight(2 * metrics.lineSpacing());
    QString visibleText = previewText;
    const qsizetype lineBreak = previewText.indexOf(QLatin1Char('\n'));
    if (lineBreak >= 0) {
        const QString example = previewText.first(lineBreak);
        const QString elided = metrics.elidedText(example, Qt::ElideMiddle, label->contentsRect().width());
        visibleText = elided + previewText.sliced(lineBreak);
    }
    label->setText(visibleText);
    label->setToolTip(visibleText == previewText ? QString() : Qt::convertFromPlainText(previewText));
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
        QMessageBox::warning(this, tr("Dossiers identiques"), sameDirectoryMessage());
        return;
    }
    if (!destination.exists() && !destination.mkpath(QStringLiteral("."))) {
        QMessageBox::critical(this, tr("Dossier d'arrivée inaccessible"),
                              tr("Impossible de créer le dossier d'arrivée."));
        return;
    }

    const auto plan = RenameCore::plan(readSourceNames(source), readExistingNames(destination), nomenclature);
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
    if (copied > 0) {
        auto *openFolder = summary.addButton(tr("Ouvrir le dossier d'arrivée"), QMessageBox::ActionRole);
        summary.setDefaultButton(QMessageBox::Ok);
        summary.setEscapeButton(QMessageBox::Ok);
        const QUrl folderUrl = QUrl::fromLocalFile(destination.absolutePath());
        connect(&summary, &QMessageBox::buttonClicked, &summary,
                [openFolder, folderUrl](QAbstractButton *button) {
            if (button == openFolder) {
                QDesktopServices::openUrl(folderUrl);
            }
        });
    }
    previewRefreshTimer.start();
    summary.exec();
}
