#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QLibraryInfo>
#include <QStringList>
#include <QStyleHints>
#include <QTranslator>

namespace {

bool loadFrenchTranslation(QTranslator &translator)
{
    const QStringList directories{
        QLibraryInfo::path(QLibraryInfo::TranslationsPath),
        QCoreApplication::applicationDirPath() + QStringLiteral("/translations")
    };
    for (const QString &directory : directories) {
        if (translator.load(QStringLiteral("qt_fr"), directory) ||
            translator.load(QStringLiteral("qtbase_fr"), directory)) {
            return true;
        }
    }
    return false;
}

void applySystemTheme(QApplication &app)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const bool dark = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    const bool dark = QGuiApplication::palette().window().color().lightness() < 128;
#endif
    QFile stylesheet(dark ? QStringLiteral(":/style.qss") : QStringLiteral(":/light.qss"));
    if (stylesheet.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(QString::fromUtf8(stylesheet.readAll()));
    }
}

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Tibxla"));
    QCoreApplication::setApplicationName(QStringLiteral("FileRenamer"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icon.png")));

    QTranslator french;
    if (loadFrenchTranslation(french)) {
        app.installTranslator(&french);
    }
    applySystemTheme(app);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                     &app, [&app] { applySystemTheme(app); });
#endif

    MainWindow window;
    window.show();
    return app.exec();
}
