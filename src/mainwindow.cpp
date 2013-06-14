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

//    connect(ui.tvUserList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
//            this, SLOT(tvUserList_itemActivated(QTreeWidgetItem*, int)));
//    connect(ui.tvUserList, SIGNAL(itemContextMenu(QTreeWidgetItem*, QPoint&)),
//        this, SLOT(tvUserList_itemContextMenu(QTreeWidgetItem*, QPoint&)));
//    connect(ui.tvUserList, SIGNAL(itemDragDropped(QTreeWidgetItem*)),
//        this, SLOT(tvUserList_itemDragDropped(QTreeWidgetItem*)));
//    connect(ui.tvUserList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
//        this, SLOT(tvUserList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui.tvUserList->installEventFilter(this);

    windowLoaded = true;
}

wavrMainWindow::~wavrMainWindow() {
}

void wavrMainWindow::init(User *pLocalUser, bool connected) {
    setWindowIcon(QIcon(IDR_APPICON));
    this->pLocalUser = pLocalUser;

    //createMainMenu();
    //createToolBar();
    createPresenceToolbar();
    //createAvatarMenu();

    //createTrayMenu();
   // createTrayIcon();
    //connectionStateChanged(connected);

    //createUserMenu();

    //ui.tvUserList->setBackgroundRole(QPalette::Highlight);
    //ui.tvUserList->header()->setStretchLastSection(false);
    //btnStatus->setIconSize(QSize(20,20));
    pSettings = new wavrSettings();
}

void wavrMainWindow::start(void) {
    //	if no avatar is set, select a random avatar (useful when running for the first time)
    if(nAvatar > AVT_COUNT) {
        qsrand((uint)QTime::currentTime().msec());
        nAvatar = qrand() % AVT_COUNT;
    }
    // This method should only be called from here, otherwise an MT_Notify message is sent
    // and the program will connect to the network before start() is called.
    //setAvatar();
   // pTrayIcon->setVisible(showSysTray);
    if(pSettings->value(IDS_AUTOSHOW, IDS_AUTOSHOW_VAL).toBool())
        show();
}

void wavrMainWindow::show(void) {
    windowLoaded = true;
    QWidget::show();
}

void wavrMainWindow::restore(void) {
    //	if window is minimized, restore it to previous state
    if(windowState().testFlag(Qt::WindowMinimized))
        setWindowState(windowState() & ~Qt::WindowMinimized);
    setWindowState(windowState() | Qt::WindowActive);
    raise();	// make main window the top most window of the application
    show();
    activateWindow();	// bring window to foreground
}

void wavrMainWindow::minimize(void) {
    // This function actually hides the window, basically the opposite of restore()
    hide();
    //showMinimizeMessage();
}

void wavrMainWindow::stop(void) {
    //	These settings are saved only if the window was opened at least once by the user
    if(windowLoaded) {
        pSettings->setValue(IDS_WINDOWMAIN, saveGeometry());
        pSettings->setValue(IDS_MINIMIZEMSG, showMinimizeMsg, IDS_MINIMIZEMSG_VAL);
    }

    pSettings->beginWriteArray(IDS_GROUPEXPHDR);
    for(int index = 0; index < ui.tvUserList->topLevelItemCount(); index++) {
        pSettings->setArrayIndex(index);
        pSettings->setValue(IDS_GROUP, ui.tvUserList->topLevelItem(index)->isExpanded());
    }
    pSettings->endArray();

    pTrayIcon->hide();

    //	delete all temp files from cache
    QDir cacheDir(StdLocation::cacheDir());
    if(!cacheDir.exists())
        return;
    QDir::Filters filters = QDir::Files | QDir::Readable;
    QDir::SortFlags sort = QDir::Name;
    //  save all cached coversation to history, then delete the files
    QString filter = "msg_*.tmp";
    wavrMessageLog* pMessageLog = new wavrMessageLog();
    QStringList fileNames = cacheDir.entryList(QStringList() << filter, filters, sort);
    foreach (QString fileName, fileNames) {
        QString filePath = cacheDir.absoluteFilePath(fileName);
        pMessageLog->restoreMessageLog(filePath, false);
        QString szMessageLog = pMessageLog->prepareMessageLogForSave();
        //History::save(pMessageLog->peerName, QDateTime::currentDateTime(), &szMessageLog);
        QFile::remove(filePath);
    }
    pMessageLog->deleteLater();

    //  delete all other temp files
    filter = "*.tmp";
    fileNames = cacheDir.entryList(QStringList() << filter, filters, sort);
    foreach (QString fileName, fileNames) {
        QString filePath = cacheDir.absoluteFilePath(fileName);
        QFile::remove(filePath);
    }
}

