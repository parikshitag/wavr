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
#include <QLineEdit>

wavrMainWindow::wavrMainWindow(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags) {
    ui.setupUi(this);

    connect(ui.tvUserList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(tvUserList_itemActivated(QTreeWidgetItem*, int)));
    connect(ui.tvUserList, SIGNAL(itemContextMenu(QTreeWidgetItem*, QPoint&)),
        this, SLOT(tvUserList_itemContextMenu(QTreeWidgetItem*, QPoint&)));
//    connect(ui.tvUserList, SIGNAL(itemDragDropped(QTreeWidgetItem*)),
//        this, SLOT(tvUserList_itemDragDropped(QTreeWidgetItem*)));
    connect(ui.tvUserList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this, SLOT(tvUserList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui.cmbPresence->lineEdit(), SIGNAL(returnPressed()), this, SLOT(cmbPresence_returnPressed()));
    connect(ui.cmbPresence->lineEdit(), SIGNAL(editingFinished()), this, SLOT(cmbPresence_editingFinished()));
    connect(ui.txtSearch, SIGNAL(textChanged(QString)), this, SLOT(txtSearch_textChanged(QString)));
    //ui.txtSearch->installEventFilter(this);
    ui.tvUserList->installEventFilter(this);
    ui.tvUserList->setFocus();
    windowLoaded = true;
}

wavrMainWindow::~wavrMainWindow() {
}

void wavrMainWindow::init(User *pLocalUser, QList<Group>* pGroupList, bool connected) {
    setWindowIcon(QIcon(IDR_APPICON));
    this->pLocalUser = pLocalUser;

    createMainMenu();
    //createToolBar();
    //createPresenceToolbar();
    //createAvatarMenu();

    //createTrayMenu();
   // createTrayIcon();
    //connectionStateChanged(connected);

    createUserMenu();
    ui.txtSearch->setVisible(false);
    ui.tvUserList->setIconSize(QSize(16, 16));
    ui.tvUserList->header()->setSectionsMovable(false);
    ui.tvUserList->header()->setStretchLastSection(false);
    ui.tvUserList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    int index = wavrHelper::statusIndexFromCode(pLocalUser->status);
    //	if status is not recognized, default to available
    index = qMax(index, 0);
    qDebug() << "status index " << index;
    ui.cmbPresence->setCurrentIndex(index);
    nAvatar = pLocalUser->avatar;

    pSettings = new wavrSettings();

    initGroups(pGroupList);
}

void wavrMainWindow::start(void) {
    connect(ui.cmbPresence, SIGNAL(currentIndexChanged(int)), this, SLOT(statusAction_triggered(int)));

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
        pItem->setIcon(0, QIcon(QPixmap(statusPic[index], "PNG")));

    wavrUserTreeWidgetGroupItem* pGroupItem = (wavrUserTreeWidgetGroupItem*)getGroupItem(&pUser->group);
    pGroupItem->addChild(pItem);
    pGroupItem->sortChildren(0, Qt::AscendingOrder);
    //wavrUserTreeWidgetGroupItem* pGroupItem = (wavrUserTreeWidgetGroupItem*)getGroupItem(&pUser->group);
    //pGroupItem->addChild(pItem);
    //pGroupItem->sortChildren(0, Qt::AscendingOrder);

    //  this should be called after item has been added to tree
    setUserAvatar(&pUser->id, &pUser->avatarPath);

    //    if(isHidden() || !isActveWindow()) {
    //        QString msg = tr("%1 is online");
    //        showTrayMessage(TM_Status, msg.arg(pItem->text(0)));
    //    }

    sendAvatar(&pUser->id);
}

void wavrMainWindow::updateUser(User *pUser) {
    if(!pUser)
        return;

    QTreeWidgetItem* pItem = getUserItem(&pUser->id);
    if(pItem) {
        updateStatusImage(pItem, &pUser->status);
        int index = wavrHelper::statusIndexFromCode(pUser->status);
        pItem->setData(0, StatusRole, index);
        pItem->setData(0, SubtextRole, pUser->note);
        pItem->setText(0, pUser->name);
        if(statusToolTip)
            pItem->setToolTip(0, wavrStrings::statusDesc()[index]);
        QTreeWidgetItem* pGroupItem = pItem->parent();
        pGroupItem->sortChildren(0, Qt::AscendingOrder);
    }
}

void wavrMainWindow::removeUser(QString *lpszUserId) {
    QTreeWidgetItem* pItem = getUserItem(lpszUserId);
    if(!pItem)
        return;

    QTreeWidgetItem* pGroup = pItem->parent();
    pGroup->removeChild(pItem);

//    if(isHidden() || !isActiveWindow()) {
//        QString msg = tr("%1 is offline.");
//        showTrayMessage(TM_Status, msg.arg(pItem->text(0)));
//        pSoundPlayer->play(SE_UserOffline);
//    }
}

void wavrMainWindow::receiveMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {
    QString filePath;

    switch(type) {
    case MT_Avatar:
        filePath = pMessage->data(XML_FILEPATH);
        setUserAvatar(lpszUserId, &filePath);
        break;
    default:
        break;
    }
}


void wavrMainWindow::settingsChanged() {
//	showSysTray = pSettings->value(IDS_SYSTRAY, IDS_SYSTRAY_VAL).toBool();
//	showSysTrayMsg = pSettings->value(IDS_SYSTRAYMSG, IDS_SYSTRAYMSG_VAL).toBool();
//	//	this setting should be loaded only at window init
//	if(init)
//		showMinimizeMsg = pSettings->value(IDS_MINIMIZEMSG, IDS_MINIMIZEMSG_VAL).toBool();
//	//	this operation should not be done when window inits
//	if(!init)
//		pTrayIcon->setVisible(showSysTray);
//	minimizeHide = pSettings->value(IDS_MINIMIZETRAY, IDS_MINIMIZETRAY_VAL).toBool();
//	singleClickActivation = pSettings->value(IDS_SINGLECLICKTRAY, IDS_SINGLECLICKTRAY_VAL).toBool();
//	allowSysTrayMinimize = pSettings->value(IDS_ALLOWSYSTRAYMIN, IDS_ALLOWSYSTRAYMIN_VAL).toBool();
//	showAlert = pSettings->value(IDS_ALERT, IDS_ALERT_VAL).toBool();
//	noBusyAlert = pSettings->value(IDS_NOBUSYALERT, IDS_NOBUSYALERT_VAL).toBool();
//	noDNDAlert = pSettings->value(IDS_NODNDALERT, IDS_NODNDALERT_VAL).toBool();
//	statusToolTip = pSettings->value(IDS_STATUSTOOLTIP, IDS_STATUSTOOLTIP_VAL).toBool();
    int viewType = pSettings->value(IDS_USERLISTVIEW, IDS_USERLISTVIEW_VAL).toInt();
    ui.tvUserList->setView((UserListView)viewType);
//    for(int index = 0; index < ui.tvUserList->topLevelItemCount(); index++) {
//        QTreeWidgetItem* item = ui.tvUserList->topLevelItem(index);
//        for(int childIndex = 0; childIndex < item->childCount(); childIndex++) {
//            QTreeWidgetItem* childItem = item->child(childIndex);
//            QSize itemSize = ui.tvUserList->view() == ULV_Detailed ? QSize(0, 36) : QSize(0, 20);
//            childItem->setSizeHint(0, itemSize);

//            QString toolTip = statusToolTip ? wavrStrings::statusDesc()[childItem->data(0, StatusRole).toInt()] : QString::null;
//            childItem->setToolTip(0, toolTip);
//        }
//    }
    //pSoundPlayer->settingsChanged();
    //ui.lblUserName->setText(pLocalUser->name);	// in case display name has been changed
}

QList<QTreeWidgetItem*> wavrMainWindow::getContactsList(void) {
    QList<QTreeWidgetItem*> contactsList;
    for(int index = 0; index < ui.tvUserList->topLevelItemCount(); index++)
        contactsList.append(ui.tvUserList->topLevelItem(index)->clone());

    return contactsList;
}

bool wavrMainWindow::eventFilter(QObject* pObject, QEvent* pEvent) {

    if(pEvent->type() == QEvent::KeyPress) {
        QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(pEvent);
        int key = pKeyEvent->key();
        if(key == Qt::Key_Escape) {
            ui.txtSearch->setVisible(false);
            ui.txtSearch->clear();
            return true;
        }
        if(!(pKeyEvent->modifiers() & Qt::ControlModifier)){
            ui.txtSearch->setVisible(true);
            ui.txtSearch->setFocus();
            ui.txtSearch->setText(pKeyEvent->text());
        }
    }
    return false;
}

void wavrMainWindow::closeEvent(QCloseEvent *pEvent) {
    // close main window to system tray
    pEvent->ignore();
    minimize();
}

void wavrMainWindow::changeEvent(QEvent *pEvent) {
    switch(pEvent->type()) {
    case QEvent::WindowStateChange:
        if(minimizeHide) {
            QWindowStateChangeEvent* e = (QWindowStateChangeEvent*)pEvent;
            if(isMinimized() && e->oldState() != Qt::WindowMinimized) {
                QTimer::singleShot(0, this, SLOT(hide()));
                pEvent->ignore();
                showMinimizeMessage();
            }
        }
        break;
    default:
        break;
    }

    QWidget::changeEvent(pEvent);
}

void wavrMainWindow::sendMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage) {
    emit messageSent(type, lpszUserId, pMessage);
}

