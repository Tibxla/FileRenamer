#ifndef SPLASHWIDGET_H
#define SPLASHWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

class SplashWidget : public QWidget {
public:
    SplashWidget(QWidget *parent = nullptr) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setFixedSize(512, 216);  // Définit une taille fixe raisonnable

        // Layout vertical
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);  // Supprimer les marges

        // Label pour l'animation (GIF)
        QLabel *animationLabel = new QLabel;
        QMovie *movie = new QMovie(":/animation.gif");

        // Vérifier si le GIF est valide
        if (!movie->isValid()) {
            qDebug() << "Erreur : Impossible de charger animation.gif";
            return;
        }

        animationLabel->setMovie(movie);
        animationLabel->setScaledContents(true);  // Redimensionne le GIF pour s'adapter au label
        movie->start();
        layout->addWidget(animationLabel);

        // Centrer le widget sur l'écran
        QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
};

#endif // SPLASHWIDGET_H
