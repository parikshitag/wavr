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


#include "network.h"

/**
 * Creates object of TcpNetwork and UdpNetwork, connect their signals with respective slots.
 * Initializes all variables.
 */
wavrNetwork::wavrNetwork(void) {
    pUdpNetwork = new wavrUdpNetwork();
    pTcpNetwork = new wavrTcpNetwork();

    connect(pUdpNetwork, SIGNAL(broadcastReceived(DatagramHeader*,QString*)),
            this, SLOT(udp_receiveBroadcast(DatagramHeader*,QString*)));
    connect(pTcpNetwork, SIGNAL(newConnection(QString*,QString*)),
            this, SLOT(tcp_newConnection(QString*,QString*)));
    connect(pTcpNetwork, SIGNAL(connectionLost(QString*)),
            this, SLOT(tcp_connectionLost(QString*)));
    connect(pTcpNetwork, SIGNAL(messageReceived(DatagramHeader*,QString*)),
            this, SLOT(tcp_receiveMessage(DatagramHeader*,QString*)));

    pTimer = NULL;
    ipAddress = QString::null;
    subnetMask = QString::null;
    networkInterface = QNetworkInterface();
    interfaceName = QString::null;
    isConnected = false;
    canReceive = false;
}

/**
 * Destroys the network object.
 */
wavrNetwork::~wavrNetwork(void) {
}

/**
 * @brief Initializes TcpNetwork and UdpNetwork class with port specified in XN_PORT.
 * @param pInitParams   wavrXmlMessage class.
 */
void wavrNetwork::init(wavrXmlMessage *pInitParams) {
    //Network Initialized.

    pSettings = new wavrSettings();
    isConnected = getIPAddress();

    int port = pInitParams->data(XML_PORT).toInt();
    pUdpNetwork->init(port);
    pTcpNetwork->init(port);
}

/**
 * @brief Starts the TcpNetwork and UdpNetwork for Communication.
 *  if isConnected then sets the multicastInterface  UdpNetwork class and
 */
void wavrNetwork::start(void){
    //Network Started
    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    pTimer->start(2000);

    if(isConnected) {
        pUdpNetwork->setMulticastInterface(networkInterface);
        pUdpNetwork->setIPAddress(ipAddress, subnetMask);
        pUdpNetwork->start();
        pTcpNetwork->setIPAddress(ipAddress);
        pTcpNetwork->start();
        canReceive = pUdpNetwork->canReceive;
    }
}

/**
 * @brief stops the QTimer, TcpNetwork and UdpNetwork Connections.
 *  Calls the stop method of TcpNetwork and UdpNetwork class.
 */
void wavrNetwork::stop(void) {
    pTimer->stop();

    pUdpNetwork->stop();
    pTcpNetwork->stop();

    //"Network stopped"
}

/**
 * @brief Returns the physical Address of the current network.
 * @return
 */
QString wavrNetwork::physicalAddress(void){
    if(networkInterface.isValid())
           return networkInterface.hardwareAddress();

    return QString::null;
}

/**
 * @brief Setter method for UdpNetwork and TcpNetwork local id.
 * @param lpszLocalId   Local Id for the current network.
 */
void wavrNetwork::setLocalId(QString* lpszLocalId){
    pUdpNetwork->setLocalId(lpszLocalId);
    pTcpNetwork->setLocalId(lpszLocalId);
}

/**
 * @brief Sends a broadcast message in a network.
 * @param lpszData  Broadcast message to be sent.
 */
void wavrNetwork::sendBroadcast(QString* lpszData) {
    pUdpNetwork->sendBroadcast(lpszData);
}

/**
 * @brief Adds a TcpNetwork connection.
 * @param lpszUserId    User Id of the peer to whom connection is to be made.
 * @param lpszAddress   Address of the peer to whom connection is to be made.
 */
void wavrNetwork::addConnection(QString* lpszUserId, QString* lpszAddress) {
    pTcpNetwork->addConnection(lpszUserId, lpszAddress);
}

