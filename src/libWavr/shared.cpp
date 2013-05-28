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
