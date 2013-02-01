#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//  Information about the application
#define IDA_TITLE       "Wavr"
#define IDA_PRODUCT     "Wavr"
#define IDA_COMPANY     "Wavr"
#define IDA_VERSION     "0.1.0"
#define IDA_DESCRIPTION "Wavr is a free peer-to-peer instant messaging application for local-network commumication"\
                        "It works on essentially every popular desktop platform."
#define IDA_CONTACT     "parikshit.ag@hotmail.com"

#if defined Q_WS_WIN
#define IDA_PLATFORM    "Windows"
#elif   defined Q_WS_MAC
#define IDA_PLATFORM    "Macintosh"
#elif   defined Q_WS_X11
#define IDA_PLATFORM    "Linux"
#endif

/****************************************************************************
**	Socket type definitions
**	The enum and the string array should always be synced
****************************************************************************/
#define ST_MESSAGE  "MSG"
#define ST_FILE     "FILE"

/****************************************************************************
**	Datagram type definitions
**	The enum and the string array should always be synced
****************************************************************************/
enum DatagramType {
    DT_None = 0,
    DT_Broadcast,   //for broadcasting presence
    DT_Message,     //for packet containing message
    DT_Max
};


const QString DatagramTypeNames[] = {
    "",
    "BRDCST",
    "MESSAG"
};



#endif // DEFINITIONS_H


