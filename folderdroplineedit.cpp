#include "folderdroplineedit.h"

#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

namespace {

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

}

FolderDropLineEdit::FolderDropLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
}

void FolderDropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    acceptDirectoryDrop(event);
}

void FolderDropLineEdit::dragMoveEvent(QDragMoveEvent *event)
{
    acceptDirectoryDrop(event);
}

void FolderDropLineEdit::dropEvent(QDropEvent *event)
{
    const QString directory = acceptDirectoryDrop(event);
    if (!directory.isEmpty()) {
        setText(directory);
    }
}
