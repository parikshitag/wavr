#ifndef CHANNELMESSAGING_H
#define CHANNELMESSAGING_H

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QDataStream>

/****************************************************************************
**Description: Handles transmission and reception of TCP streaming messages.
****************************************************************************/
class MsgStream : public QObject {
    Q_OBJECT

public:
    MsgStream(void);
    MsgStream(QString szLocalId, QString szPeerId, QString szPeerAddress, int nPort);
    ~MsgStream(void) {}

    void init(void);
    void init(QTcpSocket* socket);
    void stop(void);
    void sendMessage(QByteArray& data);

signals:
    void connectionLost(QString* lpszUserId);
    void messageReceived(QString* lpszUserId, QString* lpszAddress, QByteArray& data);

private slots:
    void connected(void);
    void disconnected(void);
    void readyRead(void);
    void bytesWritten(qint64 bytes);

private:
    QTcpSocket* socket;
    int port;
    QString localId;
    QString peerId;
    QString peerAddress;
    QByteArray outData;
    QByteArray inData;
    quint32 outDataLen;
    quint32 inDataLen;
    bool reading;
};

#endif // CHANNELMESSAGING_H
