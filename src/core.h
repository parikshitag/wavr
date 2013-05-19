#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

#include "libWavr/shared.h"
#include "libWavr/settings.h"
#include "libWavr/messaging.h"
#include "mainwindow.h"
#include "chatwindow.h"

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
    void startChatRoom(QString* lpszThreadId);
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
};

#endif // CORE_H
