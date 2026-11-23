#include "iconview.h"

#include <QPainter>

IconView::IconView(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

QIcon IconView::icon() const
{
    return currentIcon;
}

void IconView::setIcon(const QIcon &icon)
{
    currentIcon = icon;
    update();
}

void IconView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    currentIcon.paint(&painter, rect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
}
