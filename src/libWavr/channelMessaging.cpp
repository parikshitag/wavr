/****************************************************************************
** Class: MsgStream
** Description: Handles transmission and reception of TCP streaming messages.
****************************************************************************/

#include "channelMessaging.h"

MsgStream::MsgStream(void) {
    socket = NULL;
    reading = false;
}

MsgStream::MsgStream(QString szLocalId, QString szPeerId, QString szPeerAddress, int nPort) {
    localId = szLocalId;
    peerId = szPeerId;
    peerAddress = szPeerAddress;
    port = nPort;
    socket = NULL;
    reading = false;
    outDataLen = 0;
    inDataLen = 0;
}

void MsgStream::init(void) {
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    QHostAddress hostAddress(peerAddress);
    socket->connectToHost(hostAddress, port);
}

void MsgStream::init(QTcpSocket *socket) {
    this->socket = socket;
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
}

void MsgStream::stop(void) {
    if(socket && socket->isOpen())
        socket->close();
}

void MsgStream::sendMessage(QByteArray& data) {
    qint32 dataLen = sizeof(quint32) + data.length();
    outDataLen += dataLen;
    outData.resize(dataLen);

    QDataStream stream(&outData, QIODevice::WriteOnly);
    stream << (quint32)data.length();
    stream.writeRawData(data.data(), data.length());

    qint64 numBytesWritten = socket->write(outData);
    if (numBytesWritten < 0);
        //error socket write failed
}

void MsgStream::connected(void) {
    outData = localId.toLocal8Bit();
    outData.insert(0, "MSG");
}
