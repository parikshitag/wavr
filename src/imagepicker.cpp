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


#include <QMenu>
#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>
#include "uidefinitions.h"
#include "imagepicker.h"

wavrImagePicker::wavrImagePicker(QWidget *parent, QList<QString>* source, int picSize, int columns, int* selected, int actionIndex)
    : QTableWidget(parent)
{
    setMouseTracking(true);

    setBackgroundRole(QPalette::Window);
    setIconSize(QSize(picSize, picSize));
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    setShowGrid(false);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    setStyleSheet("QTableWidget { padding: 4px }");	// padding around table

    max_col = columns;
    int max_row = qCeil(source->count() / (qreal)max_col);

    setColumnCount(max_col);
    setRowCount(max_row);

    int cellSize = picSize + 8;
    verticalHeader()->setDefaultSectionSize(cellSize);
    verticalHeader()->setMinimumSectionSize(cellSize);
    horizontalHeader()->setDefaultSectionSize(cellSize);
    horizontalHeader()->setMinimumSectionSize(cellSize);

    //	set min and max size of table, with padding included
    setMinimumSize(max_col * cellSize + 8, max_row * cellSize + 8);
    setMaximumSize(max_col * cellSize + 8, max_row * cellSize + 8);

    for(int i = 0; i < max_row; i++) {
        for(int j = 0; j < max_col; j++) {
            int k = (i * max_col) + j;

            QTableWidgetItem* item = new QTableWidgetItem();
            item->setData(TypeRole, 0);
            if(k < source->count()) {
                item->setIcon(QIcon(source->value(k)));
                item->setData(TypeRole, 1);
                item->setSizeHint(QSize(picSize, picSize));
                item->setBackground(this->palette().window());
            }
            setItem(i, j, item);
        }
    }

    this->actionIndex = actionIndex;
    this->selected = selected;
    this->hoverItem = NULL;
}

wavrImagePicker::~wavrImagePicker() {
}

void wavrImagePicker::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    QMenu* pMenu = (QMenu*)this->parent();
    if(current.isValid()) {
        int index = current.row() * max_col + current.column();
        if(current.data(TypeRole).toInt() == 1) {
            *selected = index;
            pMenu->actions()[actionIndex]->trigger();
        }
        else {
            setCurrentIndex(previous);
            return;
        }
    }
    hoverItem = NULL;
    setCurrentIndex(QModelIndex());
    pMenu->close();
}

void wavrImagePicker::mouseMoveEvent(QMouseEvent* e) {
    QTableWidget::mouseMoveEvent(e);

    QTableWidgetItem* currentItem = itemAt(e->pos());
    if(currentItem != hoverItem) {
        hoverItem = currentItem;
        if(hoverItem)
            update(visualItemRect(hoverItem));
    }
}

void wavrImagePicker::paintEvent(QPaintEvent* e) {
    QTableWidget::paintEvent(e);

    //	If mouse is hovered over an item, draw a border around it
    if(hoverItem) {
        QStyleOptionFrame opt;
        opt.rect = visualItemRect(hoverItem);
        opt.rect.adjust(0, 1, -2, -1);
        QPainter painter(viewport());
        style()->drawPrimitive(QStyle::PE_FrameButtonBevel, &opt, &painter);
    }
}

void wavrImagePicker::leaveEvent(QEvent* e) {
    QTableWidget::leaveEvent(e);

    hoverItem = NULL;
}
