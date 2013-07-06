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


#include <QDir>
#include "channelFile.h"

const qint64 bufferSize = 65535;

/****************************************************************************
** Class: FileSender
** Description: Handles sending files.
****************************************************************************/
FileSender::FileSender(void) {
}

FileSender::FileSender(QString szId, QString szLocalId, QString szPeerId, QString szFilePath,
    QString szFileName, qint64 nFileSize, QString szAddress, int nPort, FileType nType) {

        id = szId;
        localId = szLocalId;
        peerId = szPeerId;
        filePath = szFilePath;
        fileName = szFileName;
        fileSize = nFileSize;
        address = szAddress;
        port = nPort;
        active = false;
        mile = fileSize / 36;
        milestone = mile;
        file = NULL;
        socket = NULL;
        timer = NULL;
        type = nType;
}

FileSender::~FileSender(void) {
}

void FileSender::init(void) {
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    QHostAddress hostAddress(address);
    socket->connectToHost(hostAddress, port);
}

void FileSender::stop(void) {
    active = false;

    if(timer)
        timer->stop();
    if(file && file->isOpen())
        file->close();
    if(socket && socket->isOpen())
        socket->close();
}

void FileSender::connected(void) {
    QByteArray data = id.toLocal8Bit();
    data.append(localId.toLocal8Bit());
    data.insert(0, "FILE");	// insert indicator that this socket handles file transfer
    //	send an id message and then wait for a START message
    //	from receiver, which will trigger readyRead signal
    socket->write(data);
}

void FileSender::disconnected(void) {
    if(active) {
        QString data;
        emit progressUpdated(FM_Send, FO_Error, type, &id, &peerId, &data);
    }
}

void FileSender::readyRead(void) {
    //	message received from receiver, start sending the file
    sendFile();
}

void FileSender::timer_timeout(void) {
    if(!active)
        return;

    QString transferred = QString::number(file->pos());
    emit progressUpdated(FM_Send, FO_Progress, type, &id, &peerId, &transferred);
}

void FileSender::bytesWritten(qint64 bytes) {
    Q_UNUSED(bytes);

    if(!active)
        return;

    qint64 unsentBytes = fileSize - file->pos();

    if(unsentBytes == 0) {
        active = false;
        file->close();
        socket->close();
        emit progressUpdated(FM_Send, FO_Complete, type, &id, &peerId, &filePath);
        return;
    }

    qint64 bytesToSend = (bufferSize < unsentBytes) ? bufferSize : unsentBytes;
    qint64 bytesRead = file->read(buffer, bytesToSend);
    socket->write(buffer, bytesRead);

//	if(file->pos() > milestone) {
//		QString transferred = QString::number(file->pos());
//		emit progressUpdated(FM_Send, FO_Progress, type, &id, &peerId, &transferred);
//		milestone += mile;
//	}
}

void FileSender::sendFile(void) {
    file = new QFile(filePath);

    if(file->open(QIODevice::ReadOnly)) {
        buffer = new char[bufferSize];
        active = true;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
        timer->start(PROGRESS_TIMEOUT);

        qint64 unsentBytes = fileSize - file->pos();
        qint64 bytesToSend = (bufferSize < unsentBytes) ? bufferSize : unsentBytes;
        qint64 bytesRead = file->read(buffer, bytesToSend);
        socket->write(buffer, bytesRead);
    } else {
        socket->close();
        QString data;
        emit progressUpdated(FM_Send, FO_Error, type, &id, &peerId, &data);
    }
}


/****************************************************************************
** Class: FileReceiver
** Description: Handles receiving files.
****************************************************************************/
FileReceiver::FileReceiver(void) {
}

FileReceiver::FileReceiver(QString szId, QString szPeerId, QString szFilePath, QString szFileName,
    qint64 nFileSize, QString szAddress, int nPort, FileType nType) {

        id = szId;
        peerId = szPeerId;
        filePath = szFilePath;
        fileName = szFileName;
        fileSize = nFileSize;
        address = szAddress;
        port = nPort;
        active = false;
        mile = fileSize / 36;
        milestone = mile;
        file = NULL;
        socket = NULL;
        timer = NULL;
        type = nType;
        lastPosition = 0;
        numTimeOuts = 0;
}

FileReceiver::~FileReceiver(void) {
}

void FileReceiver::init(QTcpSocket* socket) {
    this->socket = socket;
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    receiveFile();
    //	now send a START message to sender
    socket->write("START");
}

void FileReceiver::stop(void) {
    bool deleteFile = false;

    active = false;

    if(timer)
        timer->stop();
    if(file && file->isOpen()) {
        deleteFile = (file->pos() < fileSize);
        file->close();
    }
    if(socket && socket->isOpen())
        socket->close();

    if(deleteFile)
        QFile::remove(filePath);
}

void FileReceiver::disconnected(void) {
    if(active) {
        QString data;
        emit progressUpdated(FM_Receive, FO_Error, type, &id, &peerId, &data);
    }
}

void FileReceiver::readyRead(void) {
    if(!active)
        return;

    qint64 bytesReceived = socket->read(buffer, bufferSize);
    file->write(buffer, bytesReceived);

    qint64 unreceivedBytes = fileSize - file->pos();
    if(unreceivedBytes == 0) {
        active = false;
        file->close();
        socket->close();
        emit progressUpdated(FM_Receive, FO_Complete, type, &id, &peerId, &filePath);
        return;
    }

//	if(file->pos() > milestone) {
//		QString transferred = QString::number(file->pos());
//		emit progressUpdated(FM_Receive, FO_Progress, type, &id, &peerId, &transferred);
//		milestone += mile;
//	}
}

void FileReceiver::timer_timeout(void) {
    if(!active)
        return;

    if(lastPosition < file->pos()) {
        lastPosition = file->pos();
        numTimeOuts = 0;
    } else {
        numTimeOuts++;
        if(numTimeOuts > 20) {
            QString data;
            emit progressUpdated(FM_Receive, FO_Error, type, &id, &peerId, &data);
            stop();
            return;
        }
    }

    QString transferred = QString::number(file->pos());
    emit progressUpdated(FM_Receive, FO_Progress, type, &id, &peerId, &transferred);
}

void FileReceiver::receiveFile(void) {
    QDir dir = QFileInfo(filePath).dir();
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());

    file = new QFile(filePath);

    if(file->open(QIODevice::WriteOnly)) {
        buffer = new char[bufferSize];
        active = true;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
        timer->start(PROGRESS_TIMEOUT);
    } else {
        socket->close();
        emit progressUpdated(FM_Receive, FO_Error, type, &id, &peerId, &filePath);
    }
}
