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


#include "trace.h"
#include "tcpnetwork.h"

/**
 *  Creates object of QTcpServer.
 */
wavrTcpNetwork::wavrTcpNetwork(void){
    ipAddress = QHostAddress::Null;
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(server_newConnection()));
}

/**
 *  Creates an object of settings.
 */
void wavrTcpNetwork::init(int nPort){
    pSettings = new wavrSettings();
    tcpPort = nPort > 0 ? nPort : pSettings->value(IDS_TCPPORT, IDS_TCPPORT_VAL).toInt();
}

/**
 * TcpServer listens for incoming connections over QHostAddress::Any and tcpPort specified.
 */
void wavrTcpNetwork::start(void) {
    wavrTrace::write("Starting TCP server");
    isRunning = server->listen(QHostAddress::AnyIPv4, tcpPort);
    wavrTrace::write(isRunning ? "Success" : "Failed");
}

/**
 * Closes all open sockets ie server and messaging channels
 */
void wavrTcpNetwork::stop(void) {
    server->close();
    // Close all open sockets
    if (locMsgStream)
        locMsgStream->stop();
    QMap<QString, MsgStream*>::const_iterator index = messageMap.constBegin();
    while(index != messageMap.constEnd()) {
        MsgStream* pMsgStream = index.value();
        if (pMsgStream)
            pMsgStream->stop();
        index++;
    }
    isRunning = false;
}

/**
 * @param lpszLocalId   sets the localId
 */
void wavrTcpNetwork::setLocalId(QString *lpszLocalId) {
    localId = *lpszLocalId;
}

/**
 * Opens the socket connection to a TCP server of received datagram source.
 * And creates a seperate channel and add that channel to the list.
 * @param lpszUserId    Id of the peer to connect
 * @param lpszAddress   Address of the peer to connect
 */
void wavrTcpNetwork::addConnection(QString* lpszUserId, QString* lpszAddress) {
    if(!isRunning) {
        wavrTrace::write("Warning: TCP server not running. Unable to connect.");
        return;
    }

    wavrTrace::write("Connecting to user " + *lpszUserId + " at " + *lpszAddress);

    MsgStream* msgStream = new MsgStream(localId, *lpszUserId, *lpszAddress, tcpPort);
    connect(msgStream, SIGNAL(connectionLost(QString*)),
            this, SLOT(msgStream_connectionLost(QString*)));
    connect(msgStream, SIGNAL(messageReceived(QString*,QString*,QByteArray&)),
            this, SLOT(receiveMessage(QString*,QString*,QByteArray&)));

    // if connecting to own machine, this stream will be stored in local message stream, else in list
    if (lpszUserId->compare(localId) == 0)
        locMsgStream = msgStream;
    else
        messageMap.insert(*lpszUserId, msgStream);
    msgStream->init();
}

/**
 * Sends a message to the destinee.
 * @param lpszReceiverId Id of the destinee
 * @param lpszData  Data to be sent
 */
void wavrTcpNetwork::sendMessage(QString *lpszReceiverId, QString *lpszData) {
    if(!isRunning) {
        wavrTrace::write("Warning: TCP server not running. Message not sent");
        return;
    }

    MsgStream* msgStream;

    if(lpszReceiverId->compare(localId) == 0)
        msgStream = locMsgStream;
    else
        msgStream = messageMap.value(*lpszReceiverId);

    if(msgStream) {
        wavrTrace::write("Sending TCP data stream to user " + *lpszReceiverId);
        QByteArray clearData = lpszData->toUtf8();
        if (clearData.isEmpty()) {
            wavrTrace::write("Warning: Message could not be sent");
            return;
        }
        wavrDatagram::addHeader(DT_Message, clearData);
        msgStream->sendMessage(clearData);
        return;
    }

    wavrTrace::write("Warning: Socket not found. Message sending failed");
}

void wavrTcpNetwork::initSendFile(QString* lpszReceiverId, QString* lpszAddress, QString* lpszData) {
    wavrXmlMessage xmlMessage(*lpszData);
    int type = wavrHelper::indexOf(FileTypeNames, FT_Max, xmlMessage.data(XML_FILETYPE));

    FileSender* sender = new FileSender(xmlMessage.data(XML_FILEID), localId, *lpszReceiverId, xmlMessage.data(XML_FILEPATH),
        xmlMessage.data(XML_FILENAME), xmlMessage.data(XML_FILESIZE).toLongLong(), *lpszAddress, tcpPort, (FileType)type);
    connect(sender, SIGNAL(progressUpdated(FileMode, FileOp, FileType, QString*, QString*, QString*)),
        this, SLOT(update(FileMode, FileOp, FileType, QString*, QString*, QString*)));
    sendList.prepend(sender);
    sender->init();
}

