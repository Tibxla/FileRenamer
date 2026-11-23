// Tests d'interface : la vraie MainWindow, avec des réglages isolés dans un dossier temporaire.
#include "elidedlabel.h"
#include "folderdropzone.h"
#include "mainwindow.h"

#include <QDir>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <QTest>
#include <QToolButton>

class MainWindowTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void imposedPrefixFollowsSourceFolder();

private:
    QTemporaryDir settingsDir;
};

namespace {

void touch(const QString &path)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
}

}

void MainWindowTests::initTestCase()
{
    QVERIFY(settingsDir.isValid());
    QCoreApplication::setOrganizationName("TibxlaTests");
    QCoreApplication::setApplicationName("FileRenamerUiTests");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
}

void MainWindowTests::imposedPrefixFollowsSourceFolder()
{
    QTemporaryDir root;
    QVERIFY(root.isValid());
    QDir dir(root.path());
    QVERIFY(dir.mkdir("a") && dir.mkdir("b") && dir.mkdir("out"));
    touch(dir.filePath("a/paris_001.pdf"));
    touch(dir.filePath("a/paris_002.pdf"));
    touch(dir.filePath("a/IMG_7.jpg"));
    touch(dir.filePath("b/lyon_001.pdf"));
    {
        QSettings settings;
        settings.setValue("lastSourceDir", dir.filePath("a"));
        settings.setValue("lastDestDir", dir.filePath("out"));
        settings.setValue("lastNomenclature", "PARIS_P{n}");
    }

    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    auto *series = window.findChild<ElidedLabel *>("seriesLabel");
    auto *modify = window.findChild<QToolButton *>("modifyButton");
    auto *prefix = window.findChild<QLineEdit *>("prefixEdit");
    auto *source = window.findChild<FolderDropZone *>("sourceZone");
    auto *rename = window.findChild<QPushButton *>("renameButton");
    QVERIFY(series && modify && prefix && source && rename);

    // Détection automatique : la série la plus nombreuse est retenue.
    QTRY_COMPARE(series->fullText(), QString("Série : paris_… · 2 fichiers"));
    QVERIFY(!prefix->isVisible());

    // « Modifier » révèle le champ ; la saisie recalcule l'aperçu sans relire le disque.
    modify->click();
    QVERIFY(prefix->isVisible());
    QVERIFY(!modify->isVisible());
    QCOMPARE(prefix->placeholderText(), QString("paris_"));
    QTest::keyClicks(prefix, "IMG");
    QCOMPARE(series->fullText(), QString("Début de nom imposé : « IMG » · 1 fichier"));

    // La copie utilise le même début de nom que l'aperçu.
    QVERIFY(rename->isEnabled());
    rename->click();
    QCOMPARE(QDir(dir.filePath("out")).entryList(QDir::Files), QStringList{"PARIS_P007.jpg"});

    // Un autre dossier source vide et replie le champ : la détection automatique reprend.
    source->setPath(dir.filePath("b"));
    QVERIFY(prefix->text().isEmpty());
    QVERIFY(!prefix->isVisible());
    QTRY_COMPARE(series->fullText(), QString("Série : lyon_… · 1 fichier"));
    QVERIFY(modify->isVisible());
}

QTEST_MAIN(MainWindowTests)
#include "mainwindow_tests.moc"
