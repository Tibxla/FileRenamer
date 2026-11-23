#include "folderdropzone.h"
#include "nomenclaturestore.h"
#include "renamecore.h"

#include <QDir>
#include <QDragEnterEvent>
#include <QFile>
#include <QLabel>
#include <QMimeData>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QUrl>

class FileRenamerTests : public QObject
{
    Q_OBJECT

private slots:
    void extractNumber_data();
    void extractNumber();
    void formatNumber_data();
    void formatNumber();
    void planPadsNumbersToThreeDigits();
    void normalizeNomenclature();
    void planMultiplePlaceholdersAndExtensions();
    void planSkipsSourceWithoutNumber();
    void planSkipsExistingNameRegardlessOfCase();
    void planKeepsFirstSourceRegardlessOfExtensionCase();
    void previewText_data();
    void previewText();
    void invalidNomenclature_data();
    void invalidNomenclature();
    void sameDirectory();
    void canonicalDirectory();
    void nomenclatureStore();
    void folderAndFileDrops();
    void rejectsNonLocalDrop();
    void zoneRequestsBrowsing();
};

void FileRenamerTests::extractNumber_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("number");

    QTest::newRow("last-extension") << QString("rapport.v2_004.pdf") << QString("004");
    QTest::newRow("digits-in-extension") << QString("clip.mp4") << QString();
    QTest::newRow("leading-zeroes") << QString("paris_000004.pdf") << QString("000004");
    QTest::newRow("letter-after") << QString("paris_004P") << QString("004");
    QTest::newRow("letter-before") << QString("paris_P004") << QString("004");
    QTest::newRow("last-number") << QString("2026_paris_004") << QString("004");
    QTest::newRow("long-number") << QString("a_000123456789012345678901")
                                 << QString("000123456789012345678901");
}

void FileRenamerTests::extractNumber()
{
    QFETCH(QString, name);
    QFETCH(QString, number);
    QCOMPARE(RenameCore::extractNumber(name), number);
}

void FileRenamerTests::formatNumber_data()
{
    QTest::addColumn<QString>("digits");
    QTest::addColumn<QString>("formatted");
    QTest::newRow("one-digit") << QString("1") << QString("001");
    QTest::newRow("two-digits") << QString("12") << QString("012");
    QTest::newRow("three-digits") << QString("004") << QString("004");
    QTest::newRow("extra-zeroes") << QString("0001") << QString("001");
    QTest::newRow("zero") << QString("0") << QString("000");
    QTest::newRow("many-zeroes") << QString("00000") << QString("000");
    QTest::newRow("four-digits") << QString("1234") << QString("1234");
    QTest::newRow("long-number") << QString("123456789012345678901234567890")
                                 << QString("123456789012345678901234567890");
}

void FileRenamerTests::formatNumber()
{
    QFETCH(QString, digits);
    QFETCH(QString, formatted);
    QCOMPARE(RenameCore::formatNumber(digits), formatted);
}

void FileRenamerTests::planPadsNumbersToThreeDigits()
{
    const auto plan = RenameCore::plan({"paris_1.pdf", "paris_01.pdf", "paris_1234.pdf"}, {},
                                       "PARIS_P{n}");
    QCOMPARE(plan.copies.size(), 2);
    QCOMPARE(plan.copies.at(0).outputName, QString("PARIS_P001.pdf"));
    QCOMPARE(plan.copies.at(1).outputName, QString("PARIS_P1234.pdf"));
    QCOMPARE(plan.skipped.size(), 1);
    QCOMPARE(plan.skipped.at(0).sourceName, QString("paris_01.pdf"));
    QCOMPARE(plan.skipped.at(0).reason, RenameCore::SkipReason::DuplicateOutput);
}

void FileRenamerTests::normalizeNomenclature()
{
    QCOMPARE(RenameCore::normalizeNomenclature("paris"), QString("paris_{n}"));
    QCOMPARE(RenameCore::normalizeNomenclature("PARIS_P{n}"), QString("PARIS_P{n}"));
    QVERIFY(RenameCore::validateNomenclature("PARIS_P{n}").isEmpty());
}

void FileRenamerTests::planMultiplePlaceholdersAndExtensions()
{
    const auto plan = RenameCore::plan({"paris_004P", "rapport.v2_006.PDF"}, {}, "P{n}-{n}");
    QCOMPARE(plan.skipped.size(), 0);
    QCOMPARE(plan.copies.size(), 2);
    QCOMPARE(plan.copies.at(0).outputName, QString("P004-004"));
    QCOMPARE(plan.copies.at(1).outputName, QString("P006-006.PDF"));

    const auto implicit = RenameCore::plan({"paris_004.pdf"}, {}, "paris");
    QCOMPARE(implicit.copies.size(), 1);
    QCOMPARE(implicit.copies.constFirst().outputName, QString("paris_004.pdf"));
}

