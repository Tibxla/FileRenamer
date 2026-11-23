#include "elidedlabel.h"

#include <QEvent>
#include <QFontMetricsF>
#include <QtMath>
#include <QTextDocument>

ElidedLabel::ElidedLabel(QWidget *parent)
    : QLabel(parent)
{
    setTextFormat(Qt::PlainText);
    setWordWrap(false);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

QString ElidedLabel::fullText() const
{
    return text;
}

void ElidedLabel::setFullText(const QString &fullText)
{
    if (text == fullText) {
        return;
    }
    text = fullText;
    updateElision();
    updateGeometry();
}

void ElidedLabel::setElideMode(Qt::TextElideMode mode)
{
    elideMode = mode;
    updateElision();
}

void ElidedLabel::setToolTipWhenElided(bool enabled)
{
    toolTipWhenElided = enabled;
    updateElision();
}

QSize ElidedLabel::sizeHint() const
{
    const QMargins margins = contentsMargins();
    const int extra = margins.left() + margins.right() + 2 * margin();
    // Arrondi par excès : elidedText raccourcit dès qu'il manque une fraction de pixel.
    return {qCeil(QFontMetricsF(font()).horizontalAdvance(text)) + 1 + extra,
            fontMetrics().height() + margins.top() + margins.bottom() + 2 * margin()};
}

QSize ElidedLabel::minimumSizeHint() const
{
    const QSize hint = sizeHint();
    const QMargins margins = contentsMargins();
    const int ellipsis = fontMetrics().horizontalAdvance(QStringLiteral("…"));
    return {qMin(hint.width(), 3 * ellipsis + margins.left() + margins.right() + 2 * margin()), hint.height()};
}

void ElidedLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    updateElision();
}

void ElidedLabel::changeEvent(QEvent *event)
{
    QLabel::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
        updateElision();
        updateGeometry();
    }
}

void ElidedLabel::updateElision()
{
    const QString shown = fontMetrics().elidedText(text, elideMode, qMax(0, contentsRect().width() - 2 * margin()));
    if (QLabel::text() != shown) {
        QLabel::setText(shown);
    }
    if (toolTipWhenElided) {
        setToolTip(shown == text ? QString() : Qt::convertFromPlainText(text, Qt::WhiteSpaceNormal));
    }
}
