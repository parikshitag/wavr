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


#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>
#include "libWavr/shared.h"
#include "chatdefinitions.h"
#include "chathelper.h"
#include "libWavr/xmlmessage.h"
#include "theme.h"

enum OutputFormat{ HtmlFormat, TextFormat };

/**
 * @brief Shows the chat log ie chat happened so far in GUI windows.
 *  Message log stores the message, broadcast, file and links log in the window that happened so far.
 *  Messages and broadcast are logged with different font styles, links store the url of the link and are triggerable.
 */
class wavrMessageLog : public QWebView
{
    Q_OBJECT

public:
    wavrMessageLog(QWidget *parent = 0);
    ~wavrMessageLog(void);

    void initMessageLog(QString themePath, bool clearLog = true);
    void appendMessageLog(MessageType type, QString* lpszUserId, QString* lpszUserName, wavrXmlMessage* pMessage, bool bReload = false);
    void updateFileMessage(FileMode mode, FileOp op, QString fileId);
    void updateUserName(QString* lpszUserId, QString* lpszUserName);
    void updateAvatar(QString* lpszUserId, QString* lpszFilePath);
    void reloadMessageLog(void);
    QString prepareMessageLogForSave(OutputFormat format = HtmlFormat);
    void setAutoScroll(bool enable);
    void abortPendingFileOperations(void);
    void saveMessageLog(QString filePath);
    void restoreMessageLog(QString filePath, bool reload = true);

    QString localId;
    QString peerId;
    QString peerName;
    QHash<QString, QString> participantAvatars;
    QString lastId;
    bool hasData;
    bool showSmiley;
    bool autoFile;
    int fontSizeVal;
    bool messageTime;
    bool messageDate;
    QString themePath;
    bool allowLinks;
    bool pathToLink;
    bool trimMessage;

signals:
    void messageSent(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);

protected:
    //void changeEvent(QEvent* event);

private slots:
    void log_linkClicked(QUrl url);
    void log_contentsSizeChanged(QSize size);
    void log_linkHovered(const QString& link, const QString& title, const QString& textContent);
    void showContextMenu(const QPoint& pos);
    void copyAction_triggered(void);
    void copyLinkAction_triggered(void);
    void selectAllAction_triggered(void);

private:
    void createContextMenu(void);
    void appendMessageLog(QString* lpszHtml);
    void removeMessageLog(QString divClass);
    void appendBroadcast(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime);
    void appendMessage(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime,
                       QFont* pFont, QColor* pColor);
    void appendFileMessage(MessageType type, QString* lpszUserName, wavrXmlMessage* pMessage, bool bReload = false);
    QString getFontStyle(QFont* pFont, QColor* pColor, bool size = false);
    QString getFileStatusMessage(FileMode mode, FileOp op);
    QString getChatStateMessage(ChatState chatState);
    void fileOperation(QString fileId, QString action, QString fileType, FileMode mode = FM_Receive);
    void decodeMessage(QString* lpszMessage, bool useDefaults = false);
    void processMessageText(QString* lpszMessageText, bool useDefaults);
    QString getTimeString(QDateTime* pTime);
    void setUIText(void);

    QMap<QString, wavrXmlMessage> sendFileMap;
    QMap<QString, wavrXmlMessage> receiveFileMap;
    QList<SingleMessage> messageLog;
    ThemeData themeData;
    QMenu* contextMenu;
    QAction* copyAction;
    QAction* copyLinkAction;
    QAction* selectAllAction;
    bool linkHovered;
    bool outStyle;
    bool autoScroll;
};

#endif // MESSAGELOG_H
