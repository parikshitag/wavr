#ifndef CHATHELPER_H
#define CHATHELPER_H

#include "uidefinitions.h"
#include "chatdefinitions.h"
#include "libWavr/xmlmessage.h"

struct SingleMessage {
    MessageType type;
    QString userId;
    QString userName;
    wavrXmlMessage message;
    QString id; //secondary id for more efficient traversal

    SingleMessage() {}
    SingleMessage(MessageType mType, QString szUserId, QString szUserName, wavrXmlMessage xmlMessage, QString szId = QString()) {
        type = mType;
        userId = szUserId;
        userName = szUserName;
        message = xmlMessage;
        id = szId;
    }
};

QDataStream &operator << (QDataStream &out, const SingleMessage &message);
QDataStream &operator >> (QDataStream &in, SingleMessage &message);

class ChatHelper {
public:
    static void makeHtmlSafe(QString* lpszMessage);
    static QString replaceSmiley(QString* lpszHtml);
    static void encodeSmileys(QString* lpszMessage);
    static void decodeSmileys(QString* lpszMessage);
};
#endif // CHATHELPER_H
