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
