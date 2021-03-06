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

#include "trace.h"
#include "udpnetwork.h"

/**
 *  \class wavrUdpNetwork
 *  \ingroup    libWavr
 *  \brief  The UdpNetwork class deals with all kinds of UDP network related tasks.
 *
 *  This class broadcast messages, sends and receives Datagrams.
 */

/**
 * @brief Initializes all variables.
 */
wavrUdpNetwork::wavrUdpNetwork(void) {
    pUdpReceiver = new QUdpSocket(this);
    pUdpSender = new QUdpSocket(this);
    localId = QString::null;
    isRunning = false;
    ipAddress = QHostAddress::Any;
    subnetMask = QHostAddress::Any;
    defBroadcast = QHostAddress::Broadcast;
    broadcastList.clear();
}

wavrUdpNetwork::~wavrUdpNetwork(void) {
}

/**
 * Initializes the udpnetwork class and creates a wavrSettings object.
 * Initializes udp port, multicast Address, and broadcastList variable with the broadcast list given by the user in the settings.
 */
void wavrUdpNetwork::init(int nPort) {
    pSettings = new wavrSettings();
    nUdpPort = nPort > 0 ? nPort : pSettings->value(IDS_UDPPORT, IDS_UDPPORT_VAL).toInt();
    multicastAddress = QHostAddress(pSettings->value(IDS_MULTICAST, IDS_MULTICAST_VAL).toString());
    int size = pSettings->beginReadArray(IDS_BROADCASTHDR);
    for (int index = 0; index < size; index++) {
        pSettings->setArrayIndex(index);
        QHostAddress address = QHostAddress(pSettings->value(IDS_BROADCAST).toString());
        if (!broadcastList.contains(address))
            broadcastList.append(address);
    }
    pSettings->endArray();
}

/**
 * \brief wavrUdpNetwork::start
 *  Starts listening to incoming datagrams and receiving them.
 *  Initializes isRunning to true.
 */
void wavrUdpNetwork::start(void) {
    //  start receiving datagrams
    canReceive = startReceiving();
    isRunning = true;
}


/**
 * \brief wavrUdpNetwork::stop
 *  Disconnects the readyRead signal, leaves multicast group, and closes the socket connection.
 */
