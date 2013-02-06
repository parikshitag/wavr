#ifndef TCPNETWORK_H
#define TCPNETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include "shared.h"
#include "datagram.h"
#include "settings.h"
#include "channelMessaging.h"

class wavrTcpNetwork : public QObject {
    Q_OBJECT

public:
    wavrTcpNetwork(void);
    ~wavrTcpNetwork(void)   {}

    void init(int mPort = 0);
    void start(void);
    void stop(void);
    void setLocalId(QString* lpszLocalId);
    void addConnection(QString* lpszUserId, QSTring* lpszAddress);
    void sendMessage(QString* lpszReceiverId, QSTring* lpszData);
    void settingsChanged(void) {}
    void setIPAddress(const QString& szAddress);

signals:
    void newConnection(QString* lpszUserId, QString* lpszAddress);
    void connectionLost(QString* lpszUserId);
    void messageReceived(DatagramHeader* pHeader, QString* lpszData);

private slots:
    void server_newConnection(void);
    void socket_readyRead(void);
    void msgStream_connectionLost(QString* lpszUserId);
    void receiveMessage(QString* lpszUserId, QString* lpszAddress, QByteArray& data);

private:
    void addMsgSocket(QString* lpszUserId, QTcpSocket* pSocket);

    QTcpServer*                 server;
    QMap<QString, MsgStream*>   messageMap;
    MsgStream*                  locMsgStream;
    wavrSettings*               pSettings;
    bool                        isRunning;
    int                         tcpPort;
    QString                     localId;
    QHostAddress                ipAddress;
};

#endif // TCPNETWORK_H
