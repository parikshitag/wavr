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


#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QDateTime>
#include <QDir>
#include "messagelog.h"

wavrMessageLog::wavrMessageLog(QWidget *parent) : QWebView(parent) {
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(log_LinkClicked(QUrl)));
    connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
            this, SLOT(log_contentsSizeChanged(QSize)));
    connect(this->page(), SIGNAL(linkHovered(QString, QString, QString)),
            this, SLOT(log_linkHovered(QString, QString, QString)));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    settings()->setObjectCacheCapacities(0, 0, 0);
    createContextMenu();

    participantAvatars.clear();
    hasData = false;
    messageTime = false;
    messageDate = false;
    allowLinks = false;
    pathToLink = false;
    trimMessage = true;
    fontSizeVal = 0;
    lastId = QString::null;
    messageLog.clear();
    linkHovered = false;
    outStyle = false;
    autoScroll = true;
}

wavrMessageLog::~wavrMessageLog() {
}

void wavrMessageLog::initMessageLog(QString themePath, bool clearLog) {
    if (clearLog)
        messageLog.clear();
    lastId = QString::null;
    this->themePath = themePath;
    themeData = wavrTheme::loadTheme(themePath);
    setHtml(themeData.document);
}

void wavrMessageLog::createContextMenu(void) {
    QAction* action = pageAction(QWebPage::Copy);
    action->setShortcut(QKeySequence::Copy);
    addAction(action);
    action = pageAction(QWebPage::CopyLinkToClipboard);
    addAction(action);
    action = pageAction(QWebPage::SelectAll);
    action->setShortcut(QKeySequence::SelectAll);
    addAction(action);

    contextMenu = new QMenu(this);
    copyAction = contextMenu->addAction("&Copy", this, SLOT(copyAction_triggered()), QKeySequence::Copy);
    copyLinkAction = contextMenu->addAction("&Copy Link", this, SLOT(copyLinkAction_triggered()));
    contextMenu->addSeparator();
    selectAllAction = contextMenu->addAction("Select &All", this,
                                             SLOT(selectAllAction_triggered()), QKeySequence::SelectAll);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void wavrMessageLog::appendMessageLog(MessageType type, QString *lpszUserId, QString *lpszUserName, wavrXmlMessage *pMessage,
                                      bool bReload) {
    if(!pMessage && type != MT_Error)
        return;

    QString message;
    QString html;
    QString caption;
    QDateTime time;
    QFont font;
    QColor color;
    QString fontStyle;
    QString id = QString::null;
    bool addToLog = true;

    removeMessageLog("_wavr_statediv");

    switch(type) {
    case MT_Message:
        time.setMSecsSinceEpoch(pMessage->header(XML_TIME).toLongLong());
        message = pMessage->data(XML_MESSAGE);
        font.fromString(pMessage->data(XML_FONT));
        color.setNamedColor(pMessage->data(XML_COLOR));
        qDebug() << "message received is " << message;
        appendMessage(lpszUserId, lpszUserName, &message, &time, &font, &color);
        lastId = *lpszUserId;
        break;
    case MT_Broadcast:
        time.setMSecsSinceEpoch(pMessage->header(XML_TIME).toLongLong());
        message = pMessage->data(XML_BROADCAST);
        appendBroadcast(lpszUserId, lpszUserName, &message, &time);
        lastId = QString::null;
        break;
    case MT_ChatState:
        message = pMessage->data(XML_CHATSTATE);
        caption = getChatStateMessage((ChatState)wavrHelper::indexOf(ChatStateNames, CS_Max, message));
        if(!caption.isNull()) {
            html = themeData.stateMsg;
            html.replace("%iconpath%", "qrc"IDR_BLANK);
            html.replace("%sender%", caption.arg(*lpszUserName));
            html.replace("%message%", "");
            appendMessageLog(&html);
        }
        addToLog = false;
        break;
    case MT_Failed:
        message = pMessage->data(XML_MESSAGE);
        font.fromString(pMessage->data(XML_FONT));
        color.setNamedColor(pMessage->data(XML_COLOR));
        html = themeData.sysMsg;
        caption = tr("Message not delivered to %1:");
        fontStyle = getFontStyle(&font, &color, true);
        decodeMessage(&message);
        html.replace("%iconpath%", "qrc"IDR_CRITICALMSG);
        html.replace("%sender%", caption.arg(*lpszUserName));
        html.replace("%style%", fontStyle);
        html.replace("%message%", message);
        appendMessageLog(&html);
        lastId  = QString::null;
        break;
    case MT_Error:
        //html = themeData.sysMsg;
        html.replace("%iconpath%", "qrc"IDR_CRITICALMSG);
        html.replace("%sender%", tr("Your message was not sent."));
        html.replace("%message%", "");
        appendMessageLog(&html);
        lastId  = QString::null;
        addToLog = false;
        break;
    default:
        break;
    }

    if(!bReload && addToLog && pMessage) {
        wavrXmlMessage xmlMessage = pMessage->clone();
        QString userId = lpszUserId ? *lpszUserId : QString::null;
        QString userName = lpszUserName ? *lpszUserName : QString::null;
        messageLog.append(SingleMessage(type, userId, userName, xmlMessage, id));
    }
}

void wavrMessageLog::updateUserName(QString *lpszUserId, QString *lpszUserName) {
    // update the entries in message log
    for (int index = 0; index < messageLog.count(); index++) {
        SingleMessage msg = messageLog.takeAt(index);
        if(lpszUserId->compare(msg.userId) == 0)
            msg.userName = *lpszUserName;
        messageLog.insert(index, msg);
    }
    reloadMessageLog();
}

void wavrMessageLog::updateAvatar(QString *lpszUserId, QString *lpszFilePath) {
    participantAvatars.insert(*lpszUserId, *lpszFilePath);

    reloadMessageLog();
}

void wavrMessageLog::reloadMessageLog(void) {
    initMessageLog(false);
    for (int index = 0; index < messageLog.count(); index++) {
        SingleMessage msg = messageLog[index];
        appendMessageLog(msg.type, &msg.userId, &msg.userName, &msg.message, true);
    }
}

QString wavrMessageLog::prepareMessageLogForSave(OutputFormat format) {
    QDateTime time;

    if(format == HtmlFormat) {
        QString html =
            "<html><head><style type='text/css'>"\
            "*{font-size: 9pt;} body {-webkit-nbsp-mode: space; word-wrap: break-word;}"\
            "span.salutation {float:left; font-weight: bold;} span.time {float: right;}"\
            "span.message {clear: both; display: block;} p {border-bottom: 1px solid #CCC;}"\
            "</style></head><body>";

        for(int index = 0; index < messageLog.count(); index++) {
            SingleMessage msg = messageLog.at(index);
            if(msg.type == MT_Message || msg.type == MT_GroupMessage) {
                time.setMSecsSinceEpoch(msg.message.header(XML_TIME).toLongLong());
                QString messageText = msg.message.data(XML_MESSAGE);
                decodeMessage(&messageText, true);
                QString htmlMsg =
                    "<p><span class='salutation'>" + msg.userName + ":</span>"\
                    "<span class='time'>" + time.time().toString(Qt::SystemLocaleShortDate) + "</span>"\
                    "<span class='message'>" + messageText + "</span></p>";
                html.append(htmlMsg);
            }
        }

        html.append("</body></html>");
        return html;
    } else {
        QString text;
        for(int index = 0; index < messageLog.count(); index++) {
            SingleMessage msg = messageLog.at(index);
            if(msg.type == MT_Message || msg.type == MT_GroupMessage) {
                time.setMSecsSinceEpoch(msg.message.header(XML_TIME).toLongLong());
                QString textMsg =
                    msg.userName + " [" + time.time().toString(Qt::SystemLocaleShortDate) + "]:\n" +
                    msg.message.data(XML_MESSAGE) + "\n\n";
                text.append(textMsg);
            }
        }

        return text;
    }
}


void wavrMessageLog::setAutoScroll(bool enable) {
    autoScroll = enable;
}

void wavrMessageLog::saveMessageLog(QString filePath) {
    if(messageLog.isEmpty())
        return;

    QDir dir = QFileInfo(filePath).dir();
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly))
        return;

    QDataStream stream(&file);
    stream << peerId << peerName << messageLog;

    file.close();
}

