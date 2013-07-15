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


#include "shared.h"

/**
 * @param array Array to be searched from.
 * @param size  Size uptil value is searched in array.
 * @param value Value to be searched in array.
 * @return
 */
int wavrHelper::indexOf(const QString array[], int size, const QString &value) {
    for(int index = 0; index < size; index++) {
        if (value == array[index])
            return index;
    }
    return -1;
}

int wavrHelper::statusIndexFromCode(QString status) {
    for(int index = 0; index < ST_COUNT; index++)
        if(statusCode[index].compare(status) == 0)
            return index;
    return -1;
}

QString wavrHelper::formatSize(qint64 size) {
    qint64 gb = 1073741824;
    qint64 mb = 1048576;
    qint64 kb = 1024;

    if(size > gb)
        return QString("%1 GB").arg((double)size / gb, 0, 'f', 2);
    else if(size > mb)
        return QString("%1 MB").arg((double)size / mb, 0, 'f', 2);
    else if(size > kb)
        return QString("%1 KB").arg((double)size / kb, 0, 'f', 2);
    else
        return QString("%1 bytes").arg(size);
}

QString wavrHelper::getUuid(void) {
    QString Uuid = QUuid::createUuid().toString();
    Uuid = Uuid.remove("{").remove("}").remove("-");
    return Uuid;
}

QString wavrHelper::getLogonName(void) {
#if defined Q_WS_WIN	//	if platform is Windows
    TCHAR szUserName[UNLEN + 1];
    DWORD nSize = sizeof(szUserName);
    GetUserName(szUserName, &nSize);
    return QString::fromStdWString(szUserName);
#else	// this code should work for MAC and Linux
    char* szUserName;
    szUserName = getenv("USER");
    if(szUserName)
        return QString::fromLatin1(szUserName);
#endif

    return QString::null;
}

QString wavrHelper::getHostName(void) {
    return QHostInfo::localHostName();
}

QString wavrHelper::getOSName(void) {
    QString osName = "Unknown";

#if defined Q_WS_WIN
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_NT:
        osName = "Windows NT";
        break;
    case QSysInfo::WV_2000:
        osName = "Windows 2000";
        break;
    case QSysInfo::WV_XP:
        osName = "Windows XP";
        break;
    case QSysInfo::WV_2003:
        osName = "Windows Server 2003";
        break;
    case QSysInfo::WV_VISTA:
        osName = "Windows Vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        osName = "Windows 7";
        break;
    default:
        osName = "Windows";
        break;
    }
#elif defined Q_WS_MAC
    switch(QSysInfo::MacintoshVersion) {
    case QSysInfo::MV_CHEETAH:
        osName = "Mac OS X 10.0";
        break;
    case QSysInfo::MV_PUMA:
        osName = "Mac OS X 10.1";
        break;
    case QSysInfo::MV_JAGUAR:
        osName = "Mac OS X 10.2";
        break;
    case QSysInfo::MV_PANTHER:
        osName = "Mac OS X 10.3";
        break;
    case QSysInfo::MV_TIGER:
        osName = "Mac OS X 10.4";
        break;
    case QSysInfo::MV_LEOPARD:
        osName = "Mac OS X 10.5";
        break;
    case QSysInfo::MV_SNOWLEOPARD:
        osName = "Mac OS X 10.6";
        break;
    case QSysInfo::MV_LION:
        osName = "Mac OS X 10.7";
        break;
    default:
        osName = "Mac OS X";
        break;
    }
#elif defined Q_WS_X11
    osName = "Linux/X11";
#endif

    return osName;
}

bool wavrHelper::stringToBool(const QString& value) {
    return value.compare(WAVR_TRUE) == 0 ? true : false;
}