void wavrMainWindow::traySettingsAction_triggered(void) {
    emit showSettings();
}

void wavrMainWindow::statusAction_triggered(int index) {
    if(index != -1) {
        QString status = statusCode[index];
        pLocalUser->status = status;
        pSettings->setValue(IDS_STATUS, status);
        if (!pLocalUser->note.isEmpty() || !pLocalUser->note.isNull()) {
            QString null = NULL;
            pSettings->setValue(IDS_NOTE, "", IDS_NOTE_VAL);
            sendMessage(MT_Note, NULL, &(null));
        }
        sendMessage(MT_Status, NULL, &status);
    }
}

void wavrMainWindow::refreshAction_triggered(void) {
    QString szUserId;
    QString szMessage;

    sendMessage(MT_Refresh, &szUserId, &szMessage);
}

void wavrMainWindow::tvUserList_itemActivated(QTreeWidgetItem* pItem, int column) {
    Q_UNUSED(column);
    if(pItem->data(0, TypeRole).toString().compare("User") == 0) {
        QString szUserId = pItem->data(0, IdRole).toString();
        emit chatStarting(&szUserId);
    }
}

void wavrMainWindow::tvUserList_itemContextMenu(QTreeWidgetItem *pItem, QPoint &pos) {
    if (!pItem)
        return;

    if(pItem->data(0, TypeRole).toString().compare("User") == 0) {
        for(int index = 0; index < pUserMenu->actions().count(); index++)
            pUserMenu->actions()[index]->setData(pItem->data(0, IdRole));

        bool fileCap = ((pItem->data(0, CapsRole).toUInt() & UC_File) == UC_File);
        pUserMenu->actions()[1]->setEnabled(fileCap);
        bool folderCap = ((pItem->data(0, CapsRole).toUInt() & UC_Folder) == UC_Folder);
        pUserMenu->actions()[2]->setEnabled(folderCap);
        pUserMenu->exec(pos);
    }
}

