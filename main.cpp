#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Tibxla"));
    QCoreApplication::setApplicationName(QStringLiteral("FileRenamer"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icon.png")));

    MainWindow window;
    window.show();
    return app.exec();
}
