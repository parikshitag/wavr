
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
void wavrTcpNetwork::start(void){
    isRunning = server->listen(QHostAddress::Any, tcpPort);
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
        //error TCP server not running. Unable to connect.
        return;
    }

    // Connecting to the user
    MsgStream* msgStream = new MsgStream(localId, *lpszUserId, *lpszAddress, tcpPort);
    connect(msgStream, SIGNAL(connectionLost(QString*)),
            this, SLOT(mszStream_connectionLost(QString*)));
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
        //Warning: TCP server not running. Message not sent
        return;
    }

    MsgStream* msgStream;

    if(lpszReceiverId->compare(localId) == 0)
        msgStream = locMsgStream;
    else
        msgStream = messageMap.value(*lpszReceiverId);

    if(msgStream) {
        // Sending TCP data stream to user
        QByteArray clearData = lpszData->toUtf8();
        if (clearData.isEmpty()) {
            // Warning: Message could not be sent
            return;
        }
        msgStream->sendMessage(clearData);
        return;
    }

    // Warning: Socket not found. Message sending failed
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
    // New connection received
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
    if (buffer.startsWith(ST_MESSAGE)) {
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
    QByteArray clearData;
    QString szMessage;

    //TCP stream type pHeader->type received from user userId at address

    switch(pHeader->type) {
    case DT_Message:
        if(clearData.isEmpty()) {
            // Warning: message could not be retrieved
            break;
        }
        szMessage = QString::fromUtf8(clearData.data(), clearData.length());
        emit messageReceived(pHeader, &szMessage);
        break;
    default:
        break;
    }
}

/**
 *  Creates a new messaging channel against the request from the peer.
 * @param lpszUserId    User id of peer that sent the request.
 * @param pSocket       Socket of the peer that sent the request.
 */
void wavrTcpNetwork::addMsgSocket(QString *lpszUserId, QTcpSocket *pSocket) {
    QString address = pSocket->peerAddress().toString();
    MsgStream* msgStream = new MsgStream(localId, *lpszUserId, address, tcpPort);
    connect(msgStream, SIGNAL(connectionLost(QString*)),
            this, SLOT(msgStream_connectionLost(QString*)));
    connect(msgStream, SIGNAL(messageReceived(QString*,QString*,QByteArray&)),
            this, SLOT(receiveMessage(QString*,QString*,QByteArray&)));
    messageMap.insert(*lpszUserId, msgStream);
    msgStream->init(pSocket);
}