void wavrMainWindow::tvUserList_currentItemChanged(QTreeWidgetItem *pCurrent, QTreeWidgetItem *pPrevious) {
    Q_UNUSED(pPrevious);
    bool bEnabled = (pCurrent && pCurrent->data(0, TypeRole).toString().compare("User") == 0);
    //toolChatAction->setEnabled(bEnabled);
    //toolFileAction->setEnabled(bEnabled);
}

void wavrMainWindow::cmbPresence_returnPressed(void) {
    //	Shift the focus from txtNote to another control
    ui.tvUserList->setFocus();
}

void wavrMainWindow::cmbPresence_editingFinished(void) {
    QString note = ui.cmbPresence->currentText();
    int index = ui.cmbPresence->currentIndex();
    if(note.compare(wavrStrings::statusDesc()[index]) !=0) {
        pSettings->setValue(IDS_NOTE, note, IDS_NOTE_VAL);
        pLocalUser->note = note;
        sendMessage(MT_Note, NULL, &note);
    }
}

void wavrMainWindow::txtSearch_textChanged(QString text) {
    for(int topIndex = 0; topIndex < ui.tvUserList->topLevelItemCount(); topIndex++) {
        for(int index = 0; index < ui.tvUserList->topLevelItem(topIndex)->childCount(); index++) {
            QTreeWidgetItem* pItem = ui.tvUserList->topLevelItem(topIndex)->child(index);
            pItem->setHidden(true);
            if(pItem->text(0).startsWith(text)) {
                pItem->setHidden(false);
            }
        }
    }
}

