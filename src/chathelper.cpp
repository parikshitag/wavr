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


#include <QDataStream>
#include "chathelper.h"

QDataStream &operator << (QDataStream &out, const SingleMessage &message) {
    out << qint32(message.type) << message.userId << message.userName << message.message.toString()
        << message.id;
    return out;
}

QDataStream &operator >> (QDataStream &in, SingleMessage &message) {
    qint32 type;
    QString userId;
    QString userName;
    QString xmlMessage;
    QString id;
    in >> type >> userId >> userName >> xmlMessage >> id;
    message = SingleMessage((MessageType)type, userId, userName, wavrXmlMessage(xmlMessage), id);
    return in;
}

void ChatHelper::makeHtmlSafe(QString* lpszMessage) {
    for(int index = 0; index < HTMLESC_COUNT; index++)
        lpszMessage->replace(htmlSymbol[index], htmlEscape[index]);
}

QString ChatHelper::replaceSmiley(QString* lpszHtml) {
    //<img src="qrc:/smileys/01">
    for(int index = 0; index < SM_MAPCOUNT; index++) {
        if(lpszHtml->compare("<img src=\"qrc" + smileyPic[index] + "\">") == 0) {
            QString code = smileyCode[index];
            makeHtmlSafe(&code);
            return code;
        }
    }

    return QString::null;
}

void ChatHelper::encodeSmileys(QString* lpszMessage) {
    //	replace all emoticon images with corresponding text code
    for(int index = 0; index < SM_MAPCOUNT; index++) {
        QString code = smileyCode[index];
        makeHtmlSafe(&code);
        lpszMessage->replace("<img src=\"" + smileyPic[index] + "\" />", code);
    }
}

void ChatHelper::decodeSmileys(QString* lpszMessage) {
    //	replace text emoticons with corresponding images
    for(int index = 0; index < SM_MAPCOUNT; index++) {
        QString code = smileyCode[index];
        makeHtmlSafe(&code);
        lpszMessage->replace(code, "<img src='qrc" + smileyPic[index] + "' />", Qt::CaseInsensitive);
    }
}

