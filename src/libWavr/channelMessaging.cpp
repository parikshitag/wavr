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

void MsgStream::disconnected(void) {
    emit connectionLost(&peerId);
}

void MsgStream::readyRead(void) {
    qint64 available = socket->bytesAvailable();
    while(available > 0) {
        if(!reading) {
            reading = true;
            QByteArray len = socket->read(4);
            QDataStream stream(len);
            stream >> inDataLen;
            inData.clear();
            QByteArray data = socket->read(inDataLen);
            inData.append(data);
            inDataLen -= data.length();
            available -= (sizeof(quint32) +  data.length());
            if(inDataLen == 0) {
                reading = false;
                emit messageReceived(&peerId, &peerAddress, inData);
            }
        } else {
            QByteArray data = socket->read(inDataLen);
            inData.append(data);
            inDataLen -= data.length();
            available -= data.length();
            if(inDataLen == 0) {
                reading = false;
                emit messageReceived(&peerId, &peerAddress, inData);
            }
        }
    }
}

void MsgStream::bytesWritten(qint64 bytes) {
    outDataLen -= bytes;
    if(outDataLen == 0)
        return;

//    if(outDataLen > 0)
//        lmcTrace::write("Warning: Socket write operation not completed");
//    if(outDataLen < 0)
//        lmcTrace::write("Warning: Socket write overrun");

    //	TODO: handle situation when entire message is not written to stream in one write operation
    //	The following code is not functional currently, hence commented out.
    /*outData = outData.mid(outDataLen);
    socket->write(outData);*/
}