void wavrMainWindow::userConversationAction_triggered(void) {
    QString userId = ui.tvUserList->currentItem()->data(0, IdRole).toString();
    emit chatStarting(&userId);
}

void wavrMainWindow::userBroadcastAction_triggered(void) {
    //emit showBroadcast();
}

void wavrMainWindow::userFileAction_triggered(void) {
    QString userId = ui.tvUserList->currentItem()->data(0, IdRole).toString();
    QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
    QString fileName = QFileDialog::getOpenFileName(this, QString(), dir);
    if(!fileName.isEmpty()) {
        pSettings->setValue(IDS_OPENPATH, QFileInfo(fileName).dir().absolutePath());
        sendMessage(MT_File, &userId, &fileName);
    }
}

void wavrMainWindow::userFolderAction_triggered(void) {
    QString userId = ui.tvUserList->currentItem()->data(0, IdRole).toString();
    QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
    QString path = QFileDialog::getExistingDirectory(this, QString(), dir, QFileDialog::ShowDirsOnly);
    if(!path.isEmpty()) {
        pSettings->setValue(IDS_OPENPATH, QFileInfo(path).absolutePath());
        sendMessage(MT_Folder, &userId, &path);
    }
}

void wavrMainWindow::userInfoAction_triggered(void) {
    QString userId = ui.tvUserList->currentItem()->data(0, IdRole).toString();
    QString message;
    sendMessage(MT_Query, &userId, &message);
}

void wavrMainWindow::createMainMenu(void) {
    pMainMenuBar = new QMenuBar(this);
    pMainMenu = pMainMenuBar->addMenu(tr("&Messenger"));

    refreshAction = pMainMenu->addAction(QIcon(QPixmap(IDR_REFRESH, "PNG")), "&Refresh contacts list",
        this, SLOT(refreshAction_triggered()), QKeySequence::Refresh);
    settingsAction = pMainMenu->addAction(QIcon(QPixmap(IDR_TOOLS, "PNG")), "&Preferences",
        this, SLOT(traySettingsAction_triggered()), QKeySequence::Preferences);

    layout()->setMenuBar(pMainMenuBar);
}

void wavrMainWindow::createUserMenu(void) {
    pUserMenu = new QMenu(this);

    userChatAction = pUserMenu->addAction("&Conversation", this, SLOT(userConversationAction_triggered()));
    userFileAction = pUserMenu->addAction("Send &File", this, SLOT(userFileAction_triggered()));
    userFolderAction = pUserMenu->addAction("Send a Fol&der", this, SLOT(userFolderAction_triggered()));
    pUserMenu->addSeparator();
    userBroadcastAction = pUserMenu->addAction("Send &Broadcast Message", this, SLOT(userBroadcastAction_triggered()));
    pUserMenu->addSeparator();
    userInfoAction = pUserMenu->addAction("Get &Information", this, SLOT(userInfoAction_triggered()));
}

