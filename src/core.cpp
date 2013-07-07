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


#include <QMessageBox>
#include "libWavr/trace.h"
#include "core.h"

wavrCore::wavrCore(void) {
    pMessaging = new wavrMessaging();
    connect(pMessaging, SIGNAL(messageReceived(MessageType,QString*,wavrXmlMessage*)),
            this, SLOT(receiveMessage(MessageType,QString*,wavrXmlMessage*)));
    connect(pMessaging, SIGNAL(connectionStateChanged()), this, SLOT(connectionStateChanged()));
    pMainWindow = new wavrMainWindow();
    connect(pMainWindow, SIGNAL(appExiting()), this, SLOT(exitApp()));
    connect(pMainWindow, SIGNAL(chatStarting(QString*)), this, SLOT(startChat(QString*)));

    connect(pMainWindow, SIGNAL(showSettings()), this, SLOT(showSettings()));

    connect(pMainWindow, SIGNAL(messageSent(MessageType, QString*, wavrXmlMessage*)),
            this, SLOT(sendMessage(MessageType,QString*,wavrXmlMessage*)));

    pTabWidget = new QTabWidget;

    pSettingsDialog = NULL;

}

wavrCore::~wavrCore(void){
}

void wavrCore::init(){
    // prevent auto app exit when last visible window is closed
    qApp->setQuitOnLastWindowClosed(false);

    pInitParams = new wavrXmlMessage();

    pInitParams->addData(XML_TRACEMODE, WAVR_TRUE);
    pInitParams->addData(XML_LOGFILE, StdLocation::freeLogFile());

    wavrTrace::init(pInitParams);
    wavrTrace::write("Application initialized");

    loadSettings();
    wavrTrace::write("Settings loaded");

    pMessaging->init(pInitParams);
    pMessaging->setLoopback(true);
    pMainWindow->init(pMessaging->localUser, &pMessaging->groupList, pMessaging->isConnected());
}

bool wavrCore::start(void) {
    wavrTrace::write("Application started");
    pMessaging->start();

    if(pMessaging->isConnected() && !pMessaging->canReceive()) {
        showPortConflictMessage();
        // stop the application
        stop();
        return false;
    }

    pMainWindow->start();

    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout(void)), this, SLOT(timer_timeout()));
    // Set the timer to trigger 10 seconds after the application starts. After the
    // first trigger, the timeout period will be decided by user settings.
    adaptiveRefresh = false;
    pTimer-> start(10000);
    // Set the application to autostart when computer starts.
    bool autoStart = pSettings->value(IDS_AUTOSTART, IDS_AUTOSTART_VAL).toBool();
    wavrSettings::setAutoStart(autoStart);

    return true;
}

// This is the initial point where settings are used in the application
void wavrCore::loadSettings(void) {
    pSettings = new wavrSettings();

    lang = pSettings->value(IDS_LANGUAGE, IDS_LANGUAGE_VAL).toString();
    //Application::setLanguage(lang);
   // Application::setLayoutDirection(tr("LAYOUT_DIRECTION") == RTL_LAYOUT ? Qt::RightToLeft : Qt::LeftToRight);
    messageTop = pSettings->value(IDS_MESSAGETOP, IDS_MESSAGETOP_VAL).toBool();
    pubMessagePop = pSettings->value(IDS_PUBMESSAGEPOP, IDS_PUBMESSAGEPOP_VAL).toBool();
    refreshTime = pSettings->value(IDS_REFRESHTIME, IDS_REFRESHTIME_VAL).toInt() * 1000;


}

void wavrCore::settingsChanged() {
    pMessaging->settingsChanged();
    pMainWindow->settingsChanged();

    messageTop = pSettings->value(IDS_MESSAGETOP, IDS_MESSAGETOP_VAL).toBool();
    pubMessagePop = pSettings->value(IDS_PUBMESSAGEPOP, IDS_PUBMESSAGEPOP_VAL).toInt() * 1000;
    pTimer->setInterval(refreshTime);
    bool autoStart = pSettings->value(IDS_AUTOSTART, IDS_AUTOSTART_VAL).toBool();
    wavrSettings::setAutoStart(autoStart);
    QString appLang = pSettings->value(IDS_LANGUAGE, IDS_LANGUAGE_VAL).toString();
    if(appLang.compare(lang) != 0) {
        lang = appLang;
        //Application::setLanguage(lang);
        //Application::setLayoutDirection(tr("LAYOUT_DIRECTION") == RTL_LAYOUT ? Qt::RightToLeft : Qt::LeftToRight);
        //wavrStrings::retranslate();
    }

}

