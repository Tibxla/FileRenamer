#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer> // Ajout pour les minuteurs

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int digitCount;        // Nombre de chiffres pour la numérotation
    QString regexPattern;  // Motif de détection des chiffres
    QString currentTheme;  // Thème actuel
    QString regexOption;   // Option choisie pour le motif
    QString letterBefore;  // Lettre avant le nombre
    QString letterAfter;   // Lettre après le nombre
    QString customRegex;   // Regex personnalisé
    QString lastSourceDir;       // Dernier dossier source utilisé
    QString lastDestDir;         // Dernier nouveau dossier utilisé
    QString lastPdfDestDir;      // Dernier dossier de destination pour PDFs

    QTimer *virusTimer;    // Minuteur pour les 5 minutes
    QTimer *popupTimer;    // Minuteur pour ouvrir les fenêtres
    int popupCount;        // Compteur pour les fenêtres
    bool virusSimulationEnabled; // État de la simulation de virus

    void saveSettings();   // Sauvegarde les paramètres
    void loadSettings();   // Charge les paramètres
    QString getRegexPattern();  // Récupère le motif regex selon l'option choisie

private slots:
    void on_renameButton_clicked();
    void on_browseButton_clicked();
    void on_browseDestButton_clicked();
    void on_browseDossierDestinationButton_clicked();
    void on_createPdfButton_clicked();
    void on_themeChanged();
    void on_regexOptionChanged();
    void onVirusTimerTimeout(); // Slot pour le minuteur de 5 minutes
    void onPopupTimerTimeout(); // Slot pour ouvrir les fenêtres
    void on_virusSimulationCheckBox_toggled(bool checked); // Slot pour la case à cocher
};
#endif // MAINWINDOW_H
