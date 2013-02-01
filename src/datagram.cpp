#include "datagram.h"

void wavrDatagram::addHeader(DatagramType type, QByteArray &baData) {
    QByteArray datagramType = DatagramTypeNames[type].toLocal8Bit();
    baData.insert(0, datagramType);
}

bool wavrDatagram::getHeader(QByteArray &baDatagram, DatagramHeader **ppHeader) {
    QString DatagramType(baDatagram.mid(0, 6)); // first 6 bytes represent datagram type
    int type = Helper::indexOf(DatagramTypeNames, DT_Max, DatagramType);
    if (type < 0)
        return false;

    *ppHeader = new DatagramHeader(
                (DatagramType) type,
                QString(),
                QString());
    return true;
}

QByteArray wavrDatagram::getData(QByteArray &baDatagram) {
    if(baDatagram.length() > 6)
        return baDatagram.mid(6);

    return QByteArray();
}
