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


#ifndef TCPNETWORK_H
#define TCPNETWORK_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include "shared.h"
#include "datagram.h"
#include "settings.h"
#include "channelMessaging.h"
#include "channelFile.h"

class wavrTcpNetwork : public QObject {
    Q_OBJECT

public:
    wavrTcpNetwork(void);
    ~wavrTcpNetwork(void)   {}

    void init(int nPort = 0);
    void start(void);
    void stop(void);
    void setLocalId(QString* lpszLocalId);
    void addConnection(QString* lpszUserId, QString* lpszAddress);
    void sendMessage(QString* lpszReceiverId, QString* lpszData);
    void initSendFile(QString* lpszReceiverId, QString* lpszAddress, QString* lpszData);
    void initReceiveFile(QString* lpszSenderId, QString* lpszAddress, QString* lpszData);
    void fileOperation(FileMode mode, QString* lpszUserId, QString* lpszData);
    void settingsChanged(void) {}
    void setIPAddress(const QString& szAddress);

signals:
    void newConnection(QString* lpszUserId, QString* lpszAddress);
    void connectionLost(QString* lpszUserId);
    void messageReceived(DatagramHeader* pHeader, QString* lpszData);
    void progressReceived(QString* lpszUserId, QString* lpszData);

private slots:
    void server_newConnection(void);
    void socket_readyRead(void);
    void msgStream_connectionLost(QString* lpszUserId);
    void update(FileMode mode, FileOp op, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData);
    void receiveMessage(QString* lpszUserId, QString* lpszAddress, QByteArray& data);

private:
    void addMsgSocket(QString* lpszUserId, QTcpSocket* pSocket);
    FileSender* getSender(QString id, QString userId);
    FileReceiver* getReceiver(QString id, QString userId);
    void removeSender(FileSender* pSender);
    void removeReceiver(FileReceiver* pReceiver);

    QTcpServer*                 server;
    QList<FileSender*>          sendList;
    QList<FileReceiver*>        receiveList;
    QMap<QString, MsgStream*>   messageMap;
    MsgStream*                  locMsgStream;
    wavrSettings*               pSettings;
    bool                        isRunning;
    int                         tcpPort;
    QString                     localId;
    QHostAddress                ipAddress;
};

#endif // TCPNETWORK_H
