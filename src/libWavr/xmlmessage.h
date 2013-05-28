#ifndef XMLMESSAGE_H
#define XMLMESSAGE_H

#include <QDomDocument>
#include    "definitions.h"

#define XML_ROOT				APP_MARKER
#define XML_HEAD				"head"
#define XML_BODY				"body"
#define XML_FROM				"from"
#define XML_TO                  "to"
#define XML_TIME                "time"

#define XML_ADDRESS              "address"
#define XML_USERID               "userid"
#define XML_NAME                "name"
#define XML_VERSION              "version"

#define XML_STATUS               "status"

#define XML_LOGON                "logon"
#define XML_HOST                 "host"
#define XML_OS                   "os"
#define XML_FIRSTNAME            "firstname"
#define XML_LASTNAME             "lastname"
#define XML_ABOUT                "about"

#define XML_MESSAGE             "message"
#define XML_BROADCAST           "broadcast"

#define XML_CHATSTATE           "chatstate"

#define XML_FONT                "font"
#define XML_COLOR               "color"

#define XML_NOTE                "note"
#define XML_PORT                "port"

class wavrXmlMessage : public QDomDocument
{
public:
    wavrXmlMessage();
    wavrXmlMessage(const QString& text);
    ~wavrXmlMessage(void);

    bool addHeader(const QString& nodeName, const QString& nodeValue);
    bool addData(const QString& nodeName, const QString& nodeValue);
    QString header(const QString& nodeName);
    QString data(const QString& nodeName);
    bool removeHeader(const QString& nodeName);
    bool removeData(const QString& nodeName);
    bool headerExists(const QString& nodeName);
    bool dataExists(const QString& nodeName);
    wavrXmlMessage clone(void);
    bool isValid(void);

private:
    bool addXmlNode(const QString& parentNode, const QString& nodeName, const QString& nodeValue);
    QString getXmlNode(const QString& parentNode, const QString& nodeName);
    bool removeXmlNode(const QString& parentNode, const QString& nodeName);
    bool xmlNodeExists(const QString& parentNode, const QString& nodeName);

};

#endif // XMLMESSAGE_H