void wavrMainWindow::addUser(User *pUser) {
    if(!pUser)
        return;
    qDebug() << "Accessing user info " << pUser->name << "\n" << pUser->status;
    int index = wavrHelper::statusIndexFromCode(pUser->status);

    wavrUserTreeWidgetUserItem *pItem = new wavrUserTreeWidgetUserItem();
    pItem->setData(0, IdRole, pUser->id);
    pItem->setData(0, TypeRole, "User");
    pItem->setData(0, StatusRole, index);
    pItem->setData(0, SubtextRole, pUser->note);
    pItem->setData(0, CapsRole, pUser->caps);
    pItem->setText(0, pUser->name);
    if(statusToolTip)
        pItem->setToolTip(0, wavrStrings::statusDesc()[index]);

    if(index != -1)
        pItem->setIcon(0, QIcon(QPixmap(StatusPic[index], "PNG")));

    //wavrUserTreeWidgetGroupItem* pGroupItem = (wavrUserTreeWidgetGroupItem*)getGroupItem(&pUser->group);
    //pGroupItem->addChild(pItem);
    //pGroupItem->sortChildren(0, Qt::AscendingOrder);

    //  this should be called after item has been added to tree
    setUserAvatar(&pUser->id, &pUser->avatarPath);

//    if(isHidden() || !isActveWindow()) {
//        QString msg = tr("%1 is online");
//        showTrayMessage(TM_Status, msg.arg(pItem->text(0)));
//    }

    //sendAvatar(&pUser->id);
}

void wavrMainWindow::createPresenceToolbar(void) {
    pCmbPresence = new QComboBox(this);
    for(int index = 0; index < ST_COUNT; index++) {
        pCmbPresence->addItem(QIcon(QPixmap(StatusPic[index], "PNG")), wavrStrings::statusDesc()[index]);
    }
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(pCmbPresence, 0);
    ui.presence_toolbar->setLayout(hbox);
}

void wavrMainWindow::createSearchToolbar(void) {

}

void wavrMainWindow::showMinimizeMessage(void) {
    if(showMinimizeMsg) {
        QString msg = tr("%1 will continue to run in the background. Activate this icon to restore the application window.");
        //showTrayMessage(TM_Minimize, msg.arg(wavrStrings::appName()));
        showMinimizeMsg = false;
    }
}

void wavrMainWindow::sendAvatar(QString* lpszUserId) {
    QString filePath = StdLocation::avatarFile();
    if(!QFile::exists(filePath))
        return;

    //sendMessage(MT_Avatar, lpszUserId, &filePath);
}

void wavrMainWindow::setUserAvatar(QString* lpszUserId, QString *lpszFilePath) {
    QTreeWidgetItem* pUserItem = getUserItem(lpszUserId);
    if(!pUserItem)
        return;

    QPixmap avatar;
    if(!lpszFilePath || !QFile::exists(*lpszFilePath))
        avatar.load(AVT_DEFAULT);
    else
        avatar.load(*lpszFilePath);
    avatar = avatar.scaled(QSize(32, 32), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pUserItem->setData(0, AvatarRole, QIcon(avatar));
}

QTreeWidgetItem* wavrMainWindow::getUserItem(QString* lpszUserId) {
    for(int topIndex = 0; topIndex < ui.tvUserList->topLevelItemCount(); topIndex++) {
        for(int index = 0; index < ui.tvUserList->topLevelItem(topIndex)->childCount(); index++) {
            QTreeWidgetItem* pItem = ui.tvUserList->topLevelItem(topIndex)->child(index);
            if(pItem->data(0, IdRole).toString().compare(*lpszUserId) == 0)
                return pItem;
        }
    }

    return NULL;
}
