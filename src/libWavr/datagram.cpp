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


#include "datagram.h"

void wavrDatagram::addHeader(DatagramType type, QByteArray &baData) {
    QByteArray datagramType = DatagramTypeNames[type].toLocal8Bit();
    baData.insert(0, datagramType);
}

bool wavrDatagram::getHeader(QByteArray &baDatagram, DatagramHeader **ppHeader) {
    QString datagramType(baDatagram.mid(0, 6)); // first 6 bytes represent datagram type
    qDebug() << "Datagram tyep " << datagramType;
    int type = wavrHelper::indexOf(DatagramTypeNames, DT_Max, datagramType);
    if (type < 0)
        return false;

    *ppHeader = new DatagramHeader(
                (DatagramType)type,
                QString(),
                QString());
    return true;
}

QByteArray wavrDatagram::getData(QByteArray &baDatagram) {
    if(baDatagram.length() > 6)
        return baDatagram.mid(6);

    return QByteArray();
}
