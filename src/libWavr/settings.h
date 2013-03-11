#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QApplication>
#include <QDir>

//  Application settings definitions and default values
#define IDS_VERSION             "Application/Version"
#define IDS_VERSION_VAL         "0.1.0"

#define IDS_CONNECTION          "Connection/Connection"
#define IDS_CONNECTION_VAL      "Auto"

#define IDS_MULTICAST           "Connection/Multicast"
#define IDS_MULTICAST_VAL       "239.255.100.100"
#define IDS_UDPPORT             "Connection/UDPPort"
#define IDS_UDPPORT_VAL         50000
#define IDS_TCPPORT             "Connection/TCPPort"
#define IDS_TCPPORT_VAL         50000

#define IDS_BROADCASTHDR        "BroadcastHdr"
#define IDS_BROADCAST           "Broadcast"

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

};

#endif // SETTINGS_H