void wavrMessageLog::restoreMessageLog(QString filePath, bool reload) {
    messageLog.clear();

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QDataStream stream(&file);
    stream >> peerId >> peerName >> messageLog;

    file.close();

    if(reload)
        reloadMessageLog();
}

void wavrMessageLog::log_LinkClicked(QUrl url) {
    QString linkPath = url.toString();

    // this is a hack so qdesktopservices can open a network path
    if (linkPath.startsWith("file")) {
        // strip out the 'file:' prefix and get the path
        linkPath = linkPath.mid(5);
        // use a url that contains native separators
        QDesktopServices::openUrl(QUrl(QDir::toNativeSeparators(linkPath)));
        return;
    } else if(linkPath.startsWith("www")) {
        //prepend 'http://' to link
        linkPath.prepend("http://");
        QDesktopServices::openUrl(QUrl(linkPath));
        return;
    }else if (!linkPath.startsWith("wavr")) {
        QDesktopServices::openUrl(url);
        return;
    }

}

void wavrMessageLog::log_contentsSizeChanged(QSize size) {
    if(autoScroll) {
        QWebFrame* frame = page()->mainFrame();
        frame->scroll(0, size.height());
    }
}

void wavrMessageLog::log_linkHovered(const QString &link, const QString &title, const QString &textContent) {
    Q_UNUSED(title);
    Q_UNUSED(textContent);
    linkHovered = !link.isEmpty();
}

