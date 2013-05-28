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
