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


#include <QTimer>
#include "chatwindow.h"

const qint64 pauseTime = 5000;

wavrChatWindow::wavrChatWindow(QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, flags)
{
    ui.setupUi(this);
    // Destroy the window when it closes
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAcceptDrops(true);

    pMessageLog = new wavrMessageLog(ui.wgtLog);
    ui.logLayout->addWidget(pMessageLog);
    pMessageLog->setAcceptDrops(false);
    connect(pMessageLog, SIGNAL(messageSent(MessageType,QString*,wavrXmlMessage*)),
            this, SLOT(log_sendMessage(MessageType,QString*,wavrXmlMessage*)));

    int bottomPanelHeight = ui.txtMessage->minimumHeight() + ui.wgtToolBar->minimumHeight();
    QList<int> sizes;
    sizes.append(height() - bottomPanelHeight - ui.splitter->handleWidth());
    sizes.append(bottomPanelHeight);
    ui.splitter->setSizes(sizes);
    ui.splitter->setStyleSheet("QSplitter::handle { image: url("IDR_VGRIP"); }");

    ui.lblInfo->setBackgroundRole(QPalette::Base);
    ui.lblInfo->setAutoFillBackground(true);
    ui.lblInfo->setVisible(false);
    pMessageLog->installEventFilter(this);
    ui.txtMessage->installEventFilter(this);
    infoFlag = IT_Ok;

    localId = QString::null;
    localName = QString::null;
    peerIds.clear();
    peerNames.clear();
    peerStatuses.clear();
    peerCaps.clear();
    threadId = QString::null;
    groupMode = false;
    //dataSaved = false;

    chatState = CS_Blank;
    keyStroke = 0;
}

wavrChatWindow::~wavrChatWindow()
{
}

void wavrChatWindow::init(User *pLocalUser, User *pRemoteUser, bool connected) {
    localId = pLocalUser->id;
    localName = pLocalUser->name;

    peerId = pRemoteUser->id;
    peerIds.insert(peerId, pRemoteUser->id);
    peerNames.insert(peerId, pRemoteUser->name);
    peerStatuses.insert(peerId, pRemoteUser->status);
    peerCaps.insert(peerId, pRemoteUser->caps);

    this->pLocalUser = pLocalUser;

    pMessageLog->localId = localId;
    pMessageLog->peerId = peerId;
    pMessageLog->peerName = pRemoteUser->name;

    //  get the avatar image for the users
    pMessageLog->participantAvatars.insert(localId, pLocalUser->avatarPath);
    pMessageLog->participantAvatars.insert(peerId, pRemoteUser->avatarPath);

    createSmileyMenu();
    createToolBar();

    bConnected = connected;
    if(!bConnected)
        showStatus(IT_Disconnected, true);

    int index = wavrHelper::statusIndexFromCode(pRemoteUser->status);
    if(index != -1) {
        setWindowIcon(QIcon(statusPic[index]));
        if(statusType[index] == StatusTypeOffline)
            showStatus(IT_Offline, true);
        else if(statusType[index] == StatusTypeAway)
            showStatus(IT_Away, true);
        else if(statusType[index] == StatusTypeBusy)
            showStatus(IT_Busy, true);
    }

    pSettings = new wavrSettings();
    showSmiley = pSettings->value(IDS_EMOTICON, IDS_EMOTICON_VAL).toBool();
    pMessageLog->showSmiley = showSmiley;
    pMessageLog->autoFile = pSettings->value(IDS_AUTOFILE, IDS_AUTOFILE_VAL).toBool();
    pMessageLog->messageTime = pSettings->value(IDS_MESSAGETIME, IDS_MESSAGETIME_VAL).toBool();
    pMessageLog->messageDate = pSettings->value(IDS_MESSAGEDATE, IDS_MESSAGEDATE_VAL).toBool();
    pMessageLog->allowLinks = pSettings->value(IDS_ALLOWLINKS, IDS_ALLOWLINKS_VAL).toBool();
    pMessageLog->pathToLink = pSettings->value(IDS_PATHTOLINK, IDS_PATHTOLINK_VAL).toBool();
    pMessageLog->trimMessage = pSettings->value(IDS_TRIMMESSAGE, IDS_TRIMMESSAGE_VAL).toBool();
    clearOnClose = pSettings->value(IDS_CLEARONCLOSE, IDS_CLEARONCLOSE_VAL).toBool();

    setUIText();

    ui.txtMessage->setFocus();

    QString themePath = pSettings->value(IDS_THEME, IDS_THEME_VAL).toString();
    pMessageLog->initMessageLog(themePath);
    if(!clearOnClose)
        pMessageLog->restoreMessageLog(QDir(StdLocation::cacheDir()).absoluteFilePath("msg_" + peerId + ".tmp"));
}

