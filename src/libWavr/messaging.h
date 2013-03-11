#ifndef MESSAGING_H
#define MESSAGING_H

#include <QObject>
#include <QTimer>
#include "shared.h"
#include "message.h"
#include "network.h"
#include "settings.h"
#include "xmlmessage.h"

struct PendingMsg {
    qint64 msgId;
    bool active;
    QDateTime timeStamp;
    MessageType type;
    QString userId;
    XmlMessage xmlMessage;
    int retry;

    PendingMsg(void) {}
    PendingMsg(qint64 nMsgId, bool bActiv)
};

class messaging : public QObject
{
    Q_OBJECT
public:
    explicit messaging(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // MESSAGING_H
