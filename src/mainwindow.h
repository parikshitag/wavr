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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <QMenu>
#include <QComboBox>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <qevent.h>
#include <QTreeWidget>
#include <QWidgetAction>
#include "ui_mainwindow.h"
#include "libWavr/shared.h"
#include "libWavr/settings.h"
#include "libWavr/xmlmessage.h"
#include "libWavr/stdlocation.h"
#include "QProgressIndicator.h"

class wavrMainWindow : public QWidget
{
    Q_OBJECT
    
public:
    wavrMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~wavrMainWindow();
    
    void init(User* pLocalUser, QList<Group>* pGroupList,  bool connected);
    void start(void);
    void show(void);
    void restore(void);
    void minimize(void);
    void stop(void);
    void addUser(User* pUser);  //add new user discoverd to the list
    void updateUser(User* pUser); //update user value
    void removeUser(QString* lpszUserId);
    void receiveMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void connectionStateChanged(bool connected);
    void settingsChanged();
    //void showTrayMessage(TrayMessageType type, QString szMessage, QString szTitle = QString::null, TrayMessageIcon icon = TMI_info);
    QList<QTreeWidgetItem*> getContactsList(void);

signals:
    void appExiting(void);
    void messageSent(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void chatStarting(QString* lpszUserId);
    void showSettings(void);

protected:
//    bool eventFilter(QObject *pObject, QEvent *pEvent);
//    void closeEvent(QCloseEvent* pEvent);
//    void changeEvent(QEvent *pEvent);

private slots:
    void sendMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
//    void trayShowAction_triggered(void);
    void traySettingsAction_triggered(void);
//    void trayExistAction_triggered(void);
//    void statusAction_triggered(QAction* action);
//    void avatarAction_triggered(void);
//    void avatarBrowseAction_triggered(void);
//    void refreshAction_triggered(void);
//    void trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
//    void trayMessage_clicked(void);
//    void tvUserList_itemActivated(QTreeWidgetItem* pItem, int column);
//    void tvUserList_itemContextMenu(QTreeWidgetItem* pItem, QPoint& pos);


private:
    void createMainMenu(void);
    void createTrayMenu(void);
    void createTrayIcon(void);
    void createPresenceToolbar(void);
    void createSearchToolbar(void);
    void createUserMenu(void);
    void createToolBar(void);
    void setUIText(void);
    void showMinimizeMessage(void);
    void initGroups(QList<Group>* pGroupList);
    void updateStatusImage(QTreeWidgetItem* pItem, QString* lpszStatus);
    void setAvatar(QString fileName = QString());
    QTreeWidgetItem* getUserItem(QString* lpszUserId);
    QTreeWidgetItem* getGroupItem(QString* lpszGroupId);
    void sendMessage(MessageType type, QString* lpszUserId, QString* lpszMessage);
    void sendAvatar(QString* lpszUserId);
    void setUserAvatar(QString* lpszUserId, QString* lpszFilePath);
    void processTrayIconTrigger(void);
    void setTrayTooltip(void);

    Ui::MainWindow ui;
    wavrSettings* pSettings;
    QMenuBar* pMainMenuBar;
    QSystemTrayIcon* pTrayIcon;
    QMenu* pMainMenu;
    QComboBox* pCmbPresence;
    QLineEdit* pEditSrch;

    QMenu* pUserMenu;
    QToolButton* btnStatus;
    QAction* toolChatAction;
    User* pLocalUser;
    bool bConnected;
    int nAvatar;
    bool showSysTray;
    bool showSysTrayMsg;
    bool showMinimizeMsg;
    bool minimizeHide;
    bool allowSysTrayMinimize;
    bool showAlert;
    bool statusToolTip;
   // TrayMessageType lastTrayMessageType;
    QActionGroup* statusGroup;
    QAction* refreshAction;
    QAction* exitAction;
    QAction* settingsAction;

    QAction* userChatAction;
    QAction* avatarBrowseAction;
    bool windowLoaded;
};
#endif // MAINWINDOW_H
