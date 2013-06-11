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


#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

#include "libWavr/shared.h"
#include "libWavr/settings.h"
#include "libWavr/messaging.h"
#include "mainwindow.h"
#include "chatwindow.h"
#include "strings.h"
#include "uidefinitions.h"

class wavrCore : public QObject {
    Q_OBJECT

public:
    wavrCore(void);
    ~wavrCore(void);
    void init(void);
    bool start(void);

public slots:
    bool receiveAppMessage(const QString& szMessage);

private slots:
    void exitApp(void);
    void aboutToExit(void);
    void timer_timeout(void);
    void startChat(QString* lpszUserId);
    //void startChatRoom(QString* lpszThreadId);
    void sendMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void receiveMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void connectionStateChanged(void);
    void chatWindow_closed(QString* lpszUserId);

private:
    void stop(void);
    void loadSettings(void);
    void settingsChanged(void);
    void processMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void routeMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void createChatWindow(QString* lpszUserId);
    void showChatWindow(wavrChatWindow* chatWindow, bool show, bool alert = false);
    void showPortConflictMessage(void);

    wavrSettings*           pSettings;
    QTimer*                 pTimer;
    wavrMessaging*          pMessaging;
    wavrMainWindow*         pMainWindow;
    QTabWidget*             pTabWidget;
    QList<wavrChatWindow*>  chatWindows;


    bool                    messagePop;
    bool                    pubMessagePop;
    QString                 lang;
    bool                    adaptiveRefresh;
    int                     refreshTime;
    wavrXmlMessage*         pInitParams;
};

#endif // CORE_H
