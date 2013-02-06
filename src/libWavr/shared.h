#ifndef SHARED_H
#define SHARED_H

#include <QString>
#include "definitions.h"

//struct User {
//    QString id;
//    QString name;
//    QString address;
//    QString version;
//    QString status;
//    int avatar;
//    QString avatarPath;

//    User(void) {}
//    User(QString szId, QString szVersion, QString szAddress, QString szName, QString szStatus,
//         int nAvatar, QString szAvatarPath) {
//        this->id = szId;
//        this->version = szVersion;
//        this->address = szAddress;
//        this->name = szName;
//        this->status = szStatus;
//        this->avatar = nAvatar;
//        this->avatarPath = szAvatarPath;
//    }

//};

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

/****************************************************************************
** Class: Helper
** Description: Collection of frequently used methods.
****************************************************************************/
class Helper {
public:
    static int indexOf(const QString array[], int size, const QString& value);
};

#endif // SHARED_H