void wavrChatWindow::stop(void) {
    bool saveHistory = pSettings->value(IDS_HISTORY, IDS_HISTORY_VAL).toBool();
    if(pMessageLog->hasData && saveHistory && !dataSaved) {
        if(clearOnClose) {
//            QString szMessageLog = pMessageLog->prepareMessageLogForSave();
//            if(!groupMode)
//                History::save(peerNames.value(peerId), QDateTime::currentDateTime(), &szMessageLog);
//            else
//                History::save(tr("Group Conversation"), QDateTime::currentDateTime(), &szMessageLog);
//            dataSaved = true;
        } else {
            pMessageLog->saveMessageLog(QDir(StdLocation::cacheDir()).absoluteFilePath("msg_" + peerId + ".tmp"));
        }
    }
}

void wavrChatWindow::receiveMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage) {
    QString title;
    int statusIndex;

    //	if lpszUserId is NULL, the message was sent locally
    QString senderId = lpszUserId ? *lpszUserId : localId;
    QString senderName = lpszUserId ? peerNames.value(senderId) : localName;
    QString data;

    switch(type) {
    case MT_Message:
        appendMessageLog(type, lpszUserId, &senderName, pMessage);
        if(isHidden() || !isActiveWindow()) {
        //	pSoundPlayer->play(SE_NewMessage);
            title = tr("%1 says...");
            setWindowTitle(title.arg(senderName));
        }
        break;
    case MT_Broadcast:
        appendMessageLog(type, lpszUserId, &senderName, pMessage);
        if(isHidden() || !isActiveWindow()) {
            //pSoundPlayer->play(SE_NewMessage);
            title = tr("Broadcast from %1");
            setWindowTitle(title.arg(senderName));
        }
        break;
    case MT_ChatState:
        qDebug() << "Chat state message received";
        appendMessageLog(type, lpszUserId, &senderName, pMessage);
        break;
    case MT_Status:
        data = pMessage->data(XML_STATUS);
        statusIndex = wavrHelper::statusIndexFromCode(data);
        if(statusIndex != -1) {
            setWindowIcon(QIcon(statusPic[statusIndex]));
            statusType[statusIndex] == StatusTypeOffline ? showStatus(IT_Offline, true) : showStatus(IT_Offline, false);
            statusType[statusIndex] == StatusTypeBusy ? showStatus(IT_Busy, true) : showStatus(IT_Busy, false);
            statusType[statusIndex] == StatusTypeAway ? showStatus(IT_Away, true) : showStatus(IT_Away, false);
            peerStatuses.insert(senderId, data);
        }
        break;
    case MT_Avatar:
        data = pMessage->data(XML_FILEPATH);
        // this message may come with or without user id. NULL user id means avatar change
        // by local user, while non NULL user id means avatar change by a peer.
        pMessageLog->updateAvatar(&senderId, &data);
        break;
    case MT_UserName:
        data = pMessage->data(XML_NAME);
        if(peerNames.contains(senderId)) {
            peerNames.insert(senderId, data);
            pMessageLog->peerName = data;
        }
        pMessageLog->updateUserName(&senderId, &data);
        break;
    case MT_Failed:
        appendMessageLog(type, lpszUserId, &senderName, pMessage);
        break;
    case MT_File:
    case MT_Folder:
        if(pMessage->data(XML_FILEOP) == FileOpNames[FO_Request]) {
            //	a file request has been received
            appendMessageLog(type, lpszUserId, &senderName, pMessage);
            if(pMessage->data(XML_MODE) == FileModeNames[FM_Receive] && (isHidden() || !isActiveWindow())) {
                //pSoundPlayer->play(SE_NewFile);
                if(type == MT_File)
                    title = tr("%1 sends a file...");
                else
                    title = tr("%1 sends a folder...");
                setWindowTitle(title.arg(senderName));
            }
        } else {
            // a file message of op other than request has been received
            processFileOp(pMessage);
        }
        break;
    case MT_Depart:
        pMessageLog->abortPendingFileOperations();
        break;
    default:
        break;
    }
}

