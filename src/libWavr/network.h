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


#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork/QNetworkInterface>
#include <QTimer>
#include "udpnetwork.h"
#include "tcpnetwork.h"
#include "settings.h"
#include "xmlmessage.h"

class wavrNetwork : public QObject {
    Q_OBJECT

public:
    wavrNetwork(void);
    ~wavrNetwork(void);

    void init(wavrXmlMessage *pInitParams);
    void start(void);
    void stop(void);

    QString physicalAddress(void);
    void setLocalId(QString* lpszLocalId);
    void sendBroadcast(QString* lpszLocalId);
    void addConnection(QString* lpszUserId, QString* lpszAddress);
    void sendMessage(QString* lpszReceiverId, QString* lpszAddress, QString* lpszData);
    void settingsChanged(void);

    QString ipAddress;
    QString subnetMask;
    bool    isConnected;
    bool    canReceive;

signals:
    void connectionStateChanged(void);
    void broadcastReceived(DatagramHeader* pHeader, QString* lpszData);
    void newConnection(QString* lpszUserId, QString* lpszAddress);
    void connectionLost(QString* lpszUserId);
    void messageReceived(DatagramHeader* pHeader, QString* lpszData);

private slots:
    void timer_timeout(void);
    void udp_receiveBroadcast(DatagramHeader* pHeader, QString* lpszData);
    void tcp_newConnection(QString* lpszUserId, QString* lpszAddress);
    void tcp_connectionLost(QString* lpszUserId);
    void tcp_receiveMessage(DatagramHeader* pHeader, QString* lpszData);

private:
    bool getIPAddress(bool verbose = true);
    bool getIPAddress(QNetworkInterface* pNetworkInterface, QNetworkAddressEntry* pAddressEntry);
    bool getNetworkInterface(QNetworkInterface* pNetworkInterface);
    bool getNetworkInterface(QNetworkInterface* pNetworkInterface, QString* lpszPreferred);
    bool isInterfaceUp(QNetworkInterface* pNetworkInterface);
    bool getNetworkAddressEntry(QNetworkAddressEntry* pAddressEntry);

    wavrSettings*       pSettings;
    wavrUdpNetwork*     pUdpNetwork;
    wavrTcpNetwork*     pTcpNetwork;
    QTimer*             pTimer;
    QString             interfaceName;
    QNetworkInterface   networkInterface;
};

#endif // NETWORK_H
