#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nomenclaturestore.h"

#include <QMainWindow>
#include <QSettings>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void browseDirectory(QLineEdit *field, const QString &title);
    void renameFiles();
    void saveNomenclature();
    void removeNomenclature();
    void updateNomenclatureButtons();
    void applyTheme(bool light);

    std::unique_ptr<Ui::MainWindow> ui;
    QSettings settings;
    NomenclatureStore nomenclatures;
};

#endif
