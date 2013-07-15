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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "shared.h"

//  Application settings definitions and default values
#define IDS_VERSION             "Application/Version"
#define IDS_VERSION_VAL         "0.1.0"
#define IDS_OPENPATH			"Application/OpenPath"
#define IDS_OPENPATH_VAL		QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)

#define IDS_WINDOWMAIN          "Window/Main"

#define IDS_AUTOSTART           "AutoStart"
#define IDS_AUTOSTART_VAL       true
#define IDS_AUTOSHOW            "AutoShow"
#define IDS_AUTOSHOW_VAL        true
#define IDS_LANGUAGE            "Locale/Language"
#define IDS_LANGUAGE_VAL        "en_US"

#define IDS_MINIMIZEMSG         "SystemTray/MinimizeMsg"
#define IDS_MINIMIZEMSG_VAL     true

#define IDS_REFRESHTIME         "RefreshInterval"
#define IDS_REFRESHTIME_VAL     300

#define IDS_EMOTICON			"Messages/Emoticon"
#define IDS_EMOTICON_VAL		true
#define IDS_MESSAGETIME			"Messages/MessageTime"
#define IDS_MESSAGETIME_VAL		true
#define IDS_MESSAGEDATE			"Messages/MessageDate"
#define IDS_MESSAGEDATE_VAL		false
#define IDS_ALLOWLINKS			"Messages/AllowLinks"
#define IDS_ALLOWLINKS_VAL		true
#define IDS_PATHTOLINK			"Messages/PathToLink"
#define IDS_PATHTOLINK_VAL		false
#define IDS_TRIMMESSAGE			"Messages/Trim"
#define IDS_TRIMMESSAGE_VAL		true
#define IDS_CLEARONCLOSE        "Messages/ClearOnClose"
#define IDS_CLEARONCLOSE_VAL    true
#define IDS_MESSAGETOP         "Messages/MessagePop"        // set incoming messages foreground or minimize to taskbar
#define IDS_MESSAGETOP_VAL      false
#define IDS_PUBMESSAGEPOP       "Messages/PubMessagePop"    // set above to public messages or not
#define IDS_PUBMESSAGEPOP_VAL   false
#define IDS_HISTORY				"History/History"
#define IDS_HISTORY_VAL			true

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
#define IDS_AUTOFILE			"FileTransfer/AutoFile"
#define IDS_AUTOFILE_VAL		false

#define IDS_THEME				"Appearance/Theme"
#define IDS_THEME_VAL			":/themes/Classic"
#define IDS_USERLISTVIEW		"Appearance/UserListView"
#define IDS_USERLISTVIEW_VAL	ULV_Detailed
#define IDS_SENDKEYMOD			"Hotkeys/SendKeyMod"
#define IDS_SENDKEYMOD_VAL		false
#define IDS_STATUS              "User/Status"
#define IDS_STATUS_VAL          statusCode[0]
#define IDS_AVATAR              "User/Avatar"
#define IDS_AVATAR_VAL			65535	//	this should be a number bigger than AVT_COUNT, 65535 set arbitrarily
#define IDS_USERNAME            "User/Name"
#define IDS_USERNAME_VAL        ""
#define IDS_USERFIRSTNAME		"User/FirstName"
#define IDS_USERFIRSTNAME_VAL	""
#define IDS_USERLASTNAME		"User/LastName"
#define IDS_USERLASTNAME_VAL	""
#define IDS_USERABOUT			"User/About"
#define IDS_USERABOUT_VAL		""
#define IDS_NOTE                "User/Note"
#define IDS_NOTE_VAL            ""

#define IDS_GROUPHDR			"Groups"
#define IDS_GROUP               "Group"
#define IDS_GROUPNAME			"GroupName"
#define IDS_GROUPEXPHDR         "GroupExp"
#define IDS_GROUPMAPHDR			"GroupMap"
#define IDS_USER				"User"
#define IDS_BROADCASTHDR        "BroadcastHdr"
#define IDS_BROADCAST           "Broadcast"
class wavrSettingsBase : public QSettings {
public:
    wavrSettingsBase(void);
    wavrSettingsBase(const QString& fileName, Format format);
    wavrSettingsBase(Format format, Scope scope, const QString& organization, const QString& application);
    ~wavrSettingsBase(void);

    using QSettings::setValue;
    void setValue(const QString& key, const QVariant& value, const QVariant& defaultValue);

};

class wavrSettings : public wavrSettingsBase {
public:
    wavrSettings(void) : wavrSettingsBase(QSettings::IniFormat, QSettings::UserScope, IDA_COMPANY, IDA_PRODUCT) {}
    ~wavrSettings(void) {}

    static void setAutoStart(bool on);
};

#endif // SETTINGS_H
