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


#include "usertreewidget.h"

wavrUserTreeWidgetItem::wavrUserTreeWidgetItem() : QTreeWidgetItem(UserType + 1) {
    //	make item not user checkable
    setFlags(flags() & ~Qt::ItemIsUserCheckable);
}

QRect wavrUserTreeWidgetItem::checkBoxRect(const QRect& itemRect) {
    QRect checkBoxRect(0, 0, 0, 0);

    if(data(0, TypeRole).toString() == "Group")
        checkBoxRect = QRect(itemRect.left(), itemRect.top() + ((itemRect.height() - 12) / 2), 0, 0);
    else if(data(0, TypeRole).toString() == "User")
        checkBoxRect = QRect(itemRect.left(), itemRect.top() + 4, 0, 0);

    wavrUserTreeWidget* treeWidget = static_cast<wavrUserTreeWidget*>(this->treeWidget());
    if(treeWidget->checkable()) {
        checkBoxRect.setSize(QSize(12, 12));
        checkBoxRect.moveLeft(checkBoxRect.left() + 3);
    }

    return checkBoxRect;
}

void wavrUserTreeWidgetGroupItem::addChild(QTreeWidgetItem* child) {
    wavrUserTreeWidget* treeWidget = static_cast<wavrUserTreeWidget*>(this->treeWidget());
    child->setSizeHint(0, QSize(0, itemViewHeight[treeWidget->view()]));
    QTreeWidgetItem::addChild(child);
}

bool wavrUserTreeWidgetUserItem::operator < (const QTreeWidgetItem& other) const {
    int column = treeWidget()->sortColumn();
    if(column == 0) {
        //	sort based on status and user name
        if(data(column, StatusRole).toInt() < other.data(column, StatusRole).toInt())
            return true;
        else if(data(column, StatusRole).toInt() > other.data(column, StatusRole).toInt())
            return false;
        else
            return text(column).toLower() < other.text(column).toLower();
    }

    return text(column).toLower() < other.text(column).toLower();
}

void wavrUserTreeWidgetDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    QPalette palette = QApplication::palette();
    QRect itemRect = option.rect;
    wavrUserTreeWidgetItem* pItem = static_cast<wavrUserTreeWidgetItem*>(index.internalPointer());
    wavrUserTreeWidget* pTreeWidget = static_cast<wavrUserTreeWidget*>(pItem->treeWidget());
    QString type = pItem->data(0, TypeRole).toString();
    QString name = pItem->data(0, Qt::DisplayRole).toString();

    int padding = 2;

    if(type == "Group") {
        //	fill the entire item area with window background color (usually white)
        painter->fillRect(itemRect, palette.window());

        //	Fill the background of the item with highlight color, and draw a border with a darker shade
        QColor fillColor = palette.color(QPalette::Highlight);
        QColor borderColor = fillColor.darker(130);
        painter->setPen(QPen(borderColor));
        painter->setBrush(QBrush(fillColor));
        itemRect.adjust(1, 1, -2, -1);
        painter->drawRect(itemRect);

        //	Draw checkbox if needed
        QRect checkBoxRect = pItem->checkBoxRect(itemRect);
        if(pTreeWidget->checkable())
            drawCheckBox(painter, palette, checkBoxRect, pItem->checkState(0));

        //	Draw the expand/collapse arrow
        painter->setBrush(QBrush(palette.color(QPalette::HighlightedText)));
        painter->setPen(QPen(palette.color(QPalette::HighlightedText)));
        QPoint points[3];
        if(option.state.testFlag(QStyle::State_Children) && option.state.testFlag(QStyle::State_Open)) {
            points[0] = QPoint(checkBoxRect.right() + 4, itemRect.top() + ((itemRect.height() - 4) / 2));
            points[1] = QPoint(checkBoxRect.right() + 12, itemRect.top() + ((itemRect.height() - 4) / 2));
            points[2] = QPoint(checkBoxRect.right() + 8, itemRect.top() + ((itemRect.height() - 4) / 2) + 4);
        } else {
            points[0] = QPoint(checkBoxRect.right() + 6, itemRect.top() + ((itemRect.height() - 7) / 2));
            points[1] = QPoint(checkBoxRect.right() + 10, itemRect.top() + ((itemRect.height() - 7) / 2) + 4);
            points[2] = QPoint(checkBoxRect.right() + 6, itemRect.top() + ((itemRect.height() - 7) / 2) + 8);
        }
        painter->drawPolygon(points, 3);

        //	Draw the text
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);
        int textFlags = Qt::AlignLeft | Qt::AlignVCenter;
        //	Leave a padding of 5px on left and right
        int leftPad = checkBoxRect.width() > 0 ? checkBoxRect.right() + 16 : 16;
        QRect textRect = itemRect.adjusted(leftPad, padding, -5, -padding);
        painter->setPen(QPen(palette.color(QPalette::HighlightedText)));
        QString text = elidedText(painter->fontMetrics(), textRect.width(), Qt::ElideRight, name);
        painter->drawText(textRect, textFlags, text);
    } else if(type == "User") {
        QColor fillColor, borderColor;
        if(index.row() % 2 == 1)
            borderColor = fillColor = palette.color(QPalette::AlternateBase);
        else
            borderColor = fillColor = palette.color(QPalette::Base);

        if(option.state.testFlag(QStyle::State_Active) && option.state.testFlag(QStyle::State_Enabled)
                && option.state.testFlag(QStyle::State_Selected))
            borderColor = palette.color(QPalette::Shadow);

        painter->setPen(QPen(borderColor));
        painter->setBrush(QBrush(fillColor));
        itemRect.adjust(1, 0, -2, -1);
        painter->drawRect(itemRect);

        //	Draw checkbox if needed
        QRect checkBoxRect = pItem->checkBoxRect(itemRect);
        if(pTreeWidget->checkable())
            drawCheckBox(painter, palette, checkBoxRect, pItem->checkState(0));

        //	Draw the status image
        QPixmap statusImage = pItem->icon(0).pixmap(QSize(16, 16));
        QRect statusRect = itemRect.adjusted(itemRect.width(), 5 * padding , 0, 0);
        statusRect.setLeft(statusRect.right() - statusImage.width() - 2 * padding);
        //statusRect.setTop(statusRect.top() + statusImage.height() + padding);
        statusRect.setSize(statusImage.size());        
        painter->drawPixmap(statusRect, statusImage);

        //	Draw the avatar image

        int leftPad = checkBoxRect.width() > 0 ? checkBoxRect.right() + 5 : 5;
        QRect avatarRect = itemRect.adjusted(leftPad, padding, 0, 0);
        if(pTreeWidget->view() == ULV_Detailed) {
            QVariant avatar = pItem->data(0, AvatarRole);
            if(!avatar.isNull()) {
                QPixmap avatarImage = ((QIcon)pItem->data(0, AvatarRole).value<QIcon>()).pixmap(32, 32);
                //avatarRect.setLeft(avatarRect.right() - avatarImage.width() - padding);
                avatarRect.setSize(avatarImage.size());
                painter->drawPixmap(avatarRect, avatarImage);
            }
        }

        //	Draw the text
        painter->setPen(QPen(palette.color(QPalette::WindowText)));
        QString note = pItem->data(0, SubtextRole).toString();
        int textFlags = Qt::AlignLeft;
        textFlags |= (note.isNull()  || note.isEmpty() ?  Qt::AlignVCenter : Qt::AlignTop);
        //	Leave a padding of 5px on left and right
        QRect textRect = itemRect.adjusted(avatarRect.right() + 5, padding, -(5 + statusRect.width() + padding), -padding);
        QString text = elidedText(painter->fontMetrics(), textRect.width(), Qt::ElideRight, name);
        painter->drawText(textRect, textFlags, text);

        //	Draw sub text
        if(pTreeWidget->view() == ULV_Detailed) {
            if(!note.isNull()) {
                painter->setPen(QPen(GRAY_TEXT_COLOR));
                textFlags = Qt::AlignLeft | Qt::AlignBottom;
                text = elidedText(painter->fontMetrics(), textRect.width(), Qt::ElideRight, note);
                painter->drawText(textRect, textFlags, text);
            }
        }

    }

    painter->restore();
}

void wavrUserTreeWidgetDelegate::drawCheckBox(QPainter *painter, const QPalette& palette,
                                        const QRect &checkBoxRect, Qt::CheckState checkState) const {
    painter->setPen(QPen(palette.color(QPalette::Shadow)));
    painter->setBrush(palette.base());
    painter->drawRect(checkBoxRect);

    if(checkState == Qt::Checked) {
        painter->setPen(QPen(palette.text(), 1));
        painter->drawLine(checkBoxRect.left() + 2, checkBoxRect.top() + 6, checkBoxRect.left() + 4, checkBoxRect.top() + 8);
        painter->drawLine(checkBoxRect.left() + 2, checkBoxRect.top() + 7, checkBoxRect.left() + 4, checkBoxRect.top() + 9);
        painter->drawLine(checkBoxRect.left() + 2, checkBoxRect.top() + 8, checkBoxRect.left() + 4, checkBoxRect.top() + 10);
        painter->drawLine(checkBoxRect.left() + 4, checkBoxRect.top() + 8, checkBoxRect.left() + 10, checkBoxRect.top() + 2);
        painter->drawLine(checkBoxRect.left() + 4, checkBoxRect.top() + 9, checkBoxRect.left() + 10, checkBoxRect.top() + 3);
        painter->drawLine(checkBoxRect.left() + 4, checkBoxRect.top() + 10, checkBoxRect.left() + 10, checkBoxRect.top() + 4);
    }
}