void FileRenamerTests::planSkipsSourceWithoutNumber()
{
    const auto plan = RenameCore::plan({"clip.mp4"}, {}, "PARIS_P{n}");
    QVERIFY(plan.copies.isEmpty());
    QCOMPARE(plan.skipped.size(), 1);
    QCOMPARE(plan.skipped.constFirst().sourceName, QString("clip.mp4"));
    QCOMPARE(plan.skipped.constFirst().reason, RenameCore::SkipReason::NoNumber);
}

void FileRenamerTests::planSkipsExistingNameRegardlessOfCase()
{
    const auto plan = RenameCore::plan({"x_1.PDF"}, {"paris_p001.pdf"}, "PARIS_P{n}");
    QVERIFY(plan.copies.isEmpty());
    QCOMPARE(plan.skipped.size(), 1);
    QCOMPARE(plan.skipped.constFirst().sourceName, QString("x_1.PDF"));
    QCOMPARE(plan.skipped.constFirst().reason, RenameCore::SkipReason::AlreadyExists);
}

void FileRenamerTests::planKeepsFirstSourceRegardlessOfExtensionCase()
{
    const auto plan = RenameCore::plan({"a_1.PDF", "B_1.pdf"}, {}, "PARIS_P{n}");
    QCOMPARE(plan.copies.size(), 1);
    QCOMPARE(plan.copies.constFirst().sourceName, QString("a_1.PDF"));
    QCOMPARE(plan.copies.constFirst().outputName, QString("PARIS_P001.PDF"));
    QCOMPARE(plan.skipped.size(), 1);
    QCOMPARE(plan.skipped.constFirst().sourceName, QString("B_1.pdf"));
    QCOMPARE(plan.skipped.constFirst().reason, RenameCore::SkipReason::DuplicateOutput);
}

void FileRenamerTests::previewText_data()
{
    QTest::addColumn<QStringList>("sourceNames");
    QTest::addColumn<QStringList>("existingNames");
    QTest::addColumn<QString>("text");

    QTest::newRow("first-copy-and-counts")
        << QStringList{"a_001.pdf", "b_002.pdf", "clip.mp4", "paris_004.pdf", "rome_005.pdf"}
        << QStringList{"paris_p001.PDF"}
        << QString("Exemple : b_002.pdf → PARIS_P002.pdf\n3 fichiers à copier, 2 ignorés");
    QTest::newRow("singular")
        << QStringList{"clip.mp4", "paris_004.pdf"} << QStringList{}
        << QString("Exemple : paris_004.pdf → PARIS_P004.pdf\n1 fichier à copier, 1 ignoré");
    QTest::newRow("hide-zero-skipped")
        << QStringList{"paris_004.pdf"} << QStringList{}
        << QString("Exemple : paris_004.pdf → PARIS_P004.pdf\n1 fichier à copier");
    QTest::newRow("all-skipped")
        << QStringList{"clip.mp4", "paris_004.pdf"} << QStringList{"paris_p004.pdf"}
        << QString("Aucun fichier à copier, 2 ignorés");
    QTest::newRow("empty-directory")
        << QStringList{} << QStringList{} << QString("Aucun fichier à copier");
    QTest::newRow("literal-name-on-one-line")
        << QStringList{"<b>paris\n_004.pdf"} << QStringList{}
        << QString("Exemple : <b>paris\\n_004.pdf → PARIS_P004.pdf\n1 fichier à copier");
}

void FileRenamerTests::previewText()
{
    QFETCH(QStringList, sourceNames);
    QFETCH(QStringList, existingNames);
    QFETCH(QString, text);
    const auto plan = RenameCore::plan(sourceNames, existingNames, "PARIS_P{n}");
    QCOMPARE(RenameCore::previewText(plan), text);
}

void FileRenamerTests::invalidNomenclature_data()
{
    QTest::addColumn<QString>("nomenclature");

    QTest::newRow("empty") << QString();
    QTest::newRow("slash") << QString("a/b");
    QTest::newRow("backslash") << QString("a\\b");
    QTest::newRow("two-dots") << QString("..");
    QTest::newRow("colon") << QString("x:");
    QTest::newRow("asterisk") << QString("x*");
    QTest::newRow("question-mark") << QString("x?");
    QTest::newRow("quote") << QString("x\"");
    QTest::newRow("less-than") << QString("x<");
    QTest::newRow("greater-than") << QString("x>");
    QTest::newRow("pipe") << QString("x|");
    QTest::newRow("trailing-dot") << QString("fin.");
    QTest::newRow("trailing-space") << QString("fin ");
}

void FileRenamerTests::invalidNomenclature()
{
    QFETCH(QString, nomenclature);
    QVERIFY(!RenameCore::validateNomenclature(nomenclature).isEmpty());
}

void FileRenamerTests::sameDirectory()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString folder = temp.filePath("Folder");
    QVERIFY(RenameCore::sameDirectory({folder, {}}, {temp.filePath("./Folder/"), {}}));
    QVERIFY(!RenameCore::sameDirectory({folder, {}}, {temp.filePath("Other"), {}}));