/**
 * Setter method
 * @param szAddress sets the IP Address
 */
void wavrTcpNetwork::setIPAddress(const QString &szAddress) {
    ipAddress = QHostAddress(szAddress);
}

/**
 * creates a socket against the incoming request.
 */
void wavrTcpNetwork::server_newConnection(void) {
    wavrTrace::write("New connection received");
    QTcpSocket* socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(socket_readyRead()));
}

/**
 *  Read the socket and assign it to correct channel ie message or file.
 */
void wavrTcpNetwork::socket_readyRead(void) {
    QTcpSocket* socket = (QTcpSocket*) sender();
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(socket_readyRead()));

    QByteArray buffer = socket->read(64);
    if (buffer.startsWith("MSG")) {
        // read user id from socket and assign socket to correct message stream
        QString userId(buffer.mid(3));  // 3 is length of "MSG"
        addMsgSocket(&userId, socket);
    } else if (buffer.startsWith(ST_FILE)){
        // for file handling
    }
}

/**
 *  Emits when a connection to remote peer is lost.
 * @param lpszUserId    user id of disconnected user.
 */
void wavrTcpNetwork::msgStream_connectionLost(QString *lpszUserId) {
    emit connectionLost(lpszUserId);
}

/**
 *  Receives the message, filters it according to the Datagram Type and emits messageReceived signal.
 * @param lpszUserId    user Id of the peer.
 * @param lpszAddress   Address of the peer.
 * @param datagram      Datagram Message.
 */
void wavrTcpNetwork::receiveMessage(QString* lpszUserId, QString* lpszAddress, QByteArray& datagram) {
    DatagramHeader* pHeader = NULL;

    if (!wavrDatagram::getHeader(datagram, &pHeader))
        return;

    pHeader->userId = *lpszUserId;
    pHeader->address = *lpszAddress;
    QByteArray clearData = wavrDatagram::getData(datagram);
    QString szMessage;

    wavrTrace::write("TCP stream type " + QString::number(pHeader->type) +
                     " received from user " + *lpszUserId + " at " + *lpszAddress);

    switch(pHeader->type) {
    case DT_Message:
        if(clearData.isEmpty()) {
            wavrTrace::write("Warning: message could not be retrieved");
            break;
        }
        szMessage = QString::fromUtf8(clearData.data(), clearData.length());
        emit messageReceived(pHeader, &szMessage);
        break;
    default:
        break;
    }
}

void wavrTcpNetwork::update(FileMode mode, FileOp op, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData) {
    wavrXmlMessage xmlMessage;
    xmlMessage.addHeader(XML_FROM, *lpszUserId);
    xmlMessage.addHeader(XML_TO, localId);
    xmlMessage.addData(XML_MODE, FileModeNames[mode]);
    xmlMessage.addData(XML_FILETYPE, FileTypeNames[type]);
    xmlMessage.addData(XML_FILEOP, FileOpNames[op]);
    xmlMessage.addData(XML_FILEID, *lpszId);

    switch(op) {
    case FO_Complete:
    case FO_Error:
        xmlMessage.addData(XML_FILEPATH, *lpszData);
//        if(mode == FM_Send)
//            removeSender(static_cast<FileSender*>(sender()));
//        else
//            removeReceiver(static_cast<FileReceiver*>(sender()));
        break;
    case FO_Progress:
        xmlMessage.addData(XML_FILESIZE, *lpszData);
        break;
    default:
        break;
    }

    QString szMessage = xmlMessage.toString();
    emit progressReceived(lpszUserId, &szMessage);
}

/**
 *  Creates a new messaging channel against the request from the peer.
 * @param lpszUserId    User id of peer that sent the request.
 * @param pSocket       Socket of the peer that sent the request.
 */
void wavrTcpNetwork::addMsgSocket(QString *lpszUserId, QTcpSocket *pSocket) {
    wavrTrace::write("Accepted connection from user " + *lpszUserId);

    QString address = pSocket->peerAddress().toString();
    MsgStream* msgStream = new MsgStream(localId, *lpszUserId, address, tcpPort);
    connect(msgStream, SIGNAL(connectionLost(QString*)),
            this, SLOT(msgStream_connectionLost(QString*)));
    connect(msgStream, SIGNAL(messageReceived(QString*,QString*,QByteArray&)),
            this, SLOT(receiveMessage(QString*,QString*,QByteArray&)));
    messageMap.insert(*lpszUserId, msgStream);
    msgStream->init(pSocket);

    emit newConnection(lpszUserId, &address);
}