void wavrChatWindow::connectionStateChanged(bool connected) {
    connected ? showStatus(IT_Disconnected, false) : showStatus(IT_Disconnected, true);
}

void wavrChatWindow::settingsChanged(void) {
    showSmiley = pSettings->value(IDS_EMOTICON, IDS_EMOTICON_VAL).toBool();
    pMessageLog->showSmiley = showSmiley;
    //pMessageLog->fontSizeVal = pSettings->value(IDS_FONTSIZE, IDS_FONTSIZE_VAL).toInt();
    pMessageLog->autoFile = pSettings->value(IDS_AUTOFILE, IDS_AUTOFILE_VAL).toBool();
    //sendKeyMod = pSettings->value(IDS_SENDKEYMOD, IDS_SENDKEYMOD_VAL).toBool();
    clearOnClose = pSettings->value(IDS_CLEARONCLOSE, IDS_CLEARONCLOSE_VAL).toBool();
    //pSoundPlayer->settingsChanged();
    if(localName.compare(pLocalUser->name) != 0) {
        localName = pLocalUser->name;
        pMessageLog->updateUserName(&localId, &localName);
    }

    bool msgTime = pSettings->value(IDS_MESSAGETIME, IDS_MESSAGETIME_VAL).toBool();
    bool msgDate = pSettings->value(IDS_MESSAGEDATE, IDS_MESSAGEDATE_VAL).toBool();
    bool allowLinks = pSettings->value(IDS_ALLOWLINKS, IDS_ALLOWLINKS_VAL).toBool();
    bool pathToLink = pSettings->value(IDS_PATHTOLINK, IDS_PATHTOLINK_VAL).toBool();
    bool trim = pSettings->value(IDS_TRIMMESSAGE, IDS_TRIMMESSAGE_VAL).toBool();
    QString theme = pSettings->value(IDS_THEME, IDS_THEME_VAL).toString();
    if(msgTime != pMessageLog->messageTime || msgDate != pMessageLog->messageDate ||
            allowLinks != pMessageLog->allowLinks || pathToLink != pMessageLog->pathToLink ||
            trim != pMessageLog->trimMessage || theme.compare(pMessageLog->themePath) != 0) {
        pMessageLog->messageTime = msgTime;
        pMessageLog->messageDate = msgDate;
        pMessageLog->allowLinks = allowLinks;
        pMessageLog->pathToLink = pathToLink;
        pMessageLog->trimMessage = trim;
        pMessageLog->themePath = theme;
        pMessageLog->reloadMessageLog();
    }
}

