/****************************************************************************
**
** This file is part of LAN Messenger.
**
** Copyright (c) 2010 - 2012 Qualia Digital Solutions.
**
** Contact:  qualiatech@gmail.com
**
** LAN Messenger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** LAN Messenger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with LAN Messenger.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#include "trace.h"
#include "messaging.h"
#include <QDirIterator>

void wavrMessaging::receiveProgress(QString* lpszUserId, QString* lpszData) {
    wavrXmlMessage xmlMessage(*lpszData);
    int fileMode = wavrHelper::indexOf(FileModeNames, FM_Max, xmlMessage.data(XML_MODE));
    int fileOp = wavrHelper::indexOf(FileOpNames, FO_Max, xmlMessage.data(XML_FILEOP));
    int fileType = wavrHelper::indexOf(FileTypeNames, FT_Max, xmlMessage.data(XML_FILETYPE));
    QString fileId = xmlMessage.data(XML_FILEID);

    //	determine type of message to be sent to app layer based on file type
    MessageType type;
    switch(fileType) {
    case FT_Normal:
    case FT_Folder:
        type = MT_File;
        break;
    case FT_Avatar:
        type = MT_Avatar;
        break;
    default:
        type = MT_Blank;
        break;
    }

    wavrXmlMessage reply;

    switch(fileOp) {
    case FO_Error:
        reply.addData(XML_MODE, FileModeNames[fileMode]);
        reply.addData(XML_FILETYPE, FileTypeNames[fileType]);
        reply.addData(XML_FILEOP, FileOpNames[FO_Abort]);
        reply.addData(XML_FILEID, fileId);
        sendMessage(type, lpszUserId, &reply);

        if(updateFileTransfer((FileMode)fileMode, (FileOp)fileOp, lpszUserId, &xmlMessage))
            emit messageReceived(type, lpszUserId, &xmlMessage);
        break;
    case FO_Progress:
    case FO_Complete:
        if(updateFileTransfer((FileMode)fileMode, (FileOp)fileOp, lpszUserId, &xmlMessage))
            emit messageReceived(type, lpszUserId, &xmlMessage);
        break;
    }
}

void wavrMessaging::prepareFile(MessageType type, qint64 msgId, bool retry, QString* lpszUserId, wavrXmlMessage* pMessage) {
    Q_UNUSED(type);
    Q_UNUSED(msgId);
    Q_UNUSED(retry);

    int fileOp = wavrHelper::indexOf(FileOpNames, FO_Max, pMessage->data(XML_FILEOP));
    int fileMode = wavrHelper::indexOf(FileModeNames, FM_Max, pMessage->data(XML_MODE));

    User* user = getUser(lpszUserId);
    QString szMessage;

    wavrTrace::write("Sending file message type " + QString::number(fileOp) + " to user " + *lpszUserId
        + ", Mode: " + QString::number(fileMode));

    switch(fileOp) {
    case FO_Request:
        //  New file transfer request, add to file transfer list.
        addFileTransfer(FM_Send, lpszUserId, pMessage);
        break;
    case FO_Accept:
        updateFileTransfer(FM_Receive, (FileOp)fileOp, lpszUserId, pMessage);
        // pMessage now contains the generated id, file mode and other details.
        // Convert this to string now.
        szMessage = pMessage->toString();
        pNetwork->initReceiveFile(&user->id, &user->address, &szMessage);
        break;
    case FO_Decline:
        updateFileTransfer(FM_Receive, (FileOp)fileOp, lpszUserId, pMessage);
        break;
    case FO_Cancel:
        updateFileTransfer((FileMode)fileMode, (FileOp)fileOp, lpszUserId, pMessage);
        szMessage = pMessage->toString();
        pNetwork->fileOperation((FileMode)fileMode, &user->id, &szMessage);
        break;
    }
}

//void wavrMessaging::prepareFolder(MessageType type, qint64 msgId, bool retry, QString* lpszUserId, wavrXmlMessage* pMessage) {
//    Q_UNUSED(type);
//    Q_UNUSED(msgId);
//    Q_UNUSED(retry);

//    int folderOp = wavrHelper::indexOf(FileOpNames, FO_Max, pMessage->data(XML_FILEOP));
//    int folderMode = wavrHelper::indexOf(FileModeNames, FM_Max, pMessage->data(XML_MODE));

//    wavrTrace::write("Sending folder message type " + QString::number(folderOp) + " to user " + *lpszUserId
//        + ", Mode: " + QString::number(folderMode));

//    switch(folderOp) {
//    case FO_Request:
//        //  New file transfer request, add to file transfer list.
//        addFolderTransfer(FM_Send, lpszUserId, pMessage);
//        break;
//    case FO_Accept:
//        //  Upper layers send folder id in FileId field, copy to FolderId field
//        pMessage->addData(XML_FOLDERID, pMessage->data(XML_FILEID));
//        updateFolderTransfer(FM_Receive, (FileOp)folderOp, lpszUserId, pMessage);
//        break;
//    case FO_Decline:
//        pMessage->addData(XML_FOLDERID, pMessage->data(XML_FILEID));
//        updateFolderTransfer(FM_Receive, (FileOp)folderOp, lpszUserId, pMessage);
//        break;
//    case FO_Cancel:
//        pMessage->addData(XML_FOLDERID, pMessage->data(XML_FILEID));
//        updateFolderTransfer((FileMode)folderMode, (FileOp)folderOp, lpszUserId, pMessage);
//        break;
//    }
//}

void wavrMessaging::processFile(MessageHeader* pHeader, wavrXmlMessage* pMessage) {
    int fileMode = wavrHelper::indexOf(FileModeNames, FM_Max, pMessage->data(XML_MODE));
    int fileOp = wavrHelper::indexOf(FileOpNames, FO_Max, pMessage->data(XML_FILEOP));
    QString szMessage;

    //  Reverse file mode to match local mode. ie, message from sender with mode Send
    //  will be translated to Receive at receiver side.
    fileMode = fileMode == FM_Send ? FM_Receive : FM_Send;
    pMessage->removeData(XML_MODE);
    pMessage->addData(XML_MODE, FileModeNames[fileMode]);

    wavrTrace::write("Processing file message type " + QString::number(fileOp) + " from user " +
        pHeader->userId + ", Mode: " + QString::number(fileMode));
    switch(fileOp) {
    case FO_Request:
        if(addFileTransfer(FM_Receive, &pHeader->userId, pMessage))
            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
        break;
    case FO_Accept:
        if(updateFileTransfer(FM_Send, (FileOp)fileOp, &pHeader->userId, pMessage))
            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
        szMessage = pMessage->toString();
        pNetwork->initSendFile(&pHeader->userId, &pHeader->address, &szMessage);
        break;
//    case FO_Decline:
//        if(updateFileTransfer(FM_Send, (FileOp)fileOp, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        break;
//    case FO_Cancel:
//    case FO_Abort:
//        if(updateFileTransfer((FileMode)fileMode, (FileOp)fileOp, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        szMessage = pMessage->toString();
//        pNetwork->fileOperation((FileMode)fileMode, &pHeader->userId, &szMessage);
//        break;
    default:
        break;
    }
}

//void wavrMessaging::processFolder(MessageHeader* pHeader, wavrXmlMessage* pMessage) {
//    int fileMode = wavrHelper::indexOf(FileModeNames, FM_Max, pMessage->data(XML_MODE));
//    int fileOp = wavrHelper::indexOf(FileOpNames, FO_Max, pMessage->data(XML_FILEOP));

//    //  Reverse file mode to match local mode. ie, message from sender with mode Send
//    //  will be translated to Receive at receiver side.
//    fileMode = fileMode == FM_Send ? FM_Receive : FM_Send;
//    pMessage->removeData(XML_MODE);
//    pMessage->addData(XML_MODE, FileModeNames[fileMode]);

//    wavrTrace::write("Processing folder message type " + QString::number(fileOp) + " from user " +
//        pHeader->userId + ", Mode: " + QString::number(fileMode));

//    switch(fileOp) {
//    case FO_Request:
//        if(addFolderTransfer(FM_Receive, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        break;
//    case FO_Accept:
//        if(updateFolderTransfer(FM_Send, (FileOp)fileOp, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        break;
//    case FO_Decline:
//        if(updateFolderTransfer(FM_Send, (FileOp)fileOp, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        break;
//    case FO_Cancel:
//    case FO_Abort:
//        if(updateFolderTransfer((FileMode)fileMode, (FileOp)fileOp, &pHeader->userId, pMessage))
//            emit messageReceived(pHeader->type, &pHeader->userId, pMessage);
//        break;
//    default:
//        break;
//    }
//}

bool wavrMessaging::addFileTransfer(FileMode fileMode, QString* lpszUserId, wavrXmlMessage* pMessage) {
    int fileType = wavrHelper::indexOf(FileTypeNames, FT_Max, pMessage->data(XML_FILETYPE));
    QString fileId, folderId;
    QFileInfo fileInfo;
    wavrXmlMessage xmlMessage;

    bool emitMsg = true;
    switch(fileMode) {
    case FM_Send:
        fileId = wavrHelper::getUuid();
        fileInfo.setFile(pMessage->data(XML_FILEPATH));
        fileList.append(TransFile(fileId, QString::null, *lpszUserId, fileInfo.filePath(),
            fileInfo.fileName(), fileInfo.size(), fileMode, FO_Request, (FileType)fileType));
        pMessage->addData(XML_FILEID, fileId);
        pMessage->addData(XML_MODE, FileModeNames[fileMode]);
        pMessage->addData(XML_FILENAME, fileInfo.fileName());
        pMessage->addData(XML_FILESIZE, QString::number(fileInfo.size()));
        xmlMessage = pMessage->clone();
        switch(fileType) {
        case FT_Normal:
            qDebug() << "sendnig file filemessaginproc";
            emit messageReceived(MT_File, lpszUserId, &xmlMessage);
            break;
//        case FT_Folder:
//            folderId = pMessage->data(XML_FOLDERID);
//            fileList.last().folderId = folderId;
//            pMessage->addData(XML_RELPATH,
//                QDir(getFolderPath(folderId, *lpszUserId, FM_Send)).relativeFilePath(fileInfo.filePath()));
//            updateFolderTransfer(FM_Send, FO_Init, lpszUserId, pMessage);
//            break;
        default:
            break;
        }
        break;
    case FM_Receive:
        fileList.append(TransFile(pMessage->data(XML_FILEID), QString::null, *lpszUserId, QString::null,
            pMessage->data(XML_FILENAME), pMessage->data(XML_FILESIZE).toLongLong(),
            fileMode, FO_Request, (FileType)fileType));
        switch(fileType) {
        case FT_Avatar:
            xmlMessage.addData(XML_MODE, FileModeNames[FM_Receive]);
            xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Avatar]);
            xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Accept]);
            xmlMessage.addData(XML_FILEID, pMessage->data(XML_FILEID));
            xmlMessage.addData(XML_FILESIZE, pMessage->data(XML_FILESIZE));
            sendMessage(MT_Avatar, lpszUserId, &xmlMessage);
            emitMsg = false;    // Suppress emitting message to upper layers
            break;
//        case FT_Folder:
//            fileList.last().folderId = pMessage->data(XML_FOLDERID);
//            fileList.last().relPath = pMessage->data(XML_RELPATH);
//            xmlMessage.addData(XML_MODE, FileModeNames[FM_Receive]);
//            xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Folder]);
//            xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Accept]);
//            xmlMessage.addData(XML_FILEID, pMessage->data(XML_FILEID));
//            xmlMessage.addData(XML_FILESIZE, pMessage->data(XML_FILESIZE));
//            sendMessage(MT_File, lpszUserId, &xmlMessage);
//            emitMsg = false;    // Suppress emitting message to upper layers
//            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return emitMsg;
}

bool wavrMessaging::updateFileTransfer(FileMode fileMode, FileOp fileOp, QString* lpszUserId, wavrXmlMessage* pMessage) {
    QString fileId = pMessage->data(XML_FILEID);
    int fileType = wavrHelper::indexOf(FileTypeNames, FT_Max, pMessage->data(XML_FILETYPE));

    bool emitMsg = false;
    for(int index = 0; index < fileList.count(); index++) {
        TransFile transFile = fileList.at(index);
        if(transFile.userId == *lpszUserId && transFile.id == fileId && transFile.mode == fileMode) {
            QString filePath, fileName, folderId;
            QDir cacheDir;
            wavrXmlMessage xmlMessage;
            emitMsg = true;
            switch(fileOp) {
            case FO_Accept:
                if(fileMode == FM_Send) {
                    fileName = fileList[index].name;
                    filePath = fileList[index].path;
                    pMessage->removeData(XML_FILEPATH);
                    pMessage->addData(XML_FILEPATH, filePath);
                    pMessage->removeData(XML_FILENAME);
                    pMessage->addData(XML_FILENAME, fileName);
                    switch(fileType) {
                    case FT_Avatar:
                    case FT_Folder:
                        emitMsg = false;
                        break;
                    default:
                        break;
                    }
                } else {
                    switch(fileType) {
//                    case FT_Normal:
//                        //  set valid free file name and correct path
//                        fileName = getFreeFileName(transFile.name);
//                        filePath = QDir(StdLocation::fileStorageDir()).absoluteFilePath(fileName);
//                        fileList[index].name = fileName;
//                        fileList[index].path = filePath;
//                        pMessage->removeData(XML_FILEPATH);
//                        pMessage->addData(XML_FILEPATH, filePath);
//                        pMessage->removeData(XML_FILENAME);
//                        pMessage->addData(XML_FILENAME, fileName);
//                        xmlMessage = pMessage->clone();
//                        emit messageReceived(MT_File, lpszUserId, &xmlMessage);
//                        break;
                    case FT_Avatar:
                        cacheDir = QDir(StdLocation::cacheDir());
                        fileName = "avt_" + *lpszUserId + "_part.png";
                        filePath = cacheDir.absoluteFilePath(fileName);
                        fileList[index].name = fileName;
                        fileList[index].path = filePath;
                        pMessage->addData(XML_FILEPATH, filePath);
                        pMessage->addData(XML_FILENAME, fileName);
                        emitMsg = false;
                        break;
//                    case FT_Folder:
//                        folderId = transFile.folderId;
//                        fileName = transFile.relPath;
//                        filePath = QDir(getFolderPath(folderId, *lpszUserId, FM_Receive)).absoluteFilePath(fileName);
//                        fileList[index].path = filePath;
//                        pMessage->removeData(XML_FILEPATH);
//                        pMessage->addData(XML_FILEPATH, filePath);
//                        break;
                    default:
                        break;
                    }
                }
                break;
            case FO_Decline:
                switch(fileType) {
                case FT_Avatar:
                    emitMsg = false;
                default:
                    break;
                }
                fileList.removeAt(index);
                break;
            case FO_Cancel:
                fileList[index].op = fileOp;
                switch(fileType) {
                case FT_Normal:
                    xmlMessage = pMessage->clone();
                    emit messageReceived(MT_File, lpszUserId, &xmlMessage);
                    break;
                case FT_Avatar:
                case FT_Folder:
                    emitMsg = false;
                default:
                    break;
                }
                fileList.removeAt(index);
                break;
            case FO_Complete:
                fileList[index].op = fileOp;
                if(fileMode == FM_Send) {
                    switch(fileType) {
                    case FT_Avatar:
                        emitMsg = false;
//                    case FT_Folder:
//                        pMessage->addData(XML_FOLDERID, transFile.folderId);
//                        updateFolderTransfer(FM_Send, FO_Next, lpszUserId, pMessage);
//                        emitMsg = false;
//                        break;
                    default:
                        break;
                    }
                } else {
                    switch(fileType) {
                    case FT_Avatar:
                        cacheDir = QDir(StdLocation::cacheDir());
                        fileName = "avt_" + *lpszUserId + ".png";
                        filePath = cacheDir.absoluteFilePath(fileName);
                        QFile::remove(filePath);
                        QFile::rename(fileList[index].path, filePath);
                        fileList[index].path = filePath;
                        updateUser(MT_Avatar, *lpszUserId, filePath);
                        pMessage->removeData(XML_FILEPATH);
                        pMessage->addData(XML_FILEPATH, filePath);
                        break;
//                    case FT_Folder:
//                        pMessage->addData(XML_FOLDERID, transFile.folderId);
//                        updateFolderTransfer(FM_Receive, FO_Next, lpszUserId, pMessage);
//                        emitMsg = false;
//                        break;
                    default:
                        break;
                    }
                }
                fileList.removeAt(index);
                break;
            case FO_Error:
            case FO_Abort:
                switch(fileType) {
                case FT_Avatar:
                    emitMsg = false;
//                case FT_Folder:
//                    pMessage->addData(XML_FOLDERID, transFile.folderId);
//                    updateFolderTransfer((FileMode)fileMode, (FileOp)fileOp, lpszUserId, pMessage);
//                    emitMsg = false;
//                    break;
                default:
                    break;
                }
                if(fileMode == FM_Send) {
                    //  The file transfer cannot be removed from the list in this case
                    //  because when receiver cancels, the sender gets an error first
                    //  and then the cancel message.
                } else
                    fileList.removeAt(index);
                break;
            case FO_Progress:
                fileList[index].pos = pMessage->data(XML_FILESIZE).toLongLong();
                switch(fileType) {
                case FT_Avatar:
                    emitMsg = false;
                    break;
//                case FT_Folder:
//                    pMessage->addData(XML_FOLDERID, transFile.folderId);
//                    updateFolderTransfer((FileMode)fileMode, (FileOp)fileOp, lpszUserId, pMessage);
//                    emitMsg = false;
//                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        }
    }

    return emitMsg;
}

//QString wavrMessaging::getFreeFileName(QString fileName) {
//    QString freeFileName = fileName;

//    QString fileDir = StdLocation::fileStorageDir();
//    QDir dir(fileDir);
//    QString filePath = dir.absoluteFilePath(fileName);
//    QString baseName = fileName.mid(0, fileName.lastIndexOf("."));
//    QString ext = fileName.mid(fileName.lastIndexOf("."));

//    int fileCount = 0;
//    while(QFile::exists(filePath)) {
//        fileCount++;
//        freeFileName = baseName + " [" + QString::number(fileCount) + "]" + ext;
//        filePath = dir.absoluteFilePath(freeFileName);
//    }

//    return freeFileName;
//}

//bool wavrMessaging::addFolderTransfer(FileMode folderMode, QString* lpszUserId, wavrXmlMessage* pMessage) {
//    int folderType = wavrHelper::indexOf(FileTypeNames, FT_Max, pMessage->data(XML_FILETYPE));

//    QString folderId;
//    QFileInfo fileInfo;
//    wavrXmlMessage xmlMessage;

//    bool emitMsg = true;
//    QDirIterator iterator(QDir(pMessage->data(XML_FILEPATH)), QDirIterator::Subdirectories);
//    switch(folderMode) {
//    case FM_Send:
//        folderId = wavrHelper::getUuid();
//        fileInfo.setFile(pMessage->data(XML_FILEPATH));
//        folderList.append(TransFolder(folderId, *lpszUserId, fileInfo.filePath(),
//            fileInfo.fileName(), 0, folderMode, FO_Request, (FileType)folderType, 0));
//        //  fetch list of files to be sent
//        while(iterator.hasNext()) {
//            iterator.next();
//            if(iterator.fileInfo().isFile()) {
//                folderList.last().fileList.append(iterator.filePath());
//                folderList.last().size += iterator.fileInfo().size();
//                folderList.last().fileCount++;
//            }
//        }
//        pMessage->addData(XML_FOLDERID, folderId);
//        pMessage->addData(XML_FILEID, folderId); //  Assign the folder id to FileId field for uniformity
//        pMessage->addData(XML_MODE, FileModeNames[folderMode]);
//        pMessage->addData(XML_FILENAME, fileInfo.fileName());
//        pMessage->addData(XML_FILESIZE, QString::number(folderList.last().size));
//        pMessage->addData(XML_FILECOUNT, QString::number(folderList.last().fileCount));
//        xmlMessage = pMessage->clone();
//        emit messageReceived(MT_Folder, lpszUserId, &xmlMessage);
//        break;
//    case FM_Receive:
//        folderList.append(TransFolder(pMessage->data(XML_FOLDERID), *lpszUserId, QString::null,
//            pMessage->data(XML_FILENAME), pMessage->data(XML_FILESIZE).toLongLong(),
//            folderMode, FO_Request, (FileType)folderType, pMessage->data(XML_FILECOUNT).toInt()));
//        break;
//    default:
//        break;
//    }

//    return emitMsg;
//}

//bool wavrMessaging::updateFolderTransfer(FileMode folderMode, FileOp folderOp, QString* lpszUserId, wavrXmlMessage* pMessage) {
//    QString folderId = pMessage->data(XML_FOLDERID);

//    bool emitMsg = false;
//    for(int index = 0; index < folderList.count(); index++) {
//        TransFolder transFolder = folderList.at(index);
//        if(transFolder.userId == *lpszUserId && transFolder.id == folderId && transFolder.mode == folderMode) {
//            emitMsg = true;
//            QString folderName, folderPath;
//            int fileIndex;
//            wavrXmlMessage xmlMessage;
//            switch(folderOp) {
//            case FO_Accept:
//                if(folderMode == FM_Send) {
//                    //  Send the first file in the list
//                    if(!folderList[index].fileList.isEmpty()) {
//                        xmlMessage.addData(XML_FOLDERID, folderList[index].id);
//                        xmlMessage.addData(XML_MODE, FileModeNames[FM_Send]);
//                        xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Folder]);
//                        xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Request]);
//                        xmlMessage.addData(XML_FILEPATH, folderList[index].fileList.first());
//                        sendMessage(MT_File, lpszUserId, &xmlMessage);
//                    }
//                    folderName = folderList[index].name;
//                    folderPath = folderList[index].path;
//                    pMessage->removeData(XML_FILEPATH);
//                    pMessage->addData(XML_FILEPATH, folderPath);
//                    pMessage->removeData(XML_FILENAME);
//                    pMessage->addData(XML_FILENAME, folderName);
//                } else {
//                    //  set valid free folder name and correct path
//                    folderName = getFreeFolderName(transFolder.name);
//                    folderPath = StdLocation::fileStorageDir() + "/" + folderName;
//                    folderList[index].name = folderName;
//                    folderList[index].path = folderPath;
//                    folderList[index].currentFile = pMessage->data(XML_FILEID);
//                    pMessage->removeData(XML_FILEPATH);
//                    pMessage->addData(XML_FILEPATH, folderPath);
//                    pMessage->removeData(XML_FILENAME);
//                    pMessage->addData(XML_FILENAME, folderName);
//                    QDir(StdLocation::fileStorageDir()).mkdir(folderName);
//                    xmlMessage = pMessage->clone();
//                    emit messageReceived(MT_Folder, lpszUserId, &xmlMessage);
//                }
//                folderList[index].lastUpdated = QDateTime::currentDateTime();
//                break;
//            case FO_Decline:
//                folderList.removeAt(index);
//                break;
//            case FO_Cancel:
//                xmlMessage = pMessage->clone();
//                xmlMessage.addData(XML_FILEID, transFolder.currentFile);
//                emit messageReceived(MT_File, lpszUserId, &xmlMessage);
//                sendMessage(MT_File, lpszUserId, &xmlMessage);
//                folderList.removeAt(index);
//                break;
//            case FO_Complete:   //  This will never be executed
//                folderList.removeAt(index);
//                break;
//            case FO_Error:
//            case FO_Abort:
//                pMessage->removeData(XML_FILEID);
//                pMessage->addData(XML_FILEID, pMessage->data(XML_FOLDERID));
//                emit messageReceived(MT_Folder, lpszUserId, pMessage);
//                if(folderMode == FM_Send) {
//                    //  Cannot be removed, same reason as file transfer
//                } else
//                    folderList.removeAt(index);
//                break;
//            case FO_Progress:
//                pMessage->removeData(XML_FILEID);
//                pMessage->addData(XML_FILEID, pMessage->data(XML_FOLDERID));
//                folderList[index].filePos = pMessage->data(XML_FILESIZE).toLongLong();
//                pMessage->removeData(XML_FILESIZE);
//                pMessage->addData(XML_FILESIZE, QString::number(folderList[index].pos + folderList[index].filePos));
//                emit messageReceived(MT_Folder, lpszUserId, pMessage);
//                folderList[index].lastUpdated = QDateTime::currentDateTime();
//                break;
//            case FO_Init:
//                folderList[index].currentFile = pMessage->data(XML_FILEID);
//                break;
//            case FO_Next:
//                fileIndex = ++folderList[index].fileIndex;
//                pMessage->removeData(XML_FILEID);
//                pMessage->addData(XML_FILEID, pMessage->data(XML_FOLDERID));
//                if(fileIndex < folderList[index].fileCount) {
//                    if(folderMode == FM_Send) {
//                        xmlMessage.addData(XML_FOLDERID, folderList[index].id);
//                        xmlMessage.addData(XML_MODE, FileModeNames[FM_Send]);
//                        xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Folder]);
//                        xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Request]);
//                        xmlMessage.addData(XML_FILEPATH, folderList[index].fileList.at(fileIndex));
//                        sendMessage(MT_File, lpszUserId, &xmlMessage);
//                    }
//                    folderList[index].pos += QFileInfo(pMessage->data(XML_FILEPATH)).size();
//                    //  If the files are small, the progress timer will not have time to trigger since
//                    //  the file transfer will be completed within its timeout period. In such cases
//                    //  we manually emit a progress message if a progress update has not been sent for
//                    //  more than 1 second (the timeout period).
//                    if(folderList[index].lastUpdated.msecsTo(QDateTime::currentDateTime()) > PROGRESS_TIMEOUT) {
//                        pMessage->removeData(XML_FILEOP);
//                        pMessage->addData(XML_FILEOP, FileOpNames[FO_Progress]);
//                        pMessage->removeData(XML_FILESIZE);
//                        pMessage->addData(XML_FILESIZE, QString::number(folderList[index].pos));
//                        emit messageReceived(MT_Folder, lpszUserId, pMessage);
//                        folderList[index].lastUpdated = QDateTime::currentDateTime();
//                    }
//                } else {
//                    pMessage->removeData(XML_FILEPATH);
//                    pMessage->addData(XML_FILEPATH, folderList[index].path);
//                    folderList[index].op = FO_Complete;
//                    emit messageReceived(MT_Folder, lpszUserId, pMessage);
//                    folderList.removeAt(index);
//                }
//                break;
//            default:
//                break;
//            }
//            break;
//        }
//    }

//    return emitMsg;
//}

//QString wavrMessaging::getFreeFolderName(QString folderName) {
//    QString freeFolderName = folderName;

//    QString fileDir = StdLocation::fileStorageDir();
//    QDir dir(fileDir + "/" + folderName);

//    int fileCount = 0;
//    while(dir.exists()) {
//        fileCount++;
//        freeFolderName = folderName + " [" + QString::number(fileCount) + "]";
//        dir.setPath(fileDir + "/" + freeFolderName);
//    }

//    return freeFolderName;
//}

//QString wavrMessaging::getFolderPath(QString folderId, QString userId, FileMode mode) {
//    for(int index = 0; index < folderList.count(); index++) {
//        if(folderList[index].id == folderId && folderList[index].userId == userId
//                && folderList[index].mode == mode)
//            return folderList[index].path;
//    }

//    return QString::null;
//}
