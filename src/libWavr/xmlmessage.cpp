#include "xmlmessage.h"

/**
 * @brief wavrXmlMessage::wavrXmlMessage
 *  Creates root, head and body element of XML DOM.
 */
wavrXmlMessage::wavrXmlMessage(){
    QDomElement root = createElement(XN_ROOT);
    appendChild(root);

    QDomElement head = createElement(XN_HEAD);
    root.appendChild(head);

    QDomElement body = createElement(XN_BODY);
    root.appendChild(body);
}

/**
 * @brief wavrXmlMessage::wavrXmlMessage
 * @param text
 *  sets the content of XML Document.
 */
wavrXmlMessage::wavrXmlMessage(const QString &text) : QDomDocument() {
    setContent(text);
}

wavrXmlMessage::~wavrXmlMessage(void){
}

/**
 * @brief wavrXmlMessage::addHeader
 * @param nodeName  node name to be added.
 * @param nodeValue node value to be added.
 * @return
 *  Add node name and value to HEAD as parent node.
 */
bool wavrXmlMessage::addHeader(const QString &nodeName, const QString &nodeValue){
    return addXmlNode(XN_HEAD, nodeName, nodeValue);
}

/**
 * @brief wavrXmlMessage::addData
 * @param nodeName  node name to be added.
 * @param nodeValue node value to be added.
 * @return
 * Add node name and value to BODY as parent node.
 */
bool wavrXmlMessage::addData(const QString& nodeName, const QString& nodeValue) {
    return addXmlNode(XN_BODY, nodeName, nodeValue);
}

/**
 * @brief wavrXmlMessage::header
 * @param nodeName node name to be retrieved.
 * @return returns the node value associated with given node name.
 *  Get header node.
 */
QString wavrXmlMessage::header(const QString& nodeName) {
    return getXmlNode(XN_HEAD, nodeName);
}

/**
 * @brief wavrXmlMessage::data
 * @param nodeName node name to be retrieved.
 * @return reutrns the node value associated with given node name.
 *  Get Body node.
 */
QString wavrXmlMessage::data(const QString& nodeName) {
    return getXmlNode(XN_BODY, nodeName);
}

/**
 * @brief wavrXmlMessage::removeHeader
 * @param nodeName  Node name to be removed.
 * @return Returns true if successfully removed else false.
 *  Removes the given Header node.
 */
bool wavrXmlMessage::removeHeader(const QString& nodeName) {
    return removeXmlNode(XN_HEAD, nodeName);
}

/**
 * @brief wavrXmlMessage::removeData
 * @param nodeName  Node name to be removed.
 * @return Returns true if successfully removed else false.
 *  Removes the given Body node.
 */
bool wavrXmlMessage::removeData(const QString& nodeName) {
    return removeXmlNode(XN_BODY, nodeName);
}

/**
 * @brief wavrXmlMessage::headerExists
 * @param nodeName Node name to be searched.
 * @return Returns true if node exists else false.
 *  Searches for the given node in Head element.
 */
bool wavrXmlMessage::headerExists(const QString& nodeName) {
    return xmlNodeExists(XN_HEAD, nodeName);
}

/**
 * @brief wavrXmlMessage::dataExists
 * @param nodeName  Node name to be searched.
 * @return Returns true if node exists else false.
 *  Searches for the given node in Body element.
 */
bool wavrXmlMessage::dataExists(const QString& nodeName) {
    return xmlNodeExists(XN_BODY, nodeName);
}

/**
 * @brief XmlMessage::clone
 * @return Returns the Cloned XML message class.
 *  Clones the XML Message.
 */
wavrXmlMessage XmlMessage::clone(void) {
    XmlMessage newMsg;
    newMsg.setContent(toString());
    return newMsg;
}

bool wavrXmlMessage::isValid(void) {
    QDomElement root = documentElement();
    if(root.isNull())
        return false;

    if(root.tagName() == APP_MARKER)
        return true;

    return false;
}