/**
 * @brief Sends a Message to specified Receipient.
 * @param lpszReceiverId    Id of the peer to whom message is to be sent.
 * @param lpszAddress       Address of the peer to whom message is to be sent.
 * @param lpszData          The actual message to be sent.
 */
void wavrNetwork::sendMessage(QString* lpszReceiverId, QString* lpszAddress, QString* lpszData) {
    Q_UNUSED(lpszAddress);
    pTcpNetwork->sendMessage(lpszReceiverId, lpszData);
}

/**
 * @brief Method for acknowledgement of settings changed in UI.
 *  Calls the UdpNetwork and TcpNetwork settingsChanged methods.
 */
void wavrNetwork::settingsChanged(void) {
    pUdpNetwork->settingsChanged();
    pTcpNetwork->settingsChanged();
}

/**
 * @brief Checks for local Connection every 2 sec.
 *  If isNotConnected or broken then tries to reconnect and sets the respective multicastInterface for UdpNetwork class and IPAddresses for TcpNetwork and UdpNetwork class.
 *  Upon success, emits the connectionStateChanged signal.
 */
void wavrNetwork::timer_timeout(void) {
    bool prev = isConnected;
    isConnected = getIPAddress(false);

    if(prev != isConnected) {

        if(isConnected) {
            pUdpNetwork->setMulticastInterface(networkInterface);
            pUdpNetwork->setIPAddress(ipAddress, subnetMask);
            pUdpNetwork->start();
            pTcpNetwork->setIPAddress(ipAddress);
            pTcpNetwork->start();
            canReceive = pUdpNetwork->canReceive;
        } else {
            pUdpNetwork->stop();
            pTcpNetwork->stop();
        }
        emit connectionStateChanged();
    }
}

/**
 * @brief Emits a broadcastRecieved method with Datagram Header and Data as parameter.
 * @param pHeader   Datagram Header specifying the type, user and address of datagram source.
 * @param lpszData  Actual data which is Received.
 */
void wavrNetwork::udp_receiveBroadcast(DatagramHeader* pHeader, QString* lpszData) {
    emit broadcastReceived(pHeader, lpszData);
}

/**
 * @brief Emits a newConnection signal with User id and address of the peer.
 * @param lpszUserId    User id of the peer which sents the connection request.
 * @param lpszAddress   Address of the peer which sents the connection request.
 */
void wavrNetwork::tcp_newConnection(QString* lpszUserId, QString* lpszAddress) {
    emit newConnection(lpszUserId, lpszAddress);
}

/**
 * @brief Emits a connectionLost signal if some error occurs.
 * @param lpszUserId    User Id of the peer from whom the connection is lost.
 */
void wavrNetwork::tcp_connectionLost(QString* lpszUserId) {
    emit connectionLost(lpszUserId);
}

/**
 * @brief Emits a tcp messageRecieved signal with DatagramHeader and data as the parameter.
 * @param pHeader   Datagram Header specifying the type, user and address of datagram source.
 * @param lpszData  Actual data which is Received.
 */
void wavrNetwork::tcp_receiveMessage(DatagramHeader* pHeader, QString* lpszData) {
    emit messageReceived(pHeader, lpszData);
}

/**
 * @brief Sets the IPAddress of Active network connection.
 * @param verbose   Self expanatory.
 * @return Returns true upon success.
 *  It first checks for already used interface, if it has, then use it and ignore all others.
 *  Then tries to get preferred active network interface from the network interface list.
 *  Upon success returns true, else false.
 */
