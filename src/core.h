#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

#include "shared.h"
#include "messaging.h"
#include "mainwindow.h"

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

};

#endif // CORE_H
