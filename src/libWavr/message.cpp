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


#include "message.h"
#include <QDateTime>

QString Message::addHeader(MessageType type, qint64 id, QString* lpszLocalId, QString* lpszPeerId, wavrXmlMessage* pMessage) {
    if(!pMessage)
        pMessage = new wavrXmlMessage();

    // remove time stamp from message
    pMessage->removeHeader(XML_TIME);

    pMessage->addHeader(XML_FROM, *lpszLocalId);
    if(lpszPeerId)
        pMessage->addHeader(XML_TO, *lpszPeerId);
    pMessage->addHeader(XML_MESSAGEID, QString::number(id));
    pMessage->addHeader(XML_TYPE, MessageTypeNames[type]);

    return pMessage->toString();
}

bool Message::getHeader(QString* lpszMessage, MessageHeader** ppHeader, wavrXmlMessage** ppMessage) {
    *ppMessage = new wavrXmlMessage(*lpszMessage);
    if(!((*ppMessage)->isValid()))
        return false;

    // add time stamp to message
    (*ppMessage)->addHeader(XML_TIME, QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()));

    int type = wavrHelper::indexOf(MessageTypeNames, MT_Max, (*ppMessage)->header(XML_TYPE));
    if(type < 0)
        return false;

    *ppHeader = new MessageHeader(
                    (MessageType)type,
                    (*ppMessage)->header(XML_MESSAGEID).toLongLong(),
                    (*ppMessage)->header(XML_FROM));
    return true;
}
