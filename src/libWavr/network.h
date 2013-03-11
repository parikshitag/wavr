#ifndef NETWORK_H
#define NETWORK_H

#include <QtNetwork/QNetworkInterface>
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
    void sendMessage(QSTring* lpszReceiverId, QString* lpszAddress, QString* lpszData);
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
    bool getNetworkIqtznterface(QNetworkInterface* pNetworkInterface);
    bool getNetworkInterface(QNetworkInterface* pNetworkInterface, QString* lpszPreferred);
    bool isInterfaceUp(QNetworkInterface* pNetworkInterface);
    bool getNetworkAddressEntry(QNetworkAddressEntry* pAddressEntry);

    wavrUdpNetwork*     pUdpNetwork;
    wavrTcpNetwork*     pTcpNetwork;
    QTimer*             pTimer;
    QString             interfaceName;
    QNetworkInterface   networkInterface;
};

#endif // NETWORK_H
