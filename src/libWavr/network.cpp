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

void wavrNetwork::init(
