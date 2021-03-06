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



#ifndef CHANNELFILE_H
#define CHANNELFILE_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QFile>
#include "shared.h"

/****************************************************************************
** Class: FileSender
** Description: Handles sending files.
****************************************************************************/
class FileSender : public QObject {
    Q_OBJECT

public:
    FileSender(void);
    FileSender(QString szId, QString szLocalId, QString szPeerId, QString szFilePath, QString szFileName, qint64 nFileSize,
        QString szAddress, int nPort, FileType nType);
    ~FileSender(void);

    void init(void);
    void stop(void);

    QString id;
    QString peerId;
    FileType type;

signals:
    void progressUpdated(FileMode mode, FileOp fileOp, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData);

private slots:
    void connected(void);
    void disconnected(void);
    void readyRead(void);
    void bytesWritten(qint64 bytes);
    void timer_timeout(void);

private:
    void sendFile(void);

    QString localId;
    QString filePath;
    QString fileName;
    qint64 fileSize;
    QString address;
    int port;
    qint64 sentBytes;
    QTcpSocket* socket;
    QFile* file;
    char* buffer;
    bool active;
    qint64 milestone;
    qint64 mile;
    QTimer* timer;
};

/****************************************************************************
** Class: FileReceiver
** Description: Handles receiving files.
****************************************************************************/
class FileReceiver : public QObject {
    Q_OBJECT

public:
    FileReceiver(void);
    FileReceiver(QString szId, QString szPeerId, QString szFilePath, QString szFileName, qint64 nFileSize,
        QString szAddress, int nPort, FileType nType);
    ~FileReceiver(void);

    void init(QTcpSocket* socket);
    void stop(void);

    QString id;
    QString peerId;
    FileType type;

signals:
    void progressUpdated(FileMode mode, FileOp fileOp, FileType type, QString* lpszId, QString* lpszUserId, QString* lpszData);

private slots:
    void disconnected(void);
    void readyRead(void);
    void timer_timeout(void);

private:
    void receiveFile(void);

    QString filePath;
    QString fileName;
    qint64 fileSize;
    QString address;
    int port;
    qint64 sentBytes;
    QTcpSocket* socket;
    QFile* file;
    char* buffer;
    bool active;
    qint64 milestone;
    qint64 mile;
    QTimer* timer;
    int numTimeOuts;
    qint64 lastPosition;
};

#endif // CHANNELFILE_H