void wavrCore::stop(void) {
    pTabWidget->clear();
    for(int index = 0; index < chatWindows.count(); index++) {
        //chatWindows[index]->stop();
        chatWindows[index]->deleteLater();
    }

//    for(int index = 0; index < chatRoomWindows.count(); index++) {
//        chatRoomWindows[index]->stop();
//        chatRoomWindows[index]->deleteLater();
//    }
    if(pTimer)
        pTimer->stop();

    pMessaging->stop();
    //pMainWindow->stop();

    wavrTrace::write("Application stopped");
}


/**
 * @brief Quit Application manually.
 *  This slot handles the exit signal emitted by main window when the user
 *  selects quit from the menu.
 */
void wavrCore::exitApp(void) {
    qApp->quit();
}

/**
 * @brief Quit Application manually or from interrupt from OS.
 *  This slot handles the signal emitted by QApplication when the application
 *  quits either by user interaction or from operating system signal.
 */
void wavrCore::aboutToExit(void) {
    stop();
    pSettings->setValue(IDS_VERSION, IDA_VERSION);

    pSettings->sync();
}

void wavrCore::timer_timeout(void) {
    //Refresh the contacts list wheneve the timer triggers
    pMessaging->update();
    if(adaptiveRefresh) {
        //  The refresh interval is doubled until the refresh time defined by used is reached.
        //  Then keep refreshing at that interval.
        int nextInterval = pTimer->interval() * 2;
        int maxInterval = pSettings->value(IDS_REFRESHTIME, IDS_REFRESHTIME_VAL).toInt() * 1000;
        int interval = qMin(nextInterval, maxInterval);
        adaptiveRefresh = (nextInterval >= maxInterval) ? false : true;
        pTimer->setInterval(interval);
    } else if (refreshTime > pTimer->interval()) {
        pTimer->setInterval(refreshTime);
    }
}

void wavrCore::startChat(QString* lpszUserId) {
    //return if a chat window is already open for this user
    for(int index = 0; index < chatWindows.count(); index++) {
        if(!chatWindows[index]->groupMode && chatWindows[index]->peerIds.contains(*lpszUserId)) {
            showChatWindow(chatWindows[index], true);
            return;
        }
    }
    pTabWidget->show();
    createChatWindow(lpszUserId);
    showChatWindow(chatWindows.last(), true);
}


void wavrCore::sendMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {

    switch(type) {
    case MT_Broadcast:
    case MT_UserName:
    case MT_Message:
    case MT_ChatState:
    case MT_Version:
    case MT_Avatar:
        pMessaging->sendMessage(type, lpszUserId, pMessage);
        break;
    case MT_Status:
    case MT_Note:
        pMessaging->sendMessage(type, lpszUserId, pMessage);
        break;
    case MT_Refresh:
        pMessaging->update();
        break;
    default:
        break;
    }
}

void wavrCore::receiveMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {
    processMessage(type, lpszUserId, pMessage);
}

bool wavrCore::receiveAppMessage(const QString &szMessage) {
    bool doNotExit = true;

    if(szMessage.isEmpty()) {
        //pMainWindow->restore();
        return doNotExit;
    }
}

void wavrCore::connectionStateChanged(void) {
    bool connected = pMessaging->isConnected();

    //pMainWindow->connectionStateChanged(connected);
    for(int index = 0; index < chatWindows.count(); index++)
        //chatWindows[index]->connectionStateChanged(connected);

    if (pMessaging->isConnected() && !pMessaging->canReceive()) {
        showPortConflictMessage();
        exitApp();
    }
}

void wavrCore::showSettings(void) {
    if(!pSettingsDialog) {
        pSettingsDialog = new wavrSettingsDialog(pMainWindow);
        pSettingsDialog->init();
    }

    if(pSettingsDialog->exec())
        settingsChanged();
}

void wavrCore::chatWindow_closed(QString *lpszUserId) {
    for(int index = 0; index < chatWindows.count(); index++) {
        if(chatWindows[index]->peerIds.contains(*lpszUserId)) {
            pTabWidget->removeTab(pTabWidget->indexOf(chatWindows[index]));
            chatWindows.removeAt(index);
        }
    }
}

/**
 * @brief wavrCore::processMessage
 * @param type
 * @param lpszUserId
 * @param pMessage
 *  Checks the incoming message type, if
 *  MT_Announce: add user to the list of contacts in main window.
 *  MT_Depart: remove user from the list of contacts in main window and notify departure to that windows (chat and group)
 *  For all message types: notifies the corresponding message to all or specific active windows.
 */
