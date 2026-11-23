#ifndef FOLDERDROPLINEEDIT_H
#define FOLDERDROPLINEEDIT_H

#include <QLineEdit>

class FolderDropLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit FolderDropLineEdit(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif
