#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QRegularExpression>
#include <QDebug>
#include <QSettings>
#include <QProgressDialog>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , digitCount(3)
    , regexPattern("_(\\d+)$")
    , currentTheme("dark")
    , regexOption("no_letter")
    , letterBefore("")
    , letterAfter("")
    , customRegex("_(\\d+)$")
    , popupCount(0)
    , virusSimulationEnabled(true)
{
    ui->setupUi(this);
    setWindowTitle("FileRenamer");

    // Configuration du minuteur pour la simulation de virus (5 minutes)
    virusTimer = new QTimer(this);
    virusTimer->setSingleShot(true);
    connect(virusTimer, &QTimer::timeout, this, &MainWindow::onVirusTimerTimeout);

    // Configuration du minuteur pour les fenêtres pop-up
    popupTimer = new QTimer(this);
    connect(popupTimer, &QTimer::timeout, this, &MainWindow::onPopupTimerTimeout);

    // Connexion de la case à cocher pour activer/désactiver la simulation de virus
    connect(ui->virusSimulationCheckBox, &QCheckBox::toggled, this, &MainWindow::on_virusSimulationCheckBox_toggled);

    // Déconnexion des signaux pour éviter les doublons
    disconnect(ui->browseButton, &QPushButton::clicked, this, &MainWindow::on_browseButton_clicked);
    disconnect(ui->browseDestButton, &QPushButton::clicked, this, &MainWindow::on_browseDestButton_clicked);
    disconnect(ui->browseDossierDestinationButton, &QPushButton::clicked, this, &MainWindow::on_browseDossierDestinationButton_clicked);
    disconnect(ui->renameButton, &QPushButton::clicked, this, &MainWindow::on_renameButton_clicked);
    disconnect(ui->createPdfButton, &QPushButton::clicked, this, &MainWindow::on_createPdfButton_clicked);

    // Connexions des signaux pour les options de l'interface
    connect(ui->noLetterRadioButton, &QRadioButton::toggled, this, &MainWindow::on_regexOptionChanged);
    connect(ui->letterBeforeRadioButton, &QRadioButton::toggled, this, &MainWindow::on_regexOptionChanged);
    connect(ui->letterAfterRadioButton, &QRadioButton::toggled, this, &MainWindow::on_regexOptionChanged);
    connect(ui->customRegexRadioButton, &QRadioButton::toggled, this, &MainWindow::on_regexOptionChanged);
    connect(ui->darkThemeRadioButton, &QRadioButton::toggled, this, &MainWindow::on_themeChanged);
    connect(ui->lightThemeRadioButton, &QRadioButton::toggled, this, &MainWindow::on_themeChanged);

    // Charger les paramètres sauvegardés
    loadSettings();

    // Démarrer le minuteur pour la simulation de virus si elle est activée
    if (virusSimulationEnabled) {
        virusTimer->start(5 * 60 * 1000); // 5 minutes
    }
}

MainWindow::~MainWindow()
{
    virusTimer->stop();
    popupTimer->stop();
    saveSettings();
    delete ui;
}

void MainWindow::onVirusTimerTimeout()
{
    if (virusSimulationEnabled) {
        popupCount = 0;
        popupTimer->start(10); // Ouvre une fenêtre toutes les 10 ms
    }
}

void MainWindow::onPopupTimerTimeout()
{
    if (popupCount < 100) {
        QDialog *dialog = new QDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle("Alerte Virus");

        // Obtenir les dimensions de l'écran
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();

        // Générer des positions aléatoires
        int x = QRandomGenerator::global()->bounded(screenWidth - 300);
        int y = QRandomGenerator::global()->bounded(screenHeight - 200);
        dialog->move(x, y);

        QVBoxLayout *layout = new QVBoxLayout(dialog);
        QLabel *label = new QLabel("ALERTE VIRUS! Votre système est infecté.\nPour nettoyer votre ordinateur, veuillez faire un don de 10€ via PayPal à paypal.me/tibola.");
        layout->addWidget(label);

        QPushButton *button = new QPushButton("Faire un don pour nettoyer");
        connect(button, &QPushButton::clicked, []() {
            QDesktopServices::openUrl(QUrl("https://paypal.me/tibola"));
        });
        layout->addWidget(button);

        dialog->setLayout(layout);
        dialog->show();
        popupCount++;
    } else {
        popupTimer->stop();
    }
}

