#ifndef USERTREEWIDGET_H
#define USERTREEWIDGET_H

#include <QApplication>
#include <QTreeWidget>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QString>
#include <QStyledItemDelegate>
#include <QPainter>
#include "uidefinitions.h"

class wavrUserTreeWidgetItem : public QTreeWidgetItem {
public:
    wavrUserTreeWidgetItem();
    ~wavrUserTreeWidgetItem() {}

    QRect checkBoxRect(const QRect& itemRect);
};

class wavrUserTreeWidgetGroupItem : public wavrUserTreeWidgetItem {
public:
    wavrUserTreeWidgetGroupItem() : wavrUserTreeWidgetItem() {}
    ~wavrUserTreeWidgetGroupItem() {}

    void addChild(QTreeWidgetItem* child);
};

class wavrUserTreeWidgetUserItem : public wavrUserTreeWidgetItem {
public:
    wavrUserTreeWidgetUserItem() : wavrUserTreeWidgetItem() {}
    ~wavrUserTreeWidgetUserItem() {}

private:
    bool operator < (const QTreeWidgetItem& other) const;
};

class wavrUserTreeWidgetDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    void drawCheckBox(QPainter* painter, const QPalette& palette, const QRect& checkBoxRect, Qt::CheckState checkState) const;
};

class wavrUserTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    wavrUserTreeWidget(QWidget* parent);
    ~wavrUserTreeWidget() {}

    bool checkable(void);
    void setCheckable(bool enable);
    UserListView view(void);
    void setView(UserListView view);

signals:
    void itemDragDropped(QTreeWidgetItem* item);
    void itemContextMenu(QTreeWidgetItem* item, QPoint& pos);

protected:
    void mousePressEvent(QMouseEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    wavrUserTreeWidgetDelegate* itemDelegate;
    bool dragGroup;
    bool dragUser;
    QString parentId;
    QTreeWidgetItem* dragItem;
    bool expanded;
    bool isCheckable;
    UserListView viewType;
};

#endif // USERTREEWIDGET_H