void wavrCore::processMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {
    switch(type) {
    case MT_Announce:
        pMainWindow->addUser(pMessaging->getUser(lpszUserId));
        break;
    case MT_Depart:
        pMainWindow->removeUser(lpszUserId);
        routeMessage(type, lpszUserId, pMessage);
        break;
    case MT_Status:
    case MT_UserName:
    case MT_Note:
        pMainWindow->updateUser(pMessaging->getUser(lpszUserId));
        routeMessage(type, lpszUserId, pMessage);
        break;
    case MT_Avatar:
        pMainWindow->receiveMessage(type, lpszUserId, pMessage);
        routeMessage(type, lpszUserId, pMessage);
        break;
    case MT_Message:
        routeMessage(type, lpszUserId, pMessage);
        break;
    case MT_Failed:
        routeMessage(type, lpszUserId, pMessage);
        break;
    case MT_Error:
        break;
    case MT_ChatState:
        routeMessage(type, lpszUserId, pMessage);
        break;
    default:
        break;
    }
}

void wavrCore::routeMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {
    bool windowExists = false;
    bool needsNotice = (type == MT_Message || type == MT_Broadcast || type == MT_Failed);

    // if no specific user is specified, send this message to all windows
    if(!lpszUserId) {
        for (int index = 0; index < chatWindows.count(); index++) {
            //chatWindows[index]->receiveMessage(type, lpszUserId, pMessage);
        }
    } else {
        switch(type) {
        case MT_Avatar:
        case MT_Status:
        case MT_UserName:
            for (int index = 0; index < chatWindows.count(); index++)
                if(chatWindows[index]->peerIds.contains(*lpszUserId)
                        || chatWindows[index]->localId.compare(*lpszUserId) == 0)
                    //chatWindows[index]->receiveMessage(type, lpszUserId, pMessage);
            break;
        default:
            for(int index = 0; index < chatWindows.count(); index++) {
                if(chatWindows[index]->peerIds.contains(*lpszUserId)) {
                    //chatWindows[index]->receiveMessage(type, lpszUserId, pMessage);
                    if(needsNotice)
                        showChatWindow(chatWindows[index], messageTop, needsNotice);
                    windowExists = true;
                    break;
                }
            }
            break;
        }
    }

    // create a new window if no chat window with this user exists and the
    // incoming message is of type that needs notice
    if(!windowExists && needsNotice) {
        createChatWindow(lpszUserId);
        //chatWindows.last()->receiveMessage(type, lpszUserId, pMessage);
        if(needsNotice)
            showChatWindow(chatWindows.last(), messageTop, needsNotice);
    }
}

void wavrCore::createChatWindow(QString* lpszUserId) {
    // create new chat window for this user
    wavrChatWindow* pChatWindow = new wavrChatWindow();
    chatWindows.append(pChatWindow);
    pTabWidget->addTab(pChatWindow, *lpszUserId);

    User* pLocalUser = pMessaging->localUser;
    User* pRemoteUser = pMessaging->getUser(lpszUserId);
    connect(pChatWindow, SIGNAL(messageSent(MessageType,QString*,wavrXmlMessage*)),
            this, SLOT(sendMessage(MessageType,QString*,wavrXmlMessage*)));

    connect(pChatWindow, SIGNAL(closed(QString*)), this, SLOT(chatWindow_closed(QString*)));
    //pChatWindow->init(pLocalUser, pRemoteUser, pMessaging->isConnected());
}

void wavrCore::showChatWindow(wavrChatWindow* chatWindow, bool show, bool alert){
    if(show) {
        chatWindow->show();
        chatWindow->activateWindow();
        pTabWidget->setCurrentWidget(chatWindow);
    } else {
        chatWindow->show();
        if(alert)
            qApp->alert(chatWindow);
    }
}

void wavrCore::showPortConflictMessage(void) {
    // show message box
    QMessageBox msgBox;
    msgBox.setWindowTitle(wavrStrings::appName());
    msgBox.setWindowIcon(QIcon(IDR_APPICON));
    msgBox.setIcon(QMessageBox::Critical);
    QString msg = tr("A port address conflict has been detected. %1 will close now.");
    msgBox.setText(msg.arg(wavrStrings::appName()));
    QString detail = tr("%1 cannot start because another application is using the port "\
                        "configured for use with %2");
    msgBox.setDetailedText(detail.arg(wavrStrings::appName(), wavrStrings::appName()));
    msgBox.exec();
}
