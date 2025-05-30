#include <QApplication>
#include <QFile>
#include <QTimer>
#include "mainwindow.h"
#include "splashwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    a.setWindowIcon(QIcon(":/icon.png")); // Ajout de l'icône


    // Créer et afficher le splash screen
    SplashWidget splash;
    splash.show();

    // Charger le fichier de style
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
    } else {
        qDebug() << "Erreur : Impossible d'ouvrir style.qss";
    }

    // Créer la fenêtre principale sur le tas
    MainWindow *w = new MainWindow;

    // Configurer le timer pour fermer le splash et afficher la fenêtre principale
    QTimer::singleShot(1300, [&splash, w]() {
        splash.close();
        w->show();
    });

    // Lancer la boucle d'événements
    int result = a.exec();

    // Nettoyer la mémoire
    delete w;

    return result;
}