void wavrMessageLog::showContextMenu(const QPoint &pos) {
    copyAction->setEnabled(!selectedText().isEmpty());
    copyLinkAction->setEnabled(linkHovered);
    // Copy Link is currently hidden since it perfoms the same action as regual Copy
    // copyLinkAction->setVisible(false);
    selectAllAction->setEnabled(!page()->mainFrame()->documentElement().findFirst("body").firstChild().isNull());
    contextMenu->exec(mapToGlobal(pos));
}

void wavrMessageLog::copyAction_triggered(void) {
    pageAction(QWebPage::Copy)->trigger();
}

void wavrMessageLog::copyLinkAction_triggered(void) {
    pageAction(QWebPage::CopyLinkToClipboard)->trigger();
}

void wavrMessageLog::selectAllAction_triggered(void) {
    pageAction(QWebPage::SelectAll)->trigger();
}

void wavrMessageLog::appendMessageLog(QString *lpszHtml) {
    QWebFrame* frame = page()->mainFrame();
    QWebElement document = frame->documentElement();
    QWebElement body = document.findFirst("body");
    body.appendInside(*lpszHtml);
}

void wavrMessageLog::removeMessageLog(QString divClass) {
    QWebFrame* frame = page()->mainFrame();
    QWebElement document = frame->documentElement();
    QWebElement body = document.findFirst("body");
    QWebElement element = body.findFirst("div." + divClass);
    element.removeFromDocument();
}

void wavrMessageLog::appendBroadcast(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime) {
    Q_UNUSED(lpszUserId);

    decodeMessage(lpszMessage);

    QString html = themeData.pubMsg;
    QString caption = tr("Broadcast message from %1:");
    html.replace("%iconpath%", "qrc"IDR_BROADCASTMSG);
    html.replace("%sender%", caption.arg(*lpszUserName));
    html.replace("%time%", getTimeString(pTime));
    html.replace("%style%", "");
    html.replace("%message%", *lpszMessage);

    appendMessageLog(&html);
}

void wavrMessageLog::appendMessage(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime,
                                  QFont* pFont, QColor* pColor) {
    QString html = QString::null;
    bool localUser = (lpszUserId->compare(localId) == 0);

    decodeMessage(lpszMessage);

    QString fontStyle = getFontStyle(pFont, pColor, localUser);

    if(lpszUserId->compare(lastId) != 0) {
        html = localUser ? themeData.outMsg : themeData.inMsg;

        //	get the avatar image for this user from the cache folder
        QString filePath = participantAvatars.value(*lpszUserId);
        //	if image not found, use the default avatar image for this user
        QString iconPath = QFile::exists(filePath) ? QUrl::fromLocalFile(filePath).toString() : "qrc"AVT_DEFAULT;

        html.replace("%iconpath%", iconPath);
        html.replace("%sender%", *lpszUserName);
        html.replace("%time%", getTimeString(pTime));
        html.replace("%style%", fontStyle);
        html.replace("%message%", *lpszMessage);

        QWebFrame* frame = page()->mainFrame();
        QWebElement document = frame->documentElement();
        QWebElement body = document.findFirst("body");
        body.appendInside(html);
    } else {
        html = localUser ? themeData.outNextMsg : themeData.inNextMsg;
        html.replace("%time%", getTimeString(pTime));
        html.replace("%style%", fontStyle);
        html.replace("%message%", *lpszMessage);

        QWebFrame* frame = page()->mainFrame();
        QWebElement document = frame->documentElement();
        QWebElement body = document.findFirst("body");
        QWebElement last = body.lastChild();
        QWebElement insert = last.findFirst("div#insert");
        insert.replace(html);
    }

    hasData = true;
}
QString wavrMessageLog::getFontStyle(QFont* pFont, QColor* pColor, bool size) {
    QString style = "font-family:\"" + pFont->family() + "\"; ";
    if(pFont->italic())
        style.append("font-style:italic; ");
    if(pFont->bold())
        style.append("font-weight:bold; ");

    if(size) {
        style.append("font-size:" + QString::number(pFont->pointSize()) + "pt; ");
        style.append("color:" + pColor->name() + "; ");
    }
    else
        //style.append(fontStyle[fontSizeVal] + " ");

    if(pFont->strikeOut())
        style.append("text-decoration:line-through; ");
    if(pFont->underline())
        style.append("text-decoration:underline; ");

    return style;
}

