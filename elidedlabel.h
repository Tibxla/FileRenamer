#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>

// Libellé d'une ligne en texte brut qui raccourcit son texte avec « … » quand la place manque.
class ElidedLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ElidedLabel(QWidget *parent = nullptr);

    QString fullText() const;
    void setFullText(const QString &text);
    void setElideMode(Qt::TextElideMode mode);
    // Affiche le texte complet en infobulle quand il est raccourci.
    void setToolTipWhenElided(bool enabled);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateElision();

    QString text;
    Qt::TextElideMode elideMode = Qt::ElideRight;
    bool toolTipWhenElided = false;
};

#endif