bool wavrChatWindow::eventFilter(QObject* pObject, QEvent* pEvent) {
    if(pEvent->type() == QEvent::KeyPress) {
        QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(pEvent);
        if(pObject == ui.txtMessage) {
            if(pKeyEvent->key() == Qt::Key_Return || pKeyEvent->key() == Qt::Key_Enter) {
                bool keyMod = ((pKeyEvent->modifiers() & Qt::ControlModifier) == Qt::ControlModifier);
                if(keyMod == sendKeyMod) {
                    sendMessage();
                    setChatState(CS_Active);
                    return true;
                }
                // The TextEdit widget does not insert new line when Ctrl+Enter is pressed
                // So we insert a new line manually
                if(keyMod)
                    ui.txtMessage->insertPlainText("\n");
            } else if(pKeyEvent->key() == Qt::Key_Escape) {
                close();
                return true;
            }
            keyStroke++;
            setChatState(CS_Composing);
        } else {
            if(pKeyEvent->key() == Qt::Key_Escape) {
                close();
                return true;
            }
        }
    }

    return false;
}

void wavrChatWindow::changeEvent(QEvent* pEvent) {
    switch(pEvent->type()) {
    case QEvent::ActivationChange:
        if(isActiveWindow())
            setWindowTitle(getWindowTitle());
        break;
    case QEvent::LanguageChange:
        setUIText();
        break;
    default:
        break;
    }

    QWidget::changeEvent(pEvent);
}

void wavrChatWindow::closeEvent(QCloseEvent* pEvent) {
    setChatState(CS_Inactive);
    // Call stop() to save history
    stop();
    emit closed(&peerId);

    QWidget::closeEvent(pEvent);
}

void wavrChatWindow::dragEnterEvent(QDragEnterEvent* pEvent) {
    if(pEvent->mimeData()->hasFormat("text/uri-list")) {
        //  Check if the remote user has file transfer capability
        if((peerCaps.value(peerId) & UC_File) != UC_File)
            return;

        QList<QUrl> urls = pEvent->mimeData()->urls();
        if(urls.isEmpty())
            return;

        QString fileName = urls.first().toLocalFile();
        if(fileName.isEmpty())
            return;

        pEvent->acceptProposedAction();
    }
}

void wavrChatWindow::dropEvent(QDropEvent* pEvent) {
    QList<QUrl> urls = pEvent->mimeData()->urls();
    if(urls.isEmpty())
        return;

    foreach(QUrl url, urls) {
        QString path = url.toLocalFile();
        if(path.isEmpty())
            continue;

        QFileInfo fileInfo(path);
        if(!fileInfo.exists())
            continue;

        if(fileInfo.isFile())
            sendFile(&path);
        else if(fileInfo.isDir())
            sendFolder(&path);
    }
}

void wavrChatWindow::btnFile_clicked(void) {
    QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
    QString fileName = QFileDialog::getOpenFileName(this, QString(), dir);
    qDebug() << fileName;
    if(!fileName.isEmpty()) {
        pSettings->setValue(IDS_OPENPATH, QFileInfo(fileName).dir().absolutePath());
        sendFile(&fileName);
    }
}

void wavrChatWindow::btnFolder_clicked(void) {
    QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
    QString path = QFileDialog::getExistingDirectory(this, QString(), dir, QFileDialog::ShowDirsOnly);
    if(!path.isEmpty()) {
        pSettings->setValue(IDS_OPENPATH, QFileInfo(path).absolutePath());
        //sendFolder(&path);
    }
}

void wavrChatWindow::smileyAction_triggered(void) {
    //	nSmiley contains index of smiley
    if(showSmiley) {
        QString htmlPic("<html><head></head><body><img src='" + smileyPic[nSmiley] + "' /></body></html>");
        ui.txtMessage->insertHtml(htmlPic);
    }
    else
        ui.txtMessage->insertPlainText(smileyCode[nSmiley]);
}

void wavrChatWindow::log_sendMessage(MessageType type, QString *lpszUserId, wavrXmlMessage *pMessage) {
    emit messageSent(type, lpszUserId, pMessage);
}

void wavrChatWindow::checkChatState(void) {
    if(keyStroke > snapKeyStroke) {
        snapKeyStroke = keyStroke;
        QTimer::singleShot(pauseTime, this, SLOT(checkChatState()));
        return;
    }

    if(ui.txtMessage->document()->isEmpty())
        setChatState(CS_Active);
    else
        setChatState(CS_Paused);
}

