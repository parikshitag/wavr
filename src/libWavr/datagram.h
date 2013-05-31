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


#ifndef DATAGRAM_H
#define DATAGRAM_H

#include "shared.h"
class wavrDatagram {
public:
    static void addHeader(DatagramType type, QByteArray& baData);
    static bool getHeader(QByteArray& baDatagram, DatagramHeader** ppHeader);
    static QByteArray getData(QByteArray& baDatagram);
};

#endif // DATAGRAM_H
