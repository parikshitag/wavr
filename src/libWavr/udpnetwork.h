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


#ifndef UDPNETWORK_H
#define UDPNETWORK_H

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QList>
#include "shared.h"
#include "settings.h"


class wavrUdpNetwork : public QObject {
    Q_OBJECT

public:
    wavrUdpNetwork(void);
    ~wavrUdpNetwork(void);

    void init(int nPort = 0);
    void start(void);
    void stop(void);
    void setLocalId(QString* lpszLocalId);
    void sendBroadcast(QString* lpszData);
    void settingsChanged(void);
    void setMulticastInterface(const QNetworkInterface& networkInterface);
    void setIPAddress(const QString& szAddress, const QString& szSubnet);

    bool isConnected;
    bool canReceive;

signals:
    void broadcastReceived(DatagramHeader* pHeader, QString* lpszData);

private slots:
    void processPendingDatagrams(void);

private:
    void sendDatagram(QHostAddress remoteAddress, QByteArray& baDatagram);
    bool startReceiving(void);
    void parseDatagram(QString* lpszAddress, QByteArray& baDatagram); ///process incoming broadcasting message
    void setDefaultBroadcast(void);

    wavrSettings*   pSettings;
    QUdpSocket*     pUdpReceiver;
    QUdpSocket*     pUdpSender;

    bool                isRunning;
    int                 nUdpPort;
    QHostAddress        multicastAddress;
    QString             localId;
    QNetworkInterface   multicastInterface;
    QHostAddress        ipAddress;
    QHostAddress        subnetMask;
    QList<QHostAddress> broadcastList;
    QHostAddress        defBroadcast;   // obtained by ORing the host address and bit inversed subnet mask
};

#endif // UDPNETWORK_H
