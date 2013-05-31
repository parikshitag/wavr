/****************************************************************************
**
** This file is part of Wavr IM Application.
** 
** Copyright (c) 2013 Parikshit Agarwal.
** 
** Contact:  parikshit.ag@gmail.com
** 
** Wavr is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Wavr is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Wavr.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


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