bool wavrNetwork::getIPAddress(bool verbose) {
    // If an interface is already being used, get it. Ignore all others
    networkInterface = QNetworkInterface::interfaceFromName(interfaceName);
    if(networkInterface.isValid()) {
        QNetworkAddressEntry addressEntry;
        if(isInterfaceUp(&networkInterface) && getIPAddress(&networkInterface, &addressEntry)) {
            ipAddress = addressEntry.ip().toString();
            subnetMask = addressEntry.netmask().toString();
            return true;
        }
        ipAddress = QString::null;
        subnetMask = QString::null;
        return false;
    }

    // Currently, not using preferred connection, since using preferred connection is not
    // working properly.
    // Get the preferred interface name from settings if checking for the first time
    //if(szInterfaceName.isNull())
    //	szInterfaceName = pSettings->value(IDS_CONNECTION, IDS_CONNECTION_VAL).toString();

    //bool usePreferred = (szInterfaceName.compare(IDS_CONNECTION_VAL, Qt::CaseInsensitive) != 0);
    bool usePreferred = false;

    //wavrTrace::write("Checking for active network interface...", verbose);

    //	get a list of all network interfaces available in the system
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();

    bool activeFound = false;

    //	return the preferred interface if it is active
    for(int index = 0; index < allInterfaces.count(); index++) {
        // Skip to the next interface if it is not the preferred one
        // Checked only if searching for the preferred adapter
        if(usePreferred && interfaceName.compare(allInterfaces[index].name()) != 0)
            continue;

        if(isInterfaceUp(&allInterfaces[index])) {
            activeFound = true;
            // wavrTrace::write("Active network interface found: " + allInterfaces[index].humanReadableName(),
              //  verbose);
            QNetworkAddressEntry addressEntry;
            if(getIPAddress(&allInterfaces[index], &addressEntry)) {
                ipAddress = addressEntry.ip().toString();
                subnetMask = addressEntry.netmask().toString();
                networkInterface = allInterfaces[index];
                interfaceName = allInterfaces[index].name();
                return true;
            }
        }
    }
    //lmcTrace::write(QString("Warning: ") + (activeFound ? "No IP address found" : "No active network interface found"),
     //   verbose);
    ipAddress = QString::null;
    subnetMask = QString::null;
    return false;
}

/**
 * @brief Gets the IPAddress of specified network Interface.
 * @param pNetworkInterface Network Interface from whom IP Address is taken.
 * @param pAddressEntry Address entry list containing all associated ip addresses of the interface.
 * @return  Returns true upon success.
 *  Gets the IpAddress from the given list of IPAddresses containing associated IPV4 (and IPV6 if it support) addresses of the specified interface.
 */
bool wavrNetwork::getIPAddress(QNetworkInterface* pNetworkInterface, QNetworkAddressEntry *pAddressEntry) {
    //lmcTrace::write("Querying IP address from network interface...");

    //	get a list of all associated ip addresses of the interface
    QList<QNetworkAddressEntry> addressEntries = pNetworkInterface->addressEntries();
    //	return the first address which is an ipv4 address
    for(int index = 0; index < addressEntries.count(); index++) {
        if(addressEntries[index].ip().protocol() == QAbstractSocket::IPv4Protocol) {
            *pAddressEntry = addressEntries[index];
            //lmcTrace::write("IPv4 address found for network interface.");
            return true;
        }
    }
    // if ipv4 address is not present, check for ipv6 address
    for(int index = 0; index < addressEntries.count(); index++) {
        if(addressEntries[index].ip().protocol() == QAbstractSocket::IPv6Protocol) {
            *pAddressEntry = addressEntries[index];
            //lmcTrace::write("IPv6 address found for network interface.");
            return true;
        }
    }

    //lmcTrace::write("Warning: No IP address found for network interface.");
    return false;
}

/**
 * @brief Sets the Network Interface for all future communications.
 * @param pNetworkInterface If this interface is already being used, get it. Ignore all others.
 * @return Returns true upon success.
 *  First it checks whether network Interface is already being set or not.
 *  Then uses preferred interface name from settings if checking for the first time.
 *  Then calls getNetworkInterface method. @sa wavrNetwork::getNetworkInterface(QNetworkInterface* pNetworkInterface, QString* lpszPreferred);
 */
bool wavrNetwork::getNetworkInterface(QNetworkInterface* pNetworkInterface) {
    // If an interface is already being used, get it. Ignore all others
    if(networkInterface.isValid()) {
        *pNetworkInterface = networkInterface;
        return isInterfaceUp(pNetworkInterface);
    }

    // Get the preferred interface name from settings if checking for the first time
    if(interfaceName.isNull())
        interfaceName = pSettings->value(IDS_CONNECTION, IDS_CONNECTION_VAL).toString();

    QString szPreferred = interfaceName;
    // Currently, hard coding usePreferred to False, since using preferred connection is not
    // working properly.
    //bool usePreferred = (szPreferred.compare(IDS_CONNECTION_VAL, Qt::CaseInsensitive) != 0);
    bool usePreferred = false;

    // Return true if preferred interface is available
    if(usePreferred && getNetworkInterface(pNetworkInterface, &szPreferred))
        return true;

    // Return true if a fallback interface is available
    if(!usePreferred && getNetworkInterface(pNetworkInterface, NULL))
        return true;

    return false;
}