void wavrChatWindow::createSmileyMenu(void) {
    pSmileyAction = new wavrImagePickerAction(this, smileyPic, SM_COUNT, 19, 10, &nSmiley);
    connect(pSmileyAction, SIGNAL(triggered()), this, SLOT(smileyAction_triggered()));

    pSmileyMenu = new QMenu(this);
    pSmileyMenu->addAction(pSmileyAction);
}

void wavrChatWindow::createToolBar(void) {
    QToolBar* pLeftBar = new QToolBar(ui.wgtToolBar);
    pLeftBar->setStyleSheet("QToolBar { border: 0px }");
    pLeftBar->setIconSize(QSize(16, 16));
    ui.toolBarLayout->addWidget(pLeftBar);

    pLeftBar->addSeparator();

    pbtnSmiley = new wavrToolButton(pLeftBar);
    pbtnSmiley->setIcon(QIcon(QPixmap(IDR_SMILEY, "PNG")));
    pbtnSmiley->setPopupMode(QToolButton::InstantPopup);
    pbtnSmiley->setMenu(pSmileyMenu);
    pLeftBar->addWidget(pbtnSmiley);

    pLeftBar->addSeparator();

    pFileAction = pLeftBar->addAction(QIcon(QPixmap(IDR_FILE, "PNG")), "Send A &File...", this, SLOT(btnFile_clicked()));
    pFileAction->setShortcut(QKeySequence::Open);
    bool fileCap = ((peerCaps.value(peerId) & UC_File) == UC_File);
    pFileAction->setEnabled(fileCap);
    pFolderAction = pLeftBar->addAction(QIcon(QPixmap(IDR_SENDFOLDER, "PNG")), "Send A Fol&der...", this, SLOT(btnFolder_clicked()));
    bool folderCap = ((peerCaps.value(peerId) & UC_Folder) == UC_Folder);
    pFolderAction->setEnabled(folderCap);

    pLeftBar->addSeparator();

//	pSaveAction = pLeftBar->addAction(QIcon(QPixmap(IDR_SAVE, "PNG")), "&Save As...", this, SLOT(btnSave_clicked()));
//	pSaveAction->setShortcut(QKeySequence::Save);
//	pSaveAction->setEnabled(false);

//	pRightBar = new QToolBar(ui.wgtToolBar);
//	pRightBar->setStyleSheet("QToolBar { border: 0px }");
//	pRightBar->setIconSize(QSize(16, 16));
//	pRightBar->setLayoutDirection(Qt::RightToLeft);
//	ui.toolBarLayout->addWidget(pRightBar);

//	pHistoryAction = pRightBar->addAction(QIcon(QPixmap(IDR_HISTORY, "PNG")), "&History", this, SLOT(btnHistory_clicked()));
//	pHistoryAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
//	pTransferAction = pRightBar->addAction(QIcon(QPixmap(IDR_TRANSFER, "PNG")), "File &Transfers", this, SLOT(btnTransfers_clicked()));
//	pTransferAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));

    ui.lblDividerTop->setBackgroundRole(QPalette::Light);
    ui.lblDividerTop->setAutoFillBackground(true);
    ui.lblDividerBottom->setBackgroundRole(QPalette::Dark);
    ui.lblDividerBottom->setAutoFillBackground(true);
}

