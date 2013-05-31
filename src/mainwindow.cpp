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


#include <QDesktopServices>
#include <QTimer>
#include <QUrl>
#include "mainwindow.h"
#include "messagelog.h"

wavrMainWindow::wavrMainWindow(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags) {
    ui.setupUi(this);

    connect(ui.tvUserList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(tvUserList_itemActivated(QTreeWidgetItem*, int)));
    connect(ui.tvUserList, SIGNAL(itemContextMenu(QTreeWidgetItem*, QPoint&)),
        this, SLOT(tvUserList_itemContextMenu(QTreeWidgetItem*, QPoint&)));
    connect(ui.tvUserList, SIGNAL(itemDragDropped(QTreeWidgetItem*)),
        this, SLOT(tvUserList_itemDragDropped(QTreeWidgetItem*)));
    connect(ui.tvUserList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this, SLOT(tvUserList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui.tvUserList->installEventFilter(this);

    windowLoaded = false;
}

wavrMainWindow::~wavrMainWindow() {
}

void wavrMainWindow::init(User *pLocalUser, bool connected) {
    setWindowIcon(QIcon(IDR_APPICON));

    this->pLocalUser = pLocalUser;

}