/**
 * @brief Sets the Network Interface for all future communications.
 * @param pNetworkInterface If this interface is already being used, get it. Ignore all others.
 * @param lpszPreferred Preferred Network Interface.
 * @return Returns true upon success.
 * It retuns the preferred interface if it is active from the list of all network interfaces available in the system.
 *  @sa wavrNetwork::getNetworkInterface(QNetworkInterface* pNetworkInterface)
 */
bool wavrNetwork::getNetworkInterface(QNetworkInterface* pNetworkInterface, QString* lpszPreferred) {
    // wavrTrace::write("Checking for active network interface...");

    //	get a list of all network interfaces available in the system
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();

    //	return the preferred interface if it is active
    for(int index = 0; index < allInterfaces.count(); index++) {
        // Skip to the next interface if it is not the preferred one
        // Checked only if searching for the preferred adapter
        if(lpszPreferred && lpszPreferred->compare(allInterfaces[index].name()) != 0)
            continue;

        if(isInterfaceUp(&allInterfaces[index])) {
            *pNetworkInterface = allInterfaces[index];
            // wavrTrace::write("Active network interface found: " + pNetworkInterface->humanReadableName());
            return true;
        }
    }

    // wavrTrace::write("Warning: No active network interface found");
    return false;
}

/**
 * @brief Returns whether specified interface is running or not ie active.
 * @param pNetworkInterface Network Interface to be checked.
 * @return Returns true upon success.
 *  Checks whether a given network interface is actively running or not.
 */
bool wavrNetwork::isInterfaceUp(QNetworkInterface* pNetworkInterface) {
    if(pNetworkInterface->flags().testFlag(QNetworkInterface::IsUp)
        && pNetworkInterface->flags().testFlag(QNetworkInterface::IsRunning)
        && !pNetworkInterface->flags().testFlag(QNetworkInterface::IsLoopBack)) {
            return true;
    }

    return false;
}

/**
 * @brief wavrNetwork::getNetworkAddressEntry
 * @param pAddressEntry Gets the address entries from network interface and fills in this variable.
 * @return Returns true opon success.
 *  Gets the address entries from network interface whether IPV4 or IPV6 and fills the data in pAddressEntry.
 */
bool wavrNetwork::getNetworkAddressEntry(QNetworkAddressEntry* pAddressEntry) {
    //	get the first active network interface
    QNetworkInterface networkInterface;

    if(getNetworkInterface(&networkInterface)) {
        //lmcTrace::write("Querying IP address from network interface...");

        //	get a list of all associated ip addresses of the interface
        QList<QNetworkAddressEntry> addressEntries = networkInterface.addressEntries();
        //	return the first address which is an ipv4 address
        for(int index = 0; index < addressEntries.count(); index++) {
            if(addressEntries[index].ip().protocol() == QAbstractSocket::IPv4Protocol) {
                *pAddressEntry = addressEntries[index];
                this->networkInterface = networkInterface;
                this->interfaceName = networkInterface.name();
                //lmcTrace::write("IPv4 address found for network interface.");
                return true;
            }
        }
        // if ipv4 address is not present, check for ipv6 address
        for(int index = 0; index < addressEntries.count(); index++) {
            if(addressEntries[index].ip().protocol() == QAbstractSocket::IPv6Protocol) {
                *pAddressEntry = addressEntries[index];
                this->networkInterface = networkInterface;
                this->interfaceName = networkInterface.name();
                //lmcTrace::write("IPv6 address found for network interface.");
                return true;
            }
        }

        //lmcTrace::write("Warning: No IP address found for network interface.");
    }

    return false;
}
