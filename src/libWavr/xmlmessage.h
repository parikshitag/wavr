#ifndef XMLMESSAGE_H
#define XMLMESSAGE_H

#include <QDomDocument>
#include    "definitions.h"

#define XML_ROOT				APP_MARKER
#define XML_HEAD				"head"
#define XML_BODY				"body"
#define XML_FROM				"from"
#define XML_TO                  "to"

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
