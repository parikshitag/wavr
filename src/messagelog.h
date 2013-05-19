#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebView>
#include "libWavr/xmlmessage.h"

/**
 * @brief Shows the chat log ie chat happened so far in GUI windows.
 *  Message log stores the message, broadcast, file and links log in the window that happened so far.
 *  Messages and broadcast are logged with different font styles, links store the url of the link and are triggerable.
 */
class wavrMessageLog
{
    Q_OBJECT
public:
    wavrMessageLog(QWidget *parent = 0);
    ~wavrMessageLog(void);

    void initMessageLog(QString themePath, bool clearLog = true);
    void appendMessageLog(MessageType type, QString* lpszUserId, QString* lpszUserName, wavrXmlMessage* pMessage, bool bReload = false);
    void updateUserName(QString* lpszUserId, QString* lpszUserName);
    void updateAvatar(QString* lpszUserId, QString* lpszFilePath);
    void reloadMessageLog(void);
    void setAutoScroll(bool enable);
    void saveMessageLog(QString filePath);
    void restoreMessageLog(QString filePath, bool reload = true);

    QString localId;
    QString peerId;
    QString peerName;
    QHash<QString, QString> participantAvatars;
    QString lastId;
    bool hasData;
    bool showSmiley;
    int fontSizeVal;
    bool messageTime;
    bool messageDate;
    QString themePath;
    bool allowLinks;
    bool pathToLink;
    bool trimMessage;

signals:
    void messageSent(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);

protected:
    void changeEvent(QEvent* event);

private slots:
    void log_LinkClicked(QUrl url);
    void log_contentsSizeChanged(QSize size);
    void log_linkHovered(const QString& link, const QString& title, const QString& textContent);
    void showContextMenu(const QPoint& pos);
    void copyAction_triggered(void);
    void copyLinkAction_triggered(void);
    void selectAllAction_triggered(void);

private:
    void createContextMenu(void);
    void appendMessageLog(QString* lpszHtml);
    void removeMessageLog(QString divClass);
    void appendBroadcast(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime);
    void appendMessage(QString* lpszUserId, QString* lpszUserName, QString* lpszMessage, QDateTime* pTime,
                       QFont* pFont, QColor* pColor);
    QString getFontStyle(QFont* pFont, QColor* pColor, bool size = false);
    QString getChatStatusMessage(ChatState chatState);
    void decodeMessage(QString* lpszMessage, bool useDefaults = false);
    void processMessageText(QString* lpszMessageText, bool useDefaults);
    QString getTimeString(QDateTime* pTime);
    void setUIText(void);

    QList<SingleMessage> messageLog;
    ThemeData themeData;
    QMenu* contextMenu;
    QAction* copyAction;
    QAction* copyLinkAction;
    QAction* selectAllAction;
    bool linkHovered;
    bool outStyle;
    bool autoScroll;
};

#endif // MESSAGELOG_H