//void wavrMainWindow::createPresenceToolbar(void) {
//pCmbPresence = new QComboBox(this);
//for(int index = 0; index < ST_COUNT; index++) {
//    pCmbPresence->addItem(QIcon(QPixmap(statusPic[index], "PNG")), wavrStrings::statusDesc()[index]);
//}
//QHBoxLayout *hbox = new QHBoxLayout;
//hbox->addWidget(pCmbPresence, 0);
//ui.presence_toolbar->setLayout(hbox);
//}

void wavrMainWindow::createSearchToolbar(void) {

}

void wavrMainWindow::showMinimizeMessage(void) {
if(showMinimizeMsg) {
    QString msg = tr("%1 will continue to run in the background. Activate this icon to restore the application window.");
    //showTrayMessage(TM_Minimize, msg.arg(wavrStrings::appName()));
    showMinimizeMsg = false;
}
}

void wavrMainWindow::initGroups(QList<Group>* pGroupList) {
    for(int index = 0; index < pGroupList->count(); index++) {
        wavrUserTreeWidgetGroupItem *pItem = new wavrUserTreeWidgetGroupItem();
        pItem->setData(0, IdRole, pGroupList->value(index).id);
        pItem->setData(0, TypeRole, "Group");
        pItem->setText(0, pGroupList->value(index).name);
        pItem->setSizeHint(0, QSize(0, 22));
        ui.tvUserList->addTopLevelItem(pItem);
    }

    ui.tvUserList->expandAll();
    // size will be either number of items in group expansion list or number of top level items in
    // treeview control, whichever is less. This is to  eliminate arary out of bounds error.
    int size = qMin(pSettings->beginReadArray(IDS_GROUPEXPHDR), ui.tvUserList->topLevelItemCount());
    for(int index = 0; index < size; index++) {
        pSettings->setArrayIndex(index);
        ui.tvUserList->topLevelItem(index)->setExpanded(pSettings->value(IDS_GROUP).toBool());
    }
    pSettings->endArray();
}

void wavrMainWindow::updateStatusImage(QTreeWidgetItem* pItem, QString* lpszStatus) {
    int index = wavrHelper::statusIndexFromCode(*lpszStatus);
    if(index != -1)
        pItem->setIcon(0, QIcon(QPixmap(statusPic[index], "PNG")));
}

void wavrMainWindow::sendMessage(MessageType type, QString* lpszUserId, QString* lpszMessage) {
    wavrXmlMessage xmlMessage;

    switch(type) {
    case MT_Status:
        xmlMessage.addData(XML_STATUS, *lpszMessage);
        break;
    case MT_Note:
        xmlMessage.addData(XML_NOTE, *lpszMessage);
        break;
    case MT_Refresh:
        break;
    case MT_Group:
        //xmlMessage.addData(XML_GROUP, *lpszMessage);
        break;
    case MT_File:
        qDebug() << "sending file inside main window";
    case MT_Folder:
        xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Normal]);
            xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Request]);
            xmlMessage.addData(XML_FILEPATH, *lpszMessage);
            break;
    case MT_Avatar:
        xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Avatar]);
        xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Request]);
        xmlMessage.addData(XML_FILEPATH, *lpszMessage);
        break;
    case MT_Query:
        xmlMessage.addData(XML_QUERYOP, QueryOpNames[QO_Get]);
        break;
    default:
        break;
    }

    sendMessage(type, lpszUserId, &xmlMessage);
}

void wavrMainWindow::sendAvatar(QString* lpszUserId) {
    QString filePath = StdLocation::avatarFile();
    if(!QFile::exists(filePath))
        return;

    sendMessage(MT_Avatar, lpszUserId, &filePath);
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

QTreeWidgetItem* wavrMainWindow::getGroupItem(QString* lpszGroupId) {
    for(int topIndex = 0; topIndex < ui.tvUserList->topLevelItemCount(); topIndex++) {
        QTreeWidgetItem* pItem = ui.tvUserList->topLevelItem(topIndex);
        if(pItem->data(0, IdRole).toString().compare(*lpszGroupId) == 0)
            return pItem;
    }

    return NULL;
}
