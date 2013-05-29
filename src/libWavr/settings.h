#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QApplication>
#include <QDir>

//  Application settings definitions and default values
#define IDS_VERSION             "Application/Version"
#define IDS_VERSION_VAL         "0.1.0"

#define IDS_AUTOSTART           "AutoStart"
#define IDS_AUTOSTART_VAL       true

#define IDS_LANGUAGE            "Locale/Language"
#define IDS_LANGUAGE_VAL        "en_US"

#define IDS_REFRESHTIME         "RefreshInterval"
#define IDS_REFRESHTIME_VAL     300

#define IDS_MESSAGEPOP         "Messages/MessagePop"        // set incoming messages foreground or minimize to taskbar
#define IDS_MESSAGEPOP_VAL      false
#define IDS_PUBMESSAGEPOP       "Messages/PubMessagePop"    // set above to public messages or not
#define IDS_PUBMESSAGEPOP_VAL   false

#define IDS_CONNECTION          "Connection/Connection"
#define IDS_CONNECTION_VAL      "Auto"
#define IDS_TIMEOUT             "Connection/Timeout"
#define IDS_TIMEOUT_VAL         10
#define IDS_MAXRETRIES			"Connection/MaxRetries"
#define IDS_MAXRETRIES_VAL		1

#define IDS_MULTICAST           "Connection/Multicast"
#define IDS_MULTICAST_VAL       "239.255.100.100"
#define IDS_UDPPORT             "Connection/UDPPort"
#define IDS_UDPPORT_VAL         50000
#define IDS_TCPPORT             "Connection/TCPPort"
#define IDS_TCPPORT_VAL         50000

#define IDS_BROADCASTHDR        "BroadcastHdr"
#define IDS_BROADCAST           "Broadcast"

#define IDS_STATUS              "User/Status"
#define IDS_STATUS_VAL          statusCode[0]
#define IDS_AVATAR              "User/Avatar"
#define IDS_AVATAR_VAL			65535	//	this should be a number bigger than AVT_COUNT, 65535 set arbitrarily

#define IDS_USERNAME            "User/Avatar"
#define IDS_USERNAME_VAL        ""
#define IDS_USERFIRSTNAME		"User/FirstName"
#define IDS_USERFIRSTNAME_VAL	""
#define IDS_USERLASTNAME		"User/LastName"
#define IDS_USERLASTNAME_VAL	""
#define IDS_USERABOUT			"User/About"
#define IDS_USERABOUT_VAL		""
#define IDS_NOTE                "User/Note"
#define IDS_NOTE_VAL            ""
class wavrSettingsBase : public QSettings {
public:
    wavrSettingsBase(void);
    wavrSettingsBase(const QString& fileName, Format format);
    wavrSettingsBase(Format format, Scope scope, const QString& organization, const QString& application);
    ~wavrSettingsBase(void);

};

class wavrSettings : public wavrSettingsBase {
public:
    wavrSettings(void) : wavrSettingsBase(QSettings::IniFormat, QSettings::UserScope, IDA_COMPANY, IDA_PRODUCT) {}
    ~wavrSettings(void) {}

    static void setAutoStart(bool on);
};

#endif // SETTINGS_H
