#ifndef MESSAGE_H
#define MESSAGE_H

#include "shared.h"
#include "xmlmessage.h"

class Message {
public:
    static QString addHeader(MessageType type, qint64 id, QString* lpszLocalId, QString* lpszPeerId, wavrXmlMessage* pMessage);
    static bool getHeader(QString* lpszMessage, MessageHeader** ppHeader, wavrXmlMessage** ppMessage);
};

#endif // MESSAGE_H
