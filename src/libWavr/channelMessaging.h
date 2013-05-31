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
