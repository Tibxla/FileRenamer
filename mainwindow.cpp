#include "mainwindow.h"

#include "displaytext.h"
#include "renamecore.h"
#include "theme.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QShortcut>
#include <QStyle>
#include <QStyledItemDelegate>
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
    case RenameCore::SkipReason::OutsideSeries:
        return MainWindow::tr("Le fichier ne fait pas partie de la série retenue.");
    case RenameCore::SkipReason::PrefixMismatch:
        return MainWindow::tr("Le nom du fichier ne commence pas par le début de nom imposé.");
    }
    Q_UNREACHABLE();
}

QString ignoredFileDetail(const QString &name, const QString &reason)
{
    return singleLineText(QStringLiteral("%1 → %2").arg(name, reason));
}

void repolish(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

void setFlag(QWidget *widget, const char *name, bool value)
{
    if (widget->property(name).toBool() != value) {
        widget->setProperty(name, value);
        repolish(widget);
    }
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , nomenclatures(settings)
{
    ui->setupUi(this);
    // La taille minimale suit la mise en page : la largeur du sous-titre, et une hauteur qui grandit
    // quand la liste des fichiers ignorés est dépliée.

    const QIcon idleFolder = Theme::icon(QStringLiteral("folder"), Theme::Role::Secondary);
    const QIcon chosenFolder = Theme::icon(QStringLiteral("folder"), Theme::Role::Accent);
    const QString dropHint = tr("Glissez un dossier ici ou cliquez pour parcourir");
    ui->sourceZone->setTitle(tr("Dossier source"));
    ui->sourceZone->setPlaceholderText(dropHint);
    ui->sourceZone->setIcons(idleFolder, chosenFolder);
    ui->destZone->setTitle(tr("Dossier d'arrivée"));
    ui->destZone->setPlaceholderText(dropHint);
    ui->destZone->setIcons(idleFolder, chosenFolder);
    ui->foldersArrow->setIcon(Theme::icon(QStringLiteral("arrow-right")));
    ui->exampleArrow->setIcon(Theme::icon(QStringLiteral("arrow-right")));
    ui->exampleSource->setElideMode(Qt::ElideMiddle);
    ui->exampleSource->setToolTipWhenElided(true);
    ui->exampleOutput->setElideMode(Qt::ElideMiddle);
    ui->exampleOutput->setToolTipWhenElided(true);
    ui->resultLabel->setToolTipWhenElided(true);

    const QSize buttonIconSize(16, 16);
    ui->saveNomenclatureButton->setIcon(Theme::icon(QStringLiteral("save"), Theme::Role::Text));
    ui->saveNomenclatureButton->setIconSize(buttonIconSize);
    ui->removeNomenclatureButton->setIcon(Theme::icon(QStringLiteral("delete"), Theme::Role::Text));
    ui->removeNomenclatureButton->setIconSize(buttonIconSize);
    ui->openFolderButton->setIcon(Theme::icon(QStringLiteral("folder-open"), Theme::Role::Text));
    ui->openFolderButton->setIconSize(buttonIconSize);
    ui->ignoredToggle->setIconSize(buttonIconSize);
    ui->progressBar->setTextVisible(false);
    ui->ignoredList->document()->setDocumentMargin(2);

    ui->sourceZone->setPath(settings.value(QStringLiteral("lastSourceDir")).toString());
    ui->destZone->setPath(settings.value(QStringLiteral("lastDestDir")).toString());
    // La 1.1 enregistrait une fenêtre bien plus petite : on ne reprend que la géométrie de la 1.2.
    settings.remove(QStringLiteral("geometry"));
    const QByteArray geometry = settings.value(QStringLiteral("windowGeometry")).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    auto *combo = ui->nomenclatureComboBox;
    combo->setItemDelegate(new QStyledItemDelegate(combo));
    combo->addItems(nomenclatures.entries());
    combo->setCompleter(nullptr);
    combo->lineEdit()->setPlaceholderText(QStringLiteral("PARIS_P{n}"));
    const QString lastUsed = nomenclatures.lastUsed();
    combo->setCurrentIndex(combo->findText(lastUsed));
    combo->setEditText(lastUsed);
    updateNomenclatureButtons();

    previewRefreshTimer.setSingleShot(true);
    previewRefreshTimer.setTimerType(Qt::PreciseTimer);
    previewRefreshTimer.setInterval(250);
    connect(&previewRefreshTimer, &QTimer::timeout, this, &MainWindow::refreshPreviewDirectories);
    showPanel(Panel::Preview);
    setIgnoredExpanded(false);
    if (ui->sourceZone->isEmpty()) {
        updatePreview();
    } else {
        // Pas de message « introuvable » avant la première lecture du dossier source.
        ui->messageRow->hide();
        ui->exampleRow->hide();
        ui->countsRow->hide();
        ui->renameButton->setEnabled(false);
        previewRefreshTimer.start();
    }

    for (FolderDropZone *zone : {ui->sourceZone, ui->destZone}) {
        connect(zone, &FolderDropZone::pathChanged, this, [this] {
            inputsChanged();
            previewRefreshTimer.start();
        });
    }
    connect(ui->sourceZone, &FolderDropZone::browseRequested, this, [this] {
        browseDirectory(ui->sourceZone, tr("Sélectionner le dossier source"));
    });
    connect(ui->destZone, &FolderDropZone::browseRequested, this, [this] {
        browseDirectory(ui->destZone, tr("Sélectionner le dossier d'arrivée"));
    });
    connect(ui->renameButton, &QPushButton::clicked, this, &MainWindow::renameFiles);
    connect(ui->saveNomenclatureButton, &QPushButton::clicked, this, &MainWindow::saveNomenclature);
    connect(ui->removeNomenclatureButton, &QPushButton::clicked, this, &MainWindow::removeNomenclature);
    connect(ui->cancelButton, &QPushButton::clicked, this, [this] { cancelRequested = true; });
    // Échap annule tout de suite : le raccourci d'un bouton ne clique qu'après 100 ms.
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, this, [this] {
        if (copying) {
            cancelRequested = true;
        }
    });
    connect(ui->openFolderButton, &QPushButton::clicked, this, [this] {
        QDesktopServices::openUrl(QUrl::fromLocalFile(resultFolder));
    });
    connect(ui->ignoredToggle, &QToolButton::toggled, this, &MainWindow::setIgnoredExpanded);
    connect(combo, &QComboBox::currentTextChanged, this, [this] {
        inputsChanged();
        updateNomenclatureButtons();
        updatePreview();
    });

    ui->centralWidget->setFocus();
}

MainWindow::~MainWindow()
{
    settings.setValue(QStringLiteral("lastSourceDir"), ui->sourceZone->path());
    settings.setValue(QStringLiteral("lastDestDir"), ui->destZone->path());
    settings.setValue(QStringLiteral("windowGeometry"), saveGeometry());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // La boucle de copie s'arrête au fichier suivant ; la fenêtre se ferme sans attendre.
    if (copying) {
        cancelRequested = true;
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::browseDirectory(FolderDropZone *zone, const QString &title)
{
    if (copying) {
        return;
    }
    const QString initialPath = zone->isEmpty() ? QDir::homePath() : zone->path();
    const QString selected = QFileDialog::getExistingDirectory(
        this, title, initialPath, QFileDialog::ShowDirsOnly);
    if (!selected.isEmpty()) {
        zone->setPath(selected);
    }
}

void MainWindow::inputsChanged()
{
    if (panel == Panel::Result) {
        showPanel(Panel::Preview);
    }
}

void MainWindow::saveNomenclature()
{
    auto *combo = ui->nomenclatureComboBox;
    const QString text = combo->currentText();
    if (!RenameCore::validateNomenclature(text).isEmpty()) {
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
    const QString text = combo->currentText();
    const bool saved = combo->findText(text) >= 0;
    const bool valid = RenameCore::validateNomenclature(text).isEmpty();
    ui->saveNomenclatureButton->setEnabled(!copying && !saved && valid);
    ui->removeNomenclatureButton->setEnabled(!copying && saved);
}

void MainWindow::refreshPreviewDirectories()
{
    const QString sourceText = ui->sourceZone->path();
    const QString destText = ui->destZone->path();
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

    bool ready = false;
    const QString nomenclature = ui->nomenclatureComboBox->currentText();
    const QString error = RenameCore::validateNomenclature(nomenclature);
    if (ui->sourceZone->isEmpty()) {
        showMessage(tr("Choisissez un dossier source pour voir un exemple."), false);
    } else if (!previewSourceExists) {
        showMessage(tr("Le dossier source est introuvable. Choisissez-en un autre."), true);
    } else if (previewSameDirectory) {
        showMessage(sameDirectoryMessage(), true);
    } else if (nomenclature.isEmpty()) {
        showMessage(tr("Saisissez une nomenclature, avec {n} à l'endroit du numéro."), false);
    } else if (!error.isEmpty()) {
        showMessage(error, true);
    } else {
        const auto plan = RenameCore::plan(previewSourceNames, previewExistingNames, nomenclature);
        const qsizetype copyCount = plan.copies.size();
        const qsizetype skipCount = plan.skipped.size();

        ui->messageRow->hide();
        ui->exampleRow->setVisible(copyCount > 0);
        if (copyCount > 0) {
            const auto &first = plan.copies.constFirst();
            ui->exampleSource->setFullText(singleLineText(first.sourceName));
            ui->exampleOutput->setFullText(singleLineText(first.outputName));
        }
        ui->copyPill->setText(copyCount == 0 ? tr("Aucun fichier à copier") : tr("%1 à copier").arg(copyCount));
        setFlag(ui->copyPill, "none", copyCount == 0);
        ui->skipPill->setText(skipCount == 1 ? tr("1 ignoré") : tr("%1 ignorés").arg(skipCount));
        ui->skipPill->setVisible(skipCount > 0);
        ui->readyHint->setVisible(ui->destZone->isEmpty());
        ui->countsRow->show();
        ui->previewPage->setAccessibleDescription(RenameCore::previewText(plan));
        ready = !ui->destZone->isEmpty() && copyCount + skipCount > 0;
    }
    ui->renameButton->setEnabled(ready && !copying);
}

void MainWindow::showMessage(const QString &text, bool error)
{
    ui->exampleRow->hide();
    ui->countsRow->hide();
    ui->messageIcon->setIcon(error ? Theme::icon(QStringLiteral("error"), Theme::Role::Critical)
                                   : Theme::icon(QStringLiteral("info"), Theme::Role::Secondary));
    ui->messageLabel->setText(text);
    setFlag(ui->messageLabel, "error", error);
    setFlag(ui->messageRow, "error", error);
    ui->messageRow->show();
    ui->previewPage->setAccessibleDescription(text);
}

void MainWindow::showPanel(Panel newPanel)
{
    panel = newPanel;
    QWidget *current = newPanel == Panel::Preview ? ui->previewPage
                     : newPanel == Panel::Progress ? ui->progressPage : ui->resultPage;
    // Une page cachée ne doit pas imposer sa hauteur à la carte.
    for (int index = 0; index < ui->statusStack->count(); ++index) {
        QWidget *page = ui->statusStack->widget(index);
        const auto policy = page == current ? QSizePolicy::Preferred : QSizePolicy::Ignored;
        page->setSizePolicy(policy, policy);
    }
    ui->statusStack->setCurrentWidget(current);
    ui->statusStack->updateGeometry();
    updateStretch();
}

void MainWindow::setBusy(bool busy)
{
    copying = busy;
    if (busy) {
        ui->centralWidget->setFocus();
    }
    ui->sourceZone->setEnabled(!busy);
    ui->destZone->setEnabled(!busy);
    ui->nomenclatureComboBox->setEnabled(!busy);
    updateNomenclatureButtons();
    if (busy) {
        ui->renameButton->setEnabled(false);
    }
}

void MainWindow::setIgnoredExpanded(bool expanded)
{
    if (ui->ignoredToggle->isChecked() != expanded) {
        ui->ignoredToggle->setChecked(expanded);
        return;
    }
    ui->ignoredToggle->setIcon(Theme::icon(expanded ? QStringLiteral("chevron-down") : QStringLiteral("chevron-right"),
                                           Theme::Role::Accent));
    if (expanded) {
        // Jusqu'à quatre lignes visibles sans défiler ; au-delà, la liste défile.
        const int lines = qBound(1, ui->ignoredList->document()->blockCount(), 4);
        const int chrome = ui->ignoredList->frameWidth() * 2 + 2 * 2 + 4 * 2;
        ui->ignoredList->setMinimumHeight(lines * ui->ignoredList->fontMetrics().lineSpacing() + chrome);
    }
    ui->ignoredList->setVisible(expanded);
    updateStretch();
}

void MainWindow::updateStretch()
{
    // Dépliée, la liste des fichiers ignorés prend la hauteur libre ; sinon la carte épouse son
    // contenu et l'espace restant passe au-dessus du bouton Renommer.
    const bool listShown = panel == Panel::Result && ui->ignoredToggle->isChecked();
    ui->windowLayout->setStretchFactor(ui->nomenclatureCard, listShown ? 1 : 0);
    ui->windowLayout->setStretch(ui->windowLayout->indexOf(ui->bottomSpacer), listShown ? 0 : 1);
}

void MainWindow::showResult(qsizetype copied, const QStringList &ignored, bool cancelled)
{
    QString title;
    if (cancelled) {
        title = copied == 0 ? tr("Annulé avant la première copie")
              : copied == 1 ? tr("Annulé après 1 copie renommée")
                            : tr("Annulé après %1 copies renommées").arg(copied);
    } else {
        title = copied == 0 ? tr("Aucune copie renommée")
              : copied == 1 ? tr("1 copie renommée") : tr("%1 copies renommées").arg(copied);
    }
    const bool success = copied > 0 && !cancelled;
    ui->resultIcon->setIcon(success ? Theme::icon(QStringLiteral("check-circle"), Theme::Role::Success)
                                    : Theme::icon(QStringLiteral("warning"), Theme::Role::Caution));
    ui->resultLabel->setFullText(title);
    ui->resultLabel->setAccessibleName(title);
    ui->openFolderButton->setVisible(copied > 0);

    ui->ignoredToggle->setVisible(!ignored.isEmpty());
    ui->ignoredToggle->setText(ignored.size() == 1 ? tr("1 fichier ignoré")
                                                   : tr("%1 fichiers ignorés").arg(ignored.size()));
    ui->ignoredList->setPlainText(ignored.join(QLatin1Char('\n')));
    setIgnoredExpanded(false);
    showPanel(Panel::Result);
}

void MainWindow::renameFiles()
{
    if (copying) {
        return;
    }
    const QString nomenclature = ui->nomenclatureComboBox->currentText();
    const QString sourceText = ui->sourceZone->path();
    const QString destText = ui->destZone->path();
    if (!RenameCore::validateNomenclature(nomenclature).isEmpty() || sourceText.isEmpty() || destText.isEmpty()) {
        updatePreview();
        return;
    }

    // Les dossiers ont pu changer depuis le dernier aperçu : on revérifie tout avant de copier.
    const auto sourcePaths = directoryPaths(sourceText);
    const auto destPaths = directoryPaths(destText);
    const QDir source(sourcePaths.absolutePath);
    const QDir destination(destPaths.absolutePath);
    if (!source.exists() || RenameCore::sameDirectory(sourcePaths, destPaths)) {
        // L'aperçu explique pourquoi la copie n'a pas eu lieu.
        showPanel(Panel::Preview);
        refreshPreviewDirectories();
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
    cancelRequested = false;
    if (!plan.copies.isEmpty()) {
        const int total = static_cast<int>(plan.copies.size());
        setBusy(true);
        ui->progressBar->setRange(0, total);
        ui->progressBar->setValue(0);
        ui->progressLabel->setText(tr("Création des copies renommées…"));
        showPanel(Panel::Progress);
        QCoreApplication::processEvents();

        for (const auto &copy : plan.copies) {
            if (cancelRequested) {
                break;
            }

            QFile file(source.filePath(copy.sourceName));
            if (file.copy(destination.filePath(copy.outputName))) {
                ++copied;
            } else {
                ignored.append(ignoredFileDetail(copy.sourceName,
                                                 tr("La copie a échoué (%1).").arg(file.errorString())));
            }
            ++attempted;
            ui->progressLabel->setText(tr("Création des copies renommées… %1 sur %2").arg(attempted).arg(total));
            ui->progressBar->setValue(static_cast<int>(attempted));
            QCoreApplication::processEvents();
        }
        setBusy(false);
    }

    const bool cancelled = attempted < plan.copies.size();
    for (qsizetype index = attempted; index < plan.copies.size(); ++index) {
        ignored.append(ignoredFileDetail(plan.copies.at(index).sourceName, tr("Opération annulée.")));
    }

    resultFolder = destination.absolutePath();
    showResult(copied, ignored, cancelled);
    previewRefreshTimer.start();
}
