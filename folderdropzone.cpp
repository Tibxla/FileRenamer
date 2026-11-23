#include "folderdropzone.h"

#include "displaytext.h"
#include "elidedlabel.h"

#include <QDir>
#include <QDropEvent>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QTextDocument>
#include <QUrl>
#include <QVBoxLayout>

namespace {

constexpr int Padding = 16;
constexpr int IconSize = 32;
constexpr int IconGap = 14;
constexpr qreal Radius = 8.0;

QString droppedDirectory(const QMimeData *mimeData)
{
    const auto urls = mimeData->urls();
    for (const QUrl &url : urls) {
        if (!url.isLocalFile() || url.toLocalFile().isEmpty()) {
            continue;
        }

        const QFileInfo info(url.toLocalFile());
        if (info.isDir()) {
            return info.absoluteFilePath();
        }
        if (info.isFile()) {
            return info.absolutePath();
        }
    }
    return {};
}

QString acceptDirectoryDrop(QDropEvent *event)
{
    const QString directory = event->possibleActions().testFlag(Qt::CopyAction)
        ? droppedDirectory(event->mimeData()) : QString();
    if (directory.isEmpty()) {
        event->ignore();
        return {};
    }

    // Taking a path from the drop must never cause the source to move a file.
    event->setDropAction(Qt::CopyAction);
    event->accept();
    return directory;
}

void repolish(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

}

FolderDropZone::FolderDropZone(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , hintLabel(new QLabel(this))
    , nameLabel(new ElidedLabel(this))
    , pathLabel(new ElidedLabel(this))
{
    setAcceptDrops(true);
    setAttribute(Qt::WA_Hover);
    setFocusPolicy(Qt::TabFocus);
    setCursor(Qt::PointingHandCursor);
    // Même hauteur vide ou remplie : la mise en page ne saute pas au choix d'un dossier.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(88);

    titleLabel->setObjectName(QStringLiteral("zoneTitle"));
    hintLabel->setObjectName(QStringLiteral("zoneHint"));
    nameLabel->setObjectName(QStringLiteral("zoneName"));
    pathLabel->setObjectName(QStringLiteral("zonePath"));
    titleLabel->setTextFormat(Qt::PlainText);
    hintLabel->setTextFormat(Qt::PlainText);
    hintLabel->setWordWrap(true);
    nameLabel->setElideMode(Qt::ElideRight);
    pathLabel->setElideMode(Qt::ElideMiddle);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(Padding + IconSize + IconGap, Padding, Padding, Padding);
    layout->setSpacing(2);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(pathLabel);
    layout->addStretch();

    updateContent();
}

QString FolderDropZone::title() const
{
    return titleLabel->text();
}

void FolderDropZone::setTitle(const QString &title)
{
    titleLabel->setText(title);
    updateContent();
}

void FolderDropZone::setPlaceholderText(const QString &text)
{
    hintLabel->setText(text);
}

void FolderDropZone::setIcons(const QIcon &idle, const QIcon &chosen)
{
    idleIcon = idle;
    chosenIcon = chosen;
    update();
}

QString FolderDropZone::path() const
{
    return currentPath;
}

void FolderDropZone::setPath(const QString &path)
{
    if (path == currentPath) {
        return;
    }
    currentPath = path;
    updateContent();
    emit pathChanged(currentPath);
}

bool FolderDropZone::isEmpty() const
{
    return currentPath.isEmpty();
}

QSize FolderDropZone::sizeHint() const
{
    return {280, QWidget::sizeHint().height()};
}

QSize FolderDropZone::minimumSizeHint() const
{
    return {200, QWidget::minimumSizeHint().height()};
}

void FolderDropZone::updateContent()
{
    const bool empty = isEmpty();
    hintLabel->setVisible(empty);
    nameLabel->setVisible(!empty);
    pathLabel->setVisible(!empty);
    if (titleLabel->property("emphasis").toBool() != empty) {
        titleLabel->setProperty("emphasis", empty);
        repolish(titleLabel);
    }

    if (empty) {
        setToolTip(QString());
        setAccessibleName(title());
        setAccessibleDescription(hintLabel->text());
    } else {
        const QString native = QDir::toNativeSeparators(currentPath);
        QString name = QFileInfo(QDir::cleanPath(currentPath)).fileName();
        if (name.isEmpty()) {
            name = native;
        }
        nameLabel->setFullText(singleLineText(name));
        pathLabel->setFullText(singleLineText(native));
        setToolTip(Qt::convertFromPlainText(native, Qt::WhiteSpaceNormal));
        setAccessibleName(tr("%1 : %2").arg(title(), singleLineText(native)));
        setAccessibleDescription(QString());
    }
    update();
}

void FolderDropZone::setDragActive(bool active)
{
    if (dragActive != active) {
        dragActive = active;
        update();
    }
}

void FolderDropZone::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const bool empty = isEmpty();
    const bool hovered = isEnabled() && underMouse();
    const bool highlighted = dragActive || hovered;
    QPen pen(highlighted ? accentColor : (empty ? dashColor : borderColor), 1.0);
    if (empty && !dragActive) {
        pen.setDashPattern({5.0, 4.0});
    }
    painter.setPen(pen);
    painter.setBrush(dragActive ? dropFillColor : (hovered ? hoverFillColor : fillColor));
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), Radius, Radius);

    if (hasFocus()) {
        painter.setPen(QPen(focusColor, 2.0));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0), Radius - 0.5, Radius - 0.5);
    }

    const QRect iconRect(Padding, (height() - IconSize) / 2, IconSize, IconSize);
    (empty ? idleIcon : chosenIcon).paint(&painter, iconRect, Qt::AlignCenter,
                                          isEnabled() ? QIcon::Normal : QIcon::Disabled);
}

void FolderDropZone::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        pressed = true;
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void FolderDropZone::mouseReleaseEvent(QMouseEvent *event)
{
    const bool clicked = pressed && event->button() == Qt::LeftButton
        && rect().contains(event->position().toPoint());
    pressed = false;
    if (clicked) {
        event->accept();
        emit browseRequested();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void FolderDropZone::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!event->isAutoRepeat()) {
            event->accept();
            emit browseRequested();
        }
        return;
    default:
        QWidget::keyPressEvent(event);
    }
}

void FolderDropZone::dragEnterEvent(QDragEnterEvent *event)
{
    setDragActive(!acceptDirectoryDrop(event).isEmpty());
}

void FolderDropZone::dragMoveEvent(QDragMoveEvent *event)
{
    acceptDirectoryDrop(event);
}

void FolderDropZone::dragLeaveEvent(QDragLeaveEvent *event)
{
    setDragActive(false);
    QWidget::dragLeaveEvent(event);
}

void FolderDropZone::dropEvent(QDropEvent *event)
{
    setDragActive(false);
    const QString directory = acceptDirectoryDrop(event);
    if (!directory.isEmpty()) {
        setPath(directory);
    }
}