void MainWindow::on_virusSimulationCheckBox_toggled(bool checked)
{
    virusSimulationEnabled = checked;
    if (!checked) {
        virusTimer->stop();
        popupTimer->stop();
    } else if (!virusTimer->isActive()) {
        virusTimer->start(5 * 60 * 1000);
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("MyCompany", "FileRenamer");

    // Charger l'état de la simulation de virus
    virusSimulationEnabled = settings.value("virusSimulationEnabled", true).toBool();
    ui->virusSimulationCheckBox->setChecked(virusSimulationEnabled);

    // Charger le thème
    currentTheme = settings.value("theme", "dark").toString();
    if (currentTheme == "light") {
        ui->lightThemeRadioButton->setChecked(true);
    } else {
        ui->darkThemeRadioButton->setChecked(true);
    }
    on_themeChanged();

    // Charger l'option regex
    regexOption = settings.value("regexOption", "no_letter").toString();
    letterBefore = settings.value("letterBefore", "").toString();
    letterAfter = settings.value("letterAfter", "").toString();
    customRegex = settings.value("customRegex", "_(\\d+)$").toString();

    if (regexOption == "no_letter") {
        ui->noLetterRadioButton->setChecked(true);
    } else if (regexOption == "letter_before") {
        ui->letterBeforeRadioButton->setChecked(true);
        ui->letterBeforeLineEdit->setText(letterBefore);
    } else if (regexOption == "letter_after") {
        ui->letterAfterRadioButton->setChecked(true);
        ui->letterAfterLineEdit->setText(letterAfter);
    } else if (regexOption == "custom") {
        ui->customRegexRadioButton->setChecked(true);
        ui->customRegexLineEdit->setText(customRegex);
    }

    // Charger les autres paramètres
    digitCount = settings.value("digitCount", 4).toInt();
    ui->digitCountSpinBox->setValue(digitCount);

    // Charger les derniers chemins
    lastSourceDir = settings.value("lastSourceDir", QDir::homePath()).toString();
    lastDestDir = settings.value("lastDestDir", QDir::homePath()).toString();
    lastPdfDestDir = settings.value("lastPdfDestDir", QDir::homePath()).toString();

    // Charger la position et la taille de la fenêtre
    QPoint pos = settings.value("windowPosition", QPoint(0, 0)).toPoint();
    QSize size = settings.value("windowSize", QSize(600, 500)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::saveSettings()
{
    QSettings settings("MyCompany", "FileRenamer");

    // Sauvegarder l'état de la simulation de virus
    settings.setValue("virusSimulationEnabled", virusSimulationEnabled);

    // Sauvegarder le thème
    settings.setValue("theme", currentTheme);

    // Sauvegarder l'option regex
    settings.setValue("regexOption", regexOption);
    if (regexOption == "letter_before") {
        settings.setValue("letterBefore", ui->letterBeforeLineEdit->text());
    } else if (regexOption == "letter_after") {
        settings.setValue("letterAfter", ui->letterAfterLineEdit->text());
    } else if (regexOption == "custom") {
        settings.setValue("customRegex", ui->customRegexLineEdit->text());
    }

    // Sauvegarder les autres paramètres
    settings.setValue("digitCount", ui->digitCountSpinBox->value());

    // Sauvegarder les derniers chemins
    settings.setValue("lastSourceDir", lastSourceDir);
    settings.setValue("lastDestDir", lastDestDir);
    settings.setValue("lastPdfDestDir", lastPdfDestDir);

    // Sauvegarder la position et la taille de la fenêtre
    settings.setValue("windowPosition", pos());
    settings.setValue("windowSize", size());
}

void MainWindow::on_regexOptionChanged()
{
    if (ui->noLetterRadioButton->isChecked()) {
        regexOption = "no_letter";
        ui->letterBeforeLineEdit->setEnabled(false);
        ui->letterAfterLineEdit->setEnabled(false);
        ui->customRegexLineEdit->setEnabled(false);
    } else if (ui->letterBeforeRadioButton->isChecked()) {
        regexOption = "letter_before";
        ui->letterBeforeLineEdit->setEnabled(true);
        ui->letterAfterLineEdit->setEnabled(false);
        ui->customRegexLineEdit->setEnabled(false);
    } else if (ui->letterAfterRadioButton->isChecked()) {
        regexOption = "letter_after";
        ui->letterBeforeLineEdit->setEnabled(false);
        ui->letterAfterLineEdit->setEnabled(true);
        ui->customRegexLineEdit->setEnabled(false);
    } else if (ui->customRegexRadioButton->isChecked()) {
        regexOption = "custom";
        ui->letterBeforeLineEdit->setEnabled(false);
        ui->letterAfterLineEdit->setEnabled(false);
        ui->customRegexLineEdit->setEnabled(true);
    }
}

QString MainWindow::getRegexPattern()
{
    if (regexOption == "no_letter") {
        return "_(\\d+)$";
    } else if (regexOption == "letter_before") {
        letterBefore = ui->letterBeforeLineEdit->text();
        if (!letterBefore.isEmpty()) {
            return "_" + letterBefore + "(\\d+)$";
        }
    } else if (regexOption == "letter_after") {
        letterAfter = ui->letterAfterLineEdit->text();
        if (!letterAfter.isEmpty()) {
            return "_(\\d+)" + letterAfter + "$";
        }
    } else if (regexOption == "custom") {
        return ui->customRegexLineEdit->text();
    }
    return "_(\\d+)$";
}

void MainWindow::on_renameButton_clicked()
{
    QString sourceDir = ui->sourceDirLineEdit->text();
    QString newBaseName = ui->baseNameLineEdit->text();
    QString newFolderName = ui->folderNameLineEdit->text();

    if (sourceDir.isEmpty() || newBaseName.isEmpty() || newFolderName.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Erreur");
        msgBox.setText("Veuillez remplir tous les champs.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    digitCount = ui->digitCountSpinBox->value();
    regexPattern = getRegexPattern();

    QDir sourceDirectory(sourceDir);
    if (!sourceDirectory.exists()) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Erreur");
        msgBox.setText("Le dossier source n'existe pas.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    QDir destDir(newFolderName);
    if (!destDir.exists()) {
        if (!destDir.mkpath(".")) {
            QMessageBox msgBox(this);
            msgBox.setStyleSheet(qApp->styleSheet());
            msgBox.setWindowTitle("Erreur");
            msgBox.setText("Échec de la création du dossier de destination.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
            return;
        }
    }

    QList<QFileInfo> fileList = sourceDirectory.entryInfoList(QDir::Files);
    int totalFiles = fileList.size();
    if (totalFiles == 0) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Information");
        msgBox.setText("Aucun fichier à renommer.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    QProgressDialog progress("Renommage des fichiers...", "Annuler", 0, totalFiles, this);
    progress.setStyleSheet(qApp->styleSheet());
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    QRegularExpression re(regexPattern);
    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < totalFiles; ++i) {
        progress.setValue(i);
        if (progress.wasCanceled()) {
            break;
        }

        const QFileInfo &fileInfo = fileList.at(i);
        QString fileName = fileInfo.baseName();
        QRegularExpressionMatch match = re.match(fileName);
        if (match.hasMatch()) {
            QString number = match.captured(1);
            bool ok;
            int num = number.toInt(&ok);
            if (ok) {
                QString formattedNumber = QString("%1").arg(num, digitCount, 10, QChar('0'));
                QString newFileName = newBaseName + "_" + formattedNumber + "." + fileInfo.suffix();
                QString destPath = destDir.filePath(newFileName);
                if (QFile::exists(destPath)) {
                    qDebug() << "Le fichier destination existe déjà : " << destPath;
                    failCount++;
                } else {
                    if (QFile::copy(fileInfo.filePath(), destPath)) {
                        qDebug() << "Fichier renommé et copié : " << newFileName;
                        successCount++;
                    } else {
                        qDebug() << "Échec de la copie : " << fileInfo.fileName();
                        failCount++;
                    }
                }
            } else {
                qDebug() << "Le numéro extrait n'est pas un nombre valide : " << number;
                failCount++;
            }
        } else {
            qDebug() << "Aucun numéro trouvé dans : " << fileInfo.fileName();
            failCount++;
        }
    }

    progress.setValue(totalFiles);

    if (progress.wasCanceled()) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Annulé");
        msgBox.setText("L'opération de renommage a été annulée.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    } else {
        QString message = QString("Opération terminée.\nFichiers renommés avec succès : %1\nÉchecs : %2").arg(successCount).arg(failCount);
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Résultat");
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
}

void MainWindow::on_browseButton_clicked()
{
    qDebug() << "Bouton Parcourir cliqué, émetteur :" << sender();
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Sélectionner le dossier source", lastSourceDir.isEmpty() ? QDir::homePath() : lastSourceDir);
    if (!selectedDir.isEmpty()) {
        ui->sourceDirLineEdit->setText(selectedDir);
        lastSourceDir = selectedDir;
    }
}

void MainWindow::on_browseDestButton_clicked()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Sélectionner le dossier de destination", lastDestDir.isEmpty() ? QDir::homePath() : lastDestDir);
    if (!selectedDir.isEmpty()) {
        ui->folderNameLineEdit->setText(selectedDir);
        lastDestDir = selectedDir;
    }
}

void MainWindow::on_browseDossierDestinationButton_clicked()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Sélectionner le dossier de destination", lastPdfDestDir.isEmpty() ? QDir::homePath() : lastPdfDestDir);
    if (!selectedDir.isEmpty()) {
        ui->dossierDestinationLineEdit->setText(selectedDir);
        lastPdfDestDir = selectedDir;
    }
}

void MainWindow::on_createPdfButton_clicked()
{
    QString nomFichier = ui->nomFichierLineEdit->text();
    QString cheminDossier = ui->dossierDestinationLineEdit->text();
    int nombreFichiers = ui->nombreFichiersSpinBox->value();

    if (nomFichier.isEmpty() || cheminDossier.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Erreur");
        msgBox.setText("Veuillez remplir tous les champs.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    QDir destDir(cheminDossier);
    if (!destDir.exists()) {
        if (!destDir.mkpath(".")) {
            QMessageBox msgBox(this);
            msgBox.setStyleSheet(qApp->styleSheet());
            msgBox.setWindowTitle("Erreur");
            msgBox.setText("Échec de la création du dossier de destination.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
            return;
        }
    }

    QProgressDialog progress("Création des fichiers PDF...", "Annuler", 0, nombreFichiers, this);
    progress.setStyleSheet(qApp->styleSheet());
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    int successCount = 0;
    int failCount = 0;

    for (int i = 1; i <= nombreFichiers; ++i) {
        progress.setValue(i - 1);
        if (progress.wasCanceled()) {
            break;
        }

        QString numero = QString("%1").arg(i, 4, 10, QChar('0'));
        QString nomComplet = destDir.filePath(QString("%1_%2.pdf").arg(nomFichier).arg(numero));

        QFile file(nomComplet);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            qDebug() << "Fichier créé : " << nomComplet;
            successCount++;
        } else {
            qDebug() << "Erreur : Échec de la création du fichier : " << nomComplet;
            failCount++;
        }
    }

    progress.setValue(nombreFichiers);

    if (progress.wasCanceled()) {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Annulé");
        msgBox.setText("L'opération de création de fichiers PDF a été annulée.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    } else {
        QString message = QString("Opération terminée.\nFichiers créés avec succès : %1\nÉchecs : %2").arg(successCount).arg(failCount);
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(qApp->styleSheet());
        msgBox.setWindowTitle("Résultat");
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
}

void MainWindow::on_themeChanged()
{
    QSettings settings("MyCompany", "FileRenamer");

    if (ui->darkThemeRadioButton->isChecked()) {
        currentTheme = "dark";
        QFile styleFile(":/style.qss");
        if (styleFile.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(styleFile.readAll());
            qApp->setStyleSheet(styleSheet);
            settings.setValue("theme", "dark");
            qDebug() << "Thème sombre appliqué";
        } else {
            qDebug() << "Erreur : Impossible d'ouvrir style.qss";
        }
    } else if (ui->lightThemeRadioButton->isChecked()) {
        currentTheme = "light";
        QFile styleFile(":/light.qss");
        if (styleFile.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(styleFile.readAll());
            qApp->setStyleSheet(styleSheet);
            settings.setValue("theme", "light");
            qDebug() << "Thème clair appliqué";
        } else {
            qDebug() << "Erreur : Impossible d'ouvrir light.qss";
        }
    }
}