void wavrUdpNetwork::stop(void) {
    disconnect(pUdpReceiver, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    if(pUdpReceiver->state() == QAbstractSocket::BoundState) {
        wavrTrace::write("Leaving multicast group " + multicastAddress.toString() + " on interface " +
                         multicastInterface.humanReadableName());
        bool left = pUdpReceiver->leaveMulticastGroup(multicastAddress, multicastInterface);
        pUdpReceiver->close();
        wavrTrace::write(left ? "Success" : "Failed");
    }
    isRunning = false;
}

/**
 * @param lpszLocalId   Sets the localId
 */
void wavrUdpNetwork::setLocalId(QString* lpszLocalId) {
    localId = *lpszLocalId;
}

/**
 *  Sends datagram packet to a multicast address and broadcast list specified in settings.
 * @param lpszData  Datagram packet to be broadcasted.
 */
void wavrUdpNetwork::sendBroadcast(QString* lpszData) {
    if(!isRunning) {
        wavrTrace::write("Warning: UDP server not running. Broadcast not sent");
        return;
    }

    QByteArray datagram = lpszData->toUtf8();
    sendDatagram(multicastAddress, datagram);
    for(int index = 0; index < broadcastList.count(); index++) {
        sendDatagram(broadcastList.at(index), datagram);
    }
}

/**
 *  checks for the multicast address change.
 *  If yes,
 *  1. Then leaves already associated multicast group and joins the new multicast address.
 *  2. Redifines broadcast list and check list does not contain the new multicast address
 */
void wavrUdpNetwork::settingsChanged(void) {
    QHostAddress address = QHostAddress(pSettings->value(IDS_MULTICAST, IDS_MULTICAST_VAL).toString());
    if (multicastAddress != address) {
        if(pUdpReceiver->state() == QAbstractSocket::BoundState) {
            wavrTrace::write("Leaving mulitcast group " + multicastAddress.toString() + " on interface " +
                             multicastInterface.humanReadableName());
            bool left = pUdpReceiver->leaveMulticastGroup(multicastAddress, multicastInterface);
            wavrTrace::write((left ? "Success" : "Failed"));
        }
        multicastAddress = address;
        wavrTrace::write("Joining multicast group " + multicastAddress.toString() + " on interface " +
                         multicastInterface.humanReadableName());
        bool joined = pUdpReceiver->joinMulticastGroup(multicastAddress, multicastInterface);
        wavrTrace::write((joined ? "Success" : "Failed"));
    }
    broadcastList.clear();
    // append defbroadcast address in list
    broadcastList.append(defBroadcast);
    //redifine broadcast list and check list does not contain the new multicast address
    int size = pSettings->beginReadArray(IDS_BROADCASTHDR);
    for (int index = 0; index < size; index++) {
        pSettings->setArrayIndex(index);
        QHostAddress address = QHostAddress(pSettings->value(IDS_BROADCAST).toString());
        if (!broadcastList.contains(address))
            broadcastList.append(address);
    }
    pSettings->endArray();
}

/**
 *  Sets the multicast interface variable.
 * @param networkInterface
 */
void wavrUdpNetwork::setMulticastInterface(const QNetworkInterface& networkInterface) {
    multicastInterface = networkInterface;
}

/**
 *  Defines the broadcast address by ORing the host address and bit inversed subnet mask and appends this address in broadcastList
 * @param szAddress Sets the ipaddress variable
 * @param szSubnet  Sets the subnetmask variable
 */
void wavrUdpNetwork::setIPAddress(const QString &szAddress, const QString &szSubnet) {
    ipAddress = QHostAddress(szAddress);
    subnetMask = QHostAddress(szSubnet);
    setDefaultBroadcast();
    if (!broadcastList.contains(defBroadcast))
        broadcastList.append(defBroadcast);
}

/**
 *  process the pending datagrams present in socket and ready & parse them
 */
void wavrUdpNetwork::processPendingDatagrams(void) {
    while(pUdpReceiver->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(pUdpReceiver->pendingDatagramSize());
        QHostAddress address;
        pUdpReceiver->readDatagram(datagram.data(), datagram.size(), &address);
        QString szAddress = address.toString();
        parseDatagram(&szAddress, datagram);
    }
}

/**
 *  writes the datagram to the socket and sends them to remoteAddress
 * @param remoteAddress Address of the destinee.
 * @param datagram  Data to be sent.
 */
void wavrUdpNetwork::sendDatagram(QHostAddress remoteAddress, QByteArray& datagram) {
  if(!isRunning)
      return;

  wavrTrace::write("Sending UDP datagram to " + remoteAddress.toString() + ":" + QString::number(nUdpPort));
  pUdpSender->writeDatagram(datagram.data(),datagram.size(), remoteAddress, nUdpPort);
}

/**
 *  Joins the multicast group and listens for incoming packets.
 */
bool wavrUdpNetwork::startReceiving(void) {
    wavrTrace::write("Binding UDP listener to port " + QString::number(nUdpPort));

    if(pUdpReceiver->bind(QHostAddress::AnyIPv4, nUdpPort)) {
        wavrTrace::write("Success");
        wavrTrace::write("Joining multicast group " + multicastAddress.toString() +
            " on interface " + multicastInterface.humanReadableName());
        bool joined = pUdpReceiver->joinMulticastGroup(multicastAddress, multicastInterface);
        wavrTrace::write((joined ? "Success" : "Failed"));
        connect(pUdpReceiver, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
        return true;
    }

    wavrTrace::write("Failed");
    return false;
}

void wavrUdpNetwork::parseDatagram(QString* lpszAddress, QByteArray& datagram) {
    wavrTrace::write("UDP datagram received from " + *lpszAddress);
    DatagramHeader* pHeader = new DatagramHeader(DT_Broadcast, QString(), *lpszAddress);
    QString szData = QString::fromUtf8(datagram.data(), datagram.length());
    emit broadcastReceived(pHeader, &szData);
}

void wavrUdpNetwork::setDefaultBroadcast(void) {
    if(ipAddress.protocol() != QAbstractSocket::IPv4Protocol)
        return;

    // The network broadcast address is obtained by ORing the host address and
    // bit inversed subnet mask
    quint32 ipv4 = ipAddress.toIPv4Address();
    quint32 invMask = ~(subnetMask.toIPv4Address());
    defBroadcast = QHostAddress(ipv4 | invMask);
}