wavrUserTreeWidget::wavrUserTreeWidget(QWidget* parent) : QTreeWidget(parent) {
    itemDelegate = new wavrUserTreeWidgetDelegate();
    setItemDelegate(itemDelegate);

    isCheckable = false;
    viewType = ULV_Detailed;
}

bool wavrUserTreeWidget::checkable(void) {
    return isCheckable;
}

void wavrUserTreeWidget::setCheckable(bool enable) {
    isCheckable = enable;
}

UserListView wavrUserTreeWidget::view(void) {
    return viewType;
}

void wavrUserTreeWidget::setView(UserListView view) {
    viewType = view;

    //	Set the item heights for the selected view type
    for(int index = 0; index < topLevelItemCount(); index++) {
        QTreeWidgetItem* item = topLevelItem(index);
        for(int childIndex = 0; childIndex < item->childCount(); childIndex++)
            item->child(childIndex)->setSizeHint(0, QSize(0, itemViewHeight[viewType]));
    }
}

void wavrUserTreeWidget::mousePressEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        QTreeWidgetItem* item = itemAt(event->pos());

        dragGroup = false;
        dragUser = false;
        dragItem = NULL;
        parentId = QString::null;
        expanded = false;

        if(item) {
            dragItem = item;
            if(dynamic_cast<wavrUserTreeWidgetGroupItem*>(item)) {
                dragGroup = true;
                expanded = dragItem->isExpanded();
            }
            else if(dynamic_cast<wavrUserTreeWidgetUserItem*>(item)) {
                dragUser = true;
                parentId = dragItem->parent()->data(0, IdRole).toString();
            }
        }
    }

    QTreeWidget::mousePressEvent(event);
}

void wavrUserTreeWidget::dragMoveEvent(QDragMoveEvent* event) {
    QTreeWidget::dragMoveEvent(event);

    QTreeWidgetItem* item = itemAt(event->pos());
    bool accept = false;

    if(dragUser) {
        if(item && dynamic_cast<wavrUserTreeWidgetGroupItem*>(item) && visualItemRect(item).contains(event->pos(), true))
            accept = true;
    }
    else if(dragGroup) {
        if(!item || (dynamic_cast<wavrUserTreeWidgetGroupItem*>(item) && !visualItemRect(item).contains(event->pos(), true)))
            accept = true;
    }

    accept ? event->accept() : event->ignore();
}

void wavrUserTreeWidget::dropEvent(QDropEvent* event) {
    QTreeWidget::dropEvent(event);

    if(dragUser) {
        if(!dragItem->parent()) {
        //	user item dragged to group level. revert
            for(int index = 0; index < topLevelItemCount(); index++) {
                QTreeWidgetItem* parentItem = topLevelItem(index);
                if(parentItem->data(0, IdRole).toString().compare(parentId) == 0) {
                    takeTopLevelItem(indexOfTopLevelItem(dragItem));
                    parentItem->addChild(dragItem);
                    return;
                }
            }
        }
    }
    else if(dragGroup) {
        dragItem->setExpanded(expanded);
    }

    if(dragItem)
        emit itemDragDropped(dragItem);
}

void wavrUserTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
    QTreeWidget::contextMenuEvent(event);

    QTreeWidgetItem* item = itemAt(event->pos());
    QPoint pos = event->globalPos();
    if(item && event->reason() != QContextMenuEvent::Mouse) {
        QRect itemRect = visualItemRect(item);
        pos = itemRect.bottomLeft();
        pos.ry() += itemRect.height();
        pos = mapToGlobal(pos);
    }

    emit itemContextMenu(item, pos);
}

void wavrUserTreeWidget::mouseReleaseEvent(QMouseEvent* event) {
    QTreeWidget::mouseReleaseEvent(event);

    QPoint pos = event->pos();
    wavrUserTreeWidgetItem* item = static_cast<wavrUserTreeWidgetItem*>(itemAt(pos));
    if(item && checkable() && item->checkBoxRect(visualItemRect(item)).contains(pos)) {
        // toggle checkstate
        if(item->checkState(0) == Qt::Checked)
            item->setCheckState(0, Qt::Unchecked);
        else
            item->setCheckState(0, Qt::Checked);
    }
}

void wavrUserTreeWidget::keyPressEvent(QKeyEvent* event) {
    QTreeWidget::keyPressEvent(event);

    if(event->key() == Qt::Key_Space && selectedItems().count() > 0) {
        wavrUserTreeWidgetItem* item = static_cast<wavrUserTreeWidgetItem*>(selectedItems().at(0));
        if(item && checkable()) {
            // toggle checkstate
            if(item->checkState(0) == Qt::Checked)
                item->setCheckState(0, Qt::Unchecked);
            else
                item->setCheckState(0, Qt::Checked);
        }
    }
}
