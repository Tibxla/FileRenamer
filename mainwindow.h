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

class FolderDropZone;

namespace RenameCore {
struct Plan;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    // Ce qu'affiche le bas de la carte : l'aperçu, la copie en cours ou son résultat.
    enum class Panel {
        Preview,
        Progress,
        Result
    };

    void browseDirectory(FolderDropZone *zone, const QString &title);
    void inputsChanged();
    void renameFiles();
    void saveNomenclature();
    void removeNomenclature();
    void updateNomenclatureButtons();
    void refreshPreviewDirectories();
    void updatePreview();
    void showMessage(const QString &text, bool error, bool offerPrefix = false);
    void showSeries(const RenameCore::Plan &plan);
    void setPrefixOpen(bool open, bool focus = false);
    QString imposedPrefix() const;
    void showPanel(Panel panel);
    void setBusy(bool busy);
    void showResult(qsizetype copied, const QStringList &ignored, bool cancelled);
    void setIgnoredExpanded(bool expanded);
    void updateStretch();

    std::unique_ptr<Ui::MainWindow> ui;
    QSettings settings;
    NomenclatureStore nomenclatures;
    QTimer previewRefreshTimer;
    bool previewSourceExists = false;
    bool previewSameDirectory = false;
    QStringList previewSourceNames;
    QStringList previewExistingNames;
    Panel panel = Panel::Preview;
    bool copying = false;
    bool prefixOpen = false;
    bool cancelRequested = false;
    QString resultFolder;
};

#endif