#ifdef Q_OS_WIN
    QVERIFY(RenameCore::sameDirectory({folder, {}}, {temp.filePath("folder"), {}}));
#else
    QVERIFY(!RenameCore::sameDirectory({folder, {}}, {temp.filePath("folder"), {}}));
#endif
}

void FileRenamerTests::canonicalDirectory()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString canonical = QDir(temp.path()).canonicalPath();
    const QString otherPath = temp.filePath("alias");
    QVERIFY(!canonical.isEmpty());
    QVERIFY(RenameCore::sameDirectory({temp.path(), canonical}, {otherPath, canonical}));
    QVERIFY(!RenameCore::sameDirectory({temp.path(), canonical}, {otherPath, {}}));
    QVERIFY(RenameCore::sameDirectory({temp.path(), canonical}, {temp.path(), {}}));
}

void FileRenamerTests::nomenclatureStore()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString ini = temp.filePath("settings.ini");
    {
        QSettings settings(ini, QSettings::IniFormat);
        NomenclatureStore store(settings);
        QVERIFY(store.entries().isEmpty());
        QVERIFY(store.lastUsed().isEmpty());
        QVERIFY(store.add("PARIS_P{n}"));
        QVERIFY(store.add("LYON"));
        QVERIFY(!store.add("PARIS_P{n}"));
        QVERIFY(!store.add("a/b"));
        QVERIFY(store.remove("LYON"));
        QVERIFY(!store.remove("LYON"));
        store.setLastUsed("unused_in_list");
        settings.sync();
        QCOMPARE(settings.status(), QSettings::NoError);
    }
    {
        QSettings settings(ini, QSettings::IniFormat);
        NomenclatureStore store(settings);
        QCOMPARE(store.entries(), QStringList{"PARIS_P{n}"});
        QCOMPARE(store.lastUsed(), QString("unused_in_list"));
    }
}

void FileRenamerTests::folderAndFileDrops()
{
    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    QVERIFY(QDir(temp.path()).mkdir("folder"));
    QFile file(temp.filePath("folder/report.txt"));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    FolderDropZone field;
    QVERIFY(field.acceptDrops());
    QSignalSpy changes(&field, &FolderDropZone::pathChanged);
    const QPoint position(5, 5);
    const auto sendDrop = [&](const QString &path) {
        QMimeData mime;
        mime.setUrls({QUrl::fromLocalFile(path)});
        QDragEnterEvent enter(position, Qt::CopyAction | Qt::MoveAction, &mime,
                              Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&field, &enter);
        QVERIFY(enter.isAccepted());
        QDragMoveEvent move(position, Qt::CopyAction | Qt::MoveAction, &mime,
                            Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&field, &move);
        QVERIFY(move.isAccepted());
        QDropEvent drop(position, Qt::CopyAction | Qt::MoveAction, &mime,
                        Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(&field, &drop);
        QVERIFY(drop.isAccepted());
        QCOMPARE(drop.dropAction(), Qt::CopyAction);
    };

    sendDrop(temp.path());
    QCOMPARE(field.path(), temp.path());
    sendDrop(file.fileName());
    QCOMPARE(field.path(), temp.filePath("folder"));
    QCOMPARE(changes.count(), 2);
    QCOMPARE(changes.last().constFirst().toString(), temp.filePath("folder"));
}

void FileRenamerTests::rejectsNonLocalDrop()
{
    FolderDropZone field;
    field.setPath("dossier");
    QMimeData mime;
    mime.setUrls({QUrl("https://example.test/report.txt")});
    QDragEnterEvent enter(QPoint(5, 5), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&field, &enter);
    QVERIFY(!enter.isAccepted());

    QDropEvent drop(QPointF(5, 5), Qt::CopyAction, &mime, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(&field, &drop);
    QVERIFY(!drop.isAccepted());
    QCOMPARE(field.path(), QString("dossier"));
}

void FileRenamerTests::zoneRequestsBrowsing()
{
    FolderDropZone zone;
    zone.resize(300, 90);
    QSignalSpy requests(&zone, &FolderDropZone::browseRequested);
    QTest::mouseClick(&zone, Qt::LeftButton, Qt::NoModifier, QPoint(150, 45));
    QCOMPARE(requests.count(), 1);
    QTest::mouseClick(&zone, Qt::RightButton, Qt::NoModifier, QPoint(150, 45));
    QCOMPARE(requests.count(), 1);
    QTest::keyClick(&zone, Qt::Key_Space);
    QCOMPARE(requests.count(), 2);

    // Un clic sur un libellé de la zone compte comme un clic sur la zone.
    zone.setTitle("Dossier source");
    zone.show();
    QVERIFY(QTest::qWaitForWindowExposed(&zone));
    auto *title = zone.findChild<QLabel *>("zoneTitle");
    QVERIFY(title);
    QTest::mouseClick(title, Qt::LeftButton);
    QCOMPARE(requests.count(), 3);
    QVERIFY(zone.path().isEmpty());
}

QTEST_MAIN(FileRenamerTests)
#include "filerenamer_tests.moc"