QString wavrMessageLog::getChatStateMessage(ChatState chatState) {
    QString message = QString::null;

    switch(chatState) {
    case CS_Composing:
        message = tr("%1 is typing...");
        break;
    case CS_Paused:
        message = tr("%1 has entered text");
        break;
    default:
        break;
    }

    return message;
}

//	Called when message received, before adding to message log
//	The useDefaults parameter is an override flag that will ignore app settings
//	while decoding the message. If the flag is set, message is not trimmed,
//	smileys are left as text and links will be detected and converted.
void wavrMessageLog::decodeMessage(QString* lpszMessage, bool useDefaults) {
    if(!useDefaults && trimMessage)
        *lpszMessage = lpszMessage->trimmed();

    //	The url detection regexps only work with plain text, so link detection is done before
    //	making the text html safe. The converted links are given a "data-isLink" custom
    //	attribute to differentiate them from the message content
    if(useDefaults || allowLinks) {
//		lpszMessage->replace(QRegExp("(((https|http|ftp|file|smb):[/][/]|www.)[\\w\\d:#@%/;$()~_?\\+-=\\\\\\.&]*)"),
//							 "<a href='\\1'>\\1</a>");
        lpszMessage->replace(QRegExp("((?:(?:https?|ftp|file)://|www\\.|ftp\\.)[-A-Z0-9+&@#/%=~_|$?!:,.]*[A-Z0-9+&@#/%=~_|$])", Qt::CaseInsensitive),
                             "<a data-isLink='true' href='\\1'>\\1</a>");
        lpszMessage->replace("<a data-isLink='true' href='www", "<a data-isLink='true' href='http://www");

        if(!useDefaults && pathToLink)
            lpszMessage->replace(QRegExp("((\\\\\\\\[\\w-]+\\\\[^\\\\/:*?<>|""]+)((?:\\\\[^\\\\/:*?<>|""]+)*\\\\?)$)"),
                                 "<a data-isLink='true' href='file:\\1'>\\1</a>");
    }

    QString message = QString::null;
    int index = 0;

    while(index < lpszMessage->length()) {
        int aStart = lpszMessage->indexOf("<a data-isLink='true'", index);
        if(aStart != -1) {
            QString messageSegment = lpszMessage->mid(index, aStart - index);
            processMessageText(&messageSegment, useDefaults);
            message.append(messageSegment);
            index = lpszMessage->indexOf("</a>", aStart) + 4;
            QString linkSegment = lpszMessage->mid(aStart, index - aStart);
            message.append(linkSegment);
        } else {
            QString messageSegment = lpszMessage->mid(index);
            processMessageText(&messageSegment, useDefaults);
            message.append(messageSegment);
            break;
        }
    }

    message.replace("\n", "<br/>");

    *lpszMessage = message;
}

void wavrMessageLog::processMessageText(QString* lpszMessageText, bool useDefaults) {
    ChatHelper::makeHtmlSafe(lpszMessageText);
    //	if smileys are enabled, replace text emoticons with corresponding images
    if(!useDefaults && showSmiley)
        ChatHelper::decodeSmileys(lpszMessageText);
}

QString wavrMessageLog::getTimeString(QDateTime* pTime) {
    QString szTimeStamp;
    if(messageTime) {
        szTimeStamp.append("(");
        if(messageDate)
            szTimeStamp.append(pTime->date().toString(Qt::SystemLocaleShortDate) + "&nbsp;");
        szTimeStamp.append(pTime->time().toString(Qt::SystemLocaleShortDate) + ")&nbsp;");
    }

    return szTimeStamp;
}

void wavrMessageLog::setUIText(void) {
    copyAction->setText(tr("&Copy"));
    selectAllAction->setText(tr("Select &All"));
    reloadMessageLog();
}

