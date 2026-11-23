#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <QIcon>
#include <QWidget>

// Affiche une icône à la taille du widget, repeinte à chaque fois (elle suit donc le thème).
class IconView : public QWidget
{
    Q_OBJECT

public:
    explicit IconView(QWidget *parent = nullptr);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QIcon currentIcon;
};

#endif