void wavrChatWindow::setUIText(void) {
    ui.retranslateUi(this);

    setWindowTitle(getWindowTitle());
   // pRightBar->setLayoutDirection((QApplication::layoutDirection() == Qt::LeftToRight ? Qt::RightToLeft : Qt::LeftToRight));

    pbtnSmiley->setToolTip(tr("Insert Smiley"));
    pFileAction->setText(tr("Send A &File..."));
    pFolderAction->setText(tr("Send A Fol&der..."));
    //pSaveAction->setText(tr("&Save As..."));
    //pHistoryAction->setText(tr("&History"));
    //pTransferAction->setText(tr("File &Transfers"));
    if(!groupMode) {
        QString toolTip = tr("Send a file to %1");
        pFileAction->setToolTip(toolTip.arg(peerNames.value(peerId)));
        toolTip = tr("Send a folder to %1");
        pFolderAction->setToolTip(toolTip.arg(peerNames.value(peerId)));
    }
    //pSaveAction->setToolTip(tr("Save this conversation"));
    //pHistoryAction->setToolTip(tr("View History"));
    //pTransferAction->setToolTip(tr("View File Transfers"));
    //pFontAction->setText(tr("Change Font..."));
    //pFontAction->setToolTip(tr("Change message font"));
    //pFontColorAction->setText(tr("Change Color..."));
    //pFontColorAction->setToolTip(tr("Change message text color"));

    showStatus(IT_Ok, true);	//	this will force the info label to retranslate
}

void wavrChatWindow::sendMessage(void) {
    if(ui.txtMessage->document()->isEmpty())
        return;

    if(bConnected) {
        QString szHtmlMessage(ui.txtMessage->toHtml());
        encodeMessage(&szHtmlMessage);
        QTextDocument docMessage;
        docMessage.setHtml(szHtmlMessage);
        QString szMessage = docMessage.toPlainText();

        //QFont font = ui.txtMessage->font();
        //font.setPointSize(ui.txtMessage->fontPointSize());

        MessageType type = groupMode ? MT_GroupMessage : MT_Message;
        wavrXmlMessage xmlMessage;
        xmlMessage.addHeader(XML_TIME, QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
        //xmlMessage.addData(XML_FONT, font.toString());
        //xmlMessage.addData(XML_COLOR, messageColor.name());
        if(groupMode) {
            //xmlMessage.addData(XML_THREAD, threadId);
           // xmlMessage.addData(XML_GROUPMESSAGE, szMessage);
        } else
            xmlMessage.addData(XML_MESSAGE, szMessage);

        appendMessageLog(MT_Message, &localId, &localName, &xmlMessage);

        QHash<QString, QString>::const_iterator index = peerIds.constBegin();
        while (index != peerIds.constEnd()) {
            QString userId = index.value();
            qDebug() << "message sent to " << userId;
            emit messageSent(type, &userId, &xmlMessage);
            index++;
        }
    }
    else
        appendMessageLog(MT_Error, NULL, NULL, NULL);

    ui.txtMessage->clear();
    ui.txtMessage->setFocus();
}

void wavrChatWindow::sendFile(QString* lpszFilePath) {
    sendObject(MT_File, lpszFilePath);
}

void wavrChatWindow::sendFolder(QString* lpszFolderPath) {
    sendObject(MT_Folder, lpszFolderPath);
}

void wavrChatWindow::sendObject(MessageType type, QString* lpszPath) {
    if(bConnected) {
        wavrXmlMessage xmlMessage;
        xmlMessage.addData(XML_FILETYPE, FileTypeNames[FT_Normal]);
        xmlMessage.addData(XML_FILEOP, FileOpNames[FO_Request]);
        xmlMessage.addData(XML_FILEPATH, *lpszPath);
        qDebug() << "file delivering process";
        QString userId = peerIds.value(peerId);
        emit messageSent(type, &userId, &xmlMessage);
    } else
        appendMessageLog(MT_Error, NULL, NULL, NULL);
}

//	Called before sending message
void wavrChatWindow::encodeMessage(QString* lpszMessage) {
    //	replace all emoticon images with corresponding text code
    ChatHelper::encodeSmileys(lpszMessage);
}

void wavrChatWindow::processFileOp(wavrXmlMessage *pMessage) {
    int fileOp = wavrHelper::indexOf(FileOpNames, FO_Max, pMessage->data(XML_FILEOP));
    int fileMode = wavrHelper::indexOf(FileModeNames, FM_Max, pMessage->data(XML_MODE));
    QString fileId = pMessage->data(XML_FILEID);

    switch(fileOp) {
    case FO_Cancel:
    case FO_Accept:
    case FO_Decline:
    case FO_Error:
    case FO_Abort:
    case FO_Complete:
        updateFileMessage((FileMode)fileMode, (FileOp)fileOp, fileId);
        break;
    default:
        break;
    }
}

void wavrChatWindow::appendMessageLog(MessageType type, QString* lpszUserId, QString* lpszUserName, wavrXmlMessage* pMessage) {
    qDebug() << "message received " << *lpszUserName;
    pMessageLog->appendMessageLog(type, lpszUserId, lpszUserName, pMessage);
//    if(!pSaveAction->isEnabled())
//        pSaveAction->setEnabled(pMessageLog->hasData);
}

void wavrChatWindow::updateFileMessage(FileMode mode, FileOp op, QString fileId) {
    pMessageLog->updateFileMessage(mode, op, fileId);
}

void wavrChatWindow::showStatus(int flag, bool add) {
    infoFlag = add ? infoFlag | flag : infoFlag & ~flag;

    int relScrollPos = pMessageLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) -
            pMessageLog->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    //ui.lblInfo->setStyleSheet("QLabel { background-color:white; }");
    if(infoFlag & IT_Disconnected) {
        ui.lblInfo->setText("<span style='color:rgb(96,96,96);'>" + tr("You are no longer connected.") + "</span>");
        ui.lblInfo->setVisible(true);
    } else if(!groupMode && (infoFlag & IT_Offline))  {
        QString msg = tr("%1 is offline.");
        ui.lblInfo->setText("<span style='color:rgb(96,96,96);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
        ui.lblInfo->setVisible(true);
    } else if(!groupMode && (infoFlag & IT_Away)) {
        QString msg = tr("%1 is away.");
        ui.lblInfo->setText("<span style='color:rgb(255,115,0);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
        ui.lblInfo->setVisible(true);
    } else if(!groupMode && (infoFlag & IT_Busy)) {
        QString msg = tr("%1 is busy. You may be interrupting.");
        ui.lblInfo->setText("<span style='color:rgb(192,0,0);'>" + msg.arg(peerNames.value(peerId)) + "</span>");
        ui.lblInfo->setVisible(true);
    } else {
        ui.lblInfo->setText(QString::null);
        ui.lblInfo->setVisible(false);
    }

    int scrollPos = pMessageLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) - relScrollPos;
    pMessageLog->page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollPos);
}

