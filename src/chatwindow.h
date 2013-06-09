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


#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QToolBar>
#include <QtWidgets/QToolButton>
#include <QMenu>
#include "ui_chatwindow.h"
#include "libWavr/shared.h"
#include "libWavr/settings.h"
#include "libWavr/xmlmessage.h"

class wavrChatWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit wavrChatWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~wavrChatWindow();
    
    void init(User* pLocalUser, User* pRemoteUser, bool connected);
    void stop(void);
    void receiveMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void connectionStateChanged(bool connected);
    void settingsChanged(void);

    QString localId;
    QHash<QString, QString> peerIds;
    QString threadId;
    bool groupMode;

signals:
    void messageSent(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void closed(QString* lpszUserId);

private slots:
//    void btnFont_clicked(void);
//    void btnFontColor_clicked(void);
//    void smileyAction_triggered(void);
//    void checkChatState(void);

private:
    void createSmileyMenu(void);
    void createToolBar(void);
    void setUIText(void);
    void sendMessage(void);
    void encodeMessage(QString* lpszMessage);
    void showStatus(int flag, bool add);
    QString getWindowTitle(void);
    void setMessageFont(QFont& font);
    //void setChatState(ChatState newChatState);

    QString peerId;
    QString localName;
    QHash<QString, QString> peerNames;
    QHash<QString, QString> peerStatuses;
    QHash<QString, uint> peerCaps;
    User* pLocalUser;
    QString lastUserId;

    Ui::ChatWindow *ui;
    wavrSettings* pSettings;
    QAction* pFontAction;
    QAction* pFontColorAction;
    QMenu* pSmileyMenu;
    int nSmiley;
    bool bConnected;
    int infoFlag;
    bool showSmiley;

};

#endif // CHATWINDOW_H
