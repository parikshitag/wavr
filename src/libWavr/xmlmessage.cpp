#include "xmlmessage.h"

/**
 *  @class wavrXmlMessage
 *  @ingroup  libWavr
 *  @brief Creates a xml file that is transmitted over network.
 *  Includes setter and getter methods for HEAD and BODY elements.
 */

/**
 *  Creates root, head and body element of XML DOM.
 */
wavrXmlMessage::wavrXmlMessage(){
    QDomElement root = createElement(XML_ROOT);
    appendChild(root);

    QDomElement head = createElement(XML_HEAD);
    root.appendChild(head);

    QDomElement body = createElement(XML_BODY);
    root.appendChild(body);
}

/**
 * @param text
 *  sets the content of XML Document.
 */
wavrXmlMessage::wavrXmlMessage(const QString &text) : QDomDocument() {
    setContent(text);
}

wavrXmlMessage::~wavrXmlMessage(void){
}

/**
 * @param nodeName  node name to be added.
 * @param nodeValue node value to be added.
 * @return
 *  Add node name and value to HEAD as parent node.
 */
bool wavrXmlMessage::addHeader(const QString &nodeName, const QString &nodeValue){
    return addXmlNode(XML_HEAD, nodeName, nodeValue);
}

/**
 * @param nodeName  node name to be added.
 * @param nodeValue node value to be added.
 * @return
 * Add node name and value to BODY as parent node.
 */
bool wavrXmlMessage::addData(const QString& nodeName, const QString& nodeValue) {
    return addXmlNode(XML_BODY, nodeName, nodeValue);
}

/**
 * @param nodeName node name to be retrieved.
 * @return returns the node value associated with given node name.
 *  Get header node.
 */
QString wavrXmlMessage::header(const QString& nodeName) {
    return getXmlNode(XML_HEAD, nodeName);
}

/**
 * @param nodeName node name to be retrieved.
 * @return reutrns the node value associated with given node name.
 *  Get Body node.
 */
QString wavrXmlMessage::data(const QString& nodeName) {
    return getXmlNode(XML_BODY, nodeName);
}

/**
 * @param nodeName  Node name to be removed.
 * @return Returns true if successfully removed else false.
 *  Removes the given Header node.
 */
bool wavrXmlMessage::removeHeader(const QString& nodeName) {
    return removeXmlNode(XML_HEAD, nodeName);
}

/**
 * @param nodeName  Node name to be removed.
 * @return Returns true if successfully removed else false.
 *  Removes the given Body node.
 */
bool wavrXmlMessage::removeData(const QString& nodeName) {
    return removeXmlNode(XML_BODY, nodeName);
}

/**
 * @param nodeName Node name to be searched.
 * @return Returns true if node exists else false.
 *  Searches for the given node in Head element.
 */
bool wavrXmlMessage::headerExists(const QString& nodeName) {
    return xmlNodeExists(XML_HEAD, nodeName);
}

/**
 * @param nodeName  Node name to be searched.
 * @return Returns true if node exists else false.
 *  Searches for the given node in Body element.
 */
bool wavrXmlMessage::dataExists(const QString& nodeName) {
    return xmlNodeExists(XML_BODY, nodeName);
}

/**
 * @return Returns the Cloned XML message class.
 *  Clones the XML Message.
 */
wavrXmlMessage wavrXmlMessage::clone(void) {
    wavrXmlMessage newMsg;
    newMsg.setContent(toString());
    return newMsg;
}

/**
 * @return Returns true if XML document is valid.
 * Checks the root element if equals APP_MARKER then returns true else false.
 */
bool wavrXmlMessage::isValid(void) {
    QDomElement root = documentElement();
    if(root.isNull())
        return false;

    if(root.tagName() == APP_MARKER)
        return true;

    return false;
}

// adds the xmlNode for HEAD and BODY elements.
bool wavrXmlMessage::addXmlNode(const QString& parentNode, const QString& nodeName, const QString& nodeValue) {
    QDomElement root = documentElement();
    if(root.isNull())
        return false;

    QDomNodeList nodes = root.elementsByTagName(parentNode);
    if(nodes.isEmpty())
        return false;

    QDomElement parent = nodes.at(0).toElement();
    QDomElement element = createElement(nodeName);
    parent.appendChild(element);
    QDomText elementText = createTextNode(nodeValue);
    element.appendChild(elementText);
    return true;
}

//Returns the XmlNode element for specified nodeName.
QString wavrXmlMessage::getXmlNode(const QString& parentNode, const QString& nodeName) {
    QDomElement root = documentElement();
    if(root.isNull())
        return QString::null;

    QDomNodeList nodes = root.elementsByTagName(parentNode);
    if(nodes.isEmpty())
        return QString::null;

    QDomElement parent = nodes.at(0).toElement();
    nodes = parent.elementsByTagName(nodeName);
    if(nodes.isEmpty())
        return QString::null;

    QDomElement element = nodes.at(0).toElement();
    return element.text();
}

//Removes the XmlNode for given nodeName
bool wavrXmlMessage::removeXmlNode(const QString& parentNode, const QString& nodeName) {
    QDomElement root = documentElement();
    if(root.isNull())
        return false;

    QDomNodeList nodes = root.elementsByTagName(parentNode);
    if(nodes.isEmpty())
        return false;

    QDomElement parent = nodes.at(0).toElement();
    nodes = parent.elementsByTagName(nodeName);
    if(nodes.isEmpty())
        return false;

    QDomElement element = nodes.at(0).toElement();
    QDomNode newNode = parent.removeChild(element);
    if(newNode.isNull())
        return false;

    return true;
}

//Returns whether a given nodeName exists in Xml document or not.
bool wavrXmlMessage::xmlNodeExists(const QString& parentNode, const QString& nodeName) {
    QDomElement root = documentElement();
    if(root.isNull())
        return false;

    QDomNodeList nodes = root.elementsByTagName(parentNode);
    if(nodes.isEmpty())
        return false;

    QDomElement parent = nodes.at(0).toElement();
    nodes = parent.elementsByTagName(nodeName);
    if(nodes.isEmpty())
        return false;

    return true;
}
