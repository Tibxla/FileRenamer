#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nomenclaturestore.h"

#include <QMainWindow>
#include <QSettings>
#include <QStringList>
#include <QTimer>

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

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void browseDirectory(QLineEdit *field, const QString &title);
    void renameFiles();
    void saveNomenclature();
    void removeNomenclature();
    void updateNomenclatureButtons();
    void refreshPreviewDirectories();
    void updatePreview();
    void setPreview(const QString &text, bool error = false);
    void fitPreview();

    std::unique_ptr<Ui::MainWindow> ui;
    QSettings settings;
    NomenclatureStore nomenclatures;
    QTimer previewRefreshTimer;
    bool previewSourceExists = false;
    bool previewSameDirectory = false;
    QStringList previewSourceNames;
    QStringList previewExistingNames;
    QString previewText;
};

#endif
