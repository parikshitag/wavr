#ifndef SHARED_H
#define SHARED_H

#include <QString>
#include <QtNetwork/QHostInfo>

#include "definitions.h"

struct User {
    QString id;
    QString name;
    QString address;
    QString version;
    QString status;
    int avatar;
    QString note;
    QString avatarPath;
    uint caps;

    User(void) {}
    User(QString szId, QString szVersion, QString szAddress, QString szName, QString szStatus,
         int nAvatar, QString szNote, QString szAvatarPath, QString szCaps) {
        this->id = szId;
        this->version = szVersion;
        this->address = szAddress;
        this->name = szName;
        this->status = szStatus;
        this->avatar = nAvatar;
        this->avatarPath = szAvatarPath;
        this->caps = szCaps.toUInt();
    }

};

struct Group {
    QString id;
    QString name;

    Group(void) {}
    Group(QString szId) {
        this->id = szId;
    }
    Group(QString szId, QString szName) {
        this->id = szId;
        this->name = szName;
    }

    bool operator == (const Group& v) const { return (this->id.compare(v.id) == 0); }
};

struct DatagramHeader {
    DatagramType type;
    QString userId;
    QString address;

    DatagramHeader(DatagramType dtType, QString szUserId, QString szAddress) {
        type = dtType;
        userId = szUserId;
        address = szAddress;
    }
};

struct MessageHeader {
    MessageType type;
    qint64 id;
    QString userId;
    QString address;

    MessageHeader(MessageType mtType, qint64 nId, QString szUserId) {
        type = mtType;
        id = nId;
        userId = szUserId;
    }
};

/****************************************************************************
** Class: Helper
** Description: Collection of frequently used methods.
****************************************************************************/
class wavrHelper {
public:
    static int indexOf(const QString array[], int size, const QString& value);
    static int statusIndexFromCode(QString status);
    static QString getLogonName(void);
    static QString getHostName(void);
    static QString getOSName(void);
};

#endif // SHARED_H
