#ifndef FOLDERDROPZONE_H
#define FOLDERDROPZONE_H

#include <QColor>
#include <QIcon>
#include <QWidget>

class ElidedLabel;
class QLabel;

// Grande zone cliquable qui reçoit un dossier par glisser-déposer (un fichier déposé donne son
// dossier parent) ou, au clic et au clavier, demande la boîte de choix de dossier.
// Les couleurs viennent de la feuille de style (qproperty-…), le cadre est peint ici.
class FolderDropZone : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor borderColor MEMBER borderColor)
    Q_PROPERTY(QColor dashColor MEMBER dashColor)
    Q_PROPERTY(QColor accentColor MEMBER accentColor)
    Q_PROPERTY(QColor fillColor MEMBER fillColor)
    Q_PROPERTY(QColor hoverFillColor MEMBER hoverFillColor)
    Q_PROPERTY(QColor dropFillColor MEMBER dropFillColor)
    Q_PROPERTY(QColor focusColor MEMBER focusColor)

public:
    explicit FolderDropZone(QWidget *parent = nullptr);

    QString title() const;
    void setTitle(const QString &title);
    void setPlaceholderText(const QString &text);
    // Icônes du dossier vide et du dossier choisi.
    void setIcons(const QIcon &idle, const QIcon &chosen);

    QString path() const;
    void setPath(const QString &path);
    bool isEmpty() const;

    // Largeur souhaitée indépendante du chemin affiché : deux zones côte à côte restent égales.
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void pathChanged(const QString &path);
    void browseRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void updateContent();
    void setDragActive(bool active);

    QLabel *titleLabel;
    QLabel *hintLabel;
    ElidedLabel *nameLabel;
    ElidedLabel *pathLabel;
    QIcon idleIcon;
    QIcon chosenIcon;
    QString currentPath;
    bool dragActive = false;
    bool pressed = false;

    QColor borderColor{0xE5, 0xE5, 0xE5};
    QColor dashColor{0xC4, 0xC4, 0xC4};
    QColor accentColor{0x00, 0x67, 0xC0};
    QColor fillColor{0xFF, 0xFF, 0xFF};
    QColor hoverFillColor{0xF9, 0xF9, 0xF9};
    QColor dropFillColor{0xEB, 0xF3, 0xFB};
    QColor focusColor{0x1B, 0x1B, 0x1B};
};

#endif