QString wavrChatWindow::getWindowTitle(void) {
    QString title = QString::null;

    QHash<QString, QString>::const_iterator index = peerNames.constBegin();
    while (index != peerNames.constEnd()) {
        title.append(index.value() + ", ");
        index++;
    }

    //	remove the final comma and space
    title.remove(title.length() - 2, 2);
    title.append(" - ");
    title.append(tr("Conversation"));
    return title;
}

void wavrChatWindow::setChatState(ChatState newChatState) {
    if(chatState == newChatState)
        return;

    bool bNotify = false;

    switch(newChatState) {
    case CS_Active:
    case CS_Inactive:
        chatState = newChatState;
        bNotify = true;
        break;
    case CS_Composing:
        chatState = newChatState;
        bNotify = true;
        snapKeyStroke = keyStroke;
        QTimer::singleShot(pauseTime, this, SLOT(checkChatState()));
        break;
    case CS_Paused:
        if(chatState != CS_Inactive) {
            chatState = newChatState;
            bNotify = true;
        }
        break;
    default:
        break;
    }

    // send a chat state message
    if(bNotify && bConnected) {
        wavrXmlMessage xmlMessage;
        //if(groupMode)
            //xmlMessage.addData(XML_THREAD, threadId);
        xmlMessage.addData(XML_CHATSTATE, ChatStateNames[chatState]);

        QHash<QString, QString>::const_iterator index = peerIds.constBegin();
        while (index != peerIds.constEnd()) {
            QString userId = index.value();
            emit messageSent(MT_ChatState, &userId, &xmlMessage);
            index++;
        }
    }
}
