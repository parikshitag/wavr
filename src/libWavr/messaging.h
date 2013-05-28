#ifndef MESSAGING_H
#define MESSAGING_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "shared.h"
#include "message.h"
#include "network.h"
#include "settings.h"
#include "xmlmessage.h"

struct PendingMsg {
    qint64 msgId;
    bool active;
    QDateTime timeStamp;
    MessageType type;
    QString userId;
    wavrXmlMessage xmlMessage;
    int retry;

    PendingMsg(void) {}
    PendingMsg(qint64 nMsgId, bool bActive, QDateTime timeStamp, MessageType mtType, QString szUserId, wavrXmlMessage xmlMessage, int nRetry) {
        this->msgId = nMsgId;
        this->active = bActive;
        this->timeStamp = timeStamp;
        this->type = mtType;
        this->userId = szUserId;
        this->xmlMessage = xmlMessage;
        this->retry = nRetry;
    }
};

struct ReceivedMsg {
    qint64 msgId;
    QString userId;

    ReceivedMsg(void) {}
    ReceivedMsg(qint64 nMsgId, QString szUserId) {
        this->msgId = nMsgId;
        this->userId = szUserId;
    }

    bool operator == (const ReceivedMsg& v) const { return ((this->msgId == v.msgId) && (this->userId.compare(v.userId) == 0)); }
};

class wavrMessaging : public QObject {
    Q_OBJECT

public:
    wavrMessaging(void);
    ~wavrMessaging(void);

    void init(wavrXmlMessage* pInitParams);
    void start(void);
    void update(void);
    void stop(void);
    bool isConnected(void);
    bool canReceive(void);
    void setLoopback(bool on);
    User* getUser(QString* lpszUserId);
    void sendBroadcast(MessageType type, wavrXmlMessage* pMessage);
    void sendMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void settingsChanged(void);
    //void updateGroup(GroupOp op, QVariant value1, QVariant value2);
    //void updateGroupMap(QString oldGroup, QString newGroup);
    void saveGroups(void);
    int userCount(void);

    User* localUser;
    QList<User> userList;
    //QList<Group> groupList;

signals:
    void messageReceived(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void connectionStateChanged(void);

private slots:
    void receiveBroadcast(DatagramHeader* pHeader, QString* lpszData);
    void receiveMessage(DatagramHeader* pHeader, QString* lpszData);
    void receiveWebMessage(QString* lpszData);
    void newConnection(QString* lpszUserId, QString* lpszAddress);
    void connectionLost(QString* lpszUserId);
    void receiveProgress(QString* lpszUserId, QString* lpszData);
    void network_connectionStateChanged(void);
    void timer_timeout(void);

private:
    QString createUserId(QString* lpszAddress, QString* lpszUserName);
    QString getUserName(void);
    void loadGroups(void);
    void getUserInfo(wavrXmlMessage* pMessage);
    void sendUserData(MessageType type, QueryOp op, QString* lpszUserId, QString* lpszAddress);
    void prepareBroadcast(MessageType type, wavrXmlMessage* pMessage);
    void prepareMessage(MessageType type, qint64 msgId, bool retry, QString* lpszUserId, wavrXmlMessage* pMessage);
   // void prepareFile(MessageType type, qint64 msgId, bool retry, QString* lpszUserId, wavrXmlMessage* pMessage);
    //void prepareFolder(MessageType type, qint64 msgId, bool retry, QString* lpszUserId, wavrXmlMessage* pMessage);
    void processBroadcast(MessageHeader* pHeader, wavrXmlMessage* pMessage);
    void processMessage(MessageHeader* pHeader, wavrXmlMessage* pMessage);
   // void processFile(MessageHeader* pHeader, wavrXmlMessage* pMessage);
   // void processFolder(MessageHeader* pHeader, wavrXmlMessage* pMessage);
  //  void processWebMessage(MessageHeader* pHeader, wavrXmlMessage* pMessage);
    bool addUser(QString szUserId, QString szVersion, QString szAddress, QString szName, QString szStatus, QString szAvatar, QString szNote, QString szCaps);
    void updateUser(MessageType type, QString szUserId, QString szUserData);
    void removeUser(QString szUserId);
    bool addReceivedMsg(qint64 msgId, QString userId);
    void addPendingMsg(qint64 msgId, MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);
    void removePendingMsg(qint64);
    void removeAllPendingMsg(QString* lpszUserId);
    void checkPendingMsg(void);
    void resendMessage(MessageType type, qint64 msgId, QString* lpszUserId, wavrXmlMessage* pMessage);
   // bool addFileTransfer(FileMode fileMode, QString *lpszUserId, wavrXmlMessage *pMessage);
  //  bool updateFileTransfer(FileMode fileMode, FileOp fileOp, QString *lpszUserId, wavrXmlMessage *pMessage);
  //  QString getFreeFileName(QString fileName);
   // bool addFolderTransfer(FileMode folderMode, QString* lpszUserId, wavrXmlMessage* pMessage);
   // bool updateFolderTransfer(FileMode folderMode, FileOp folderOp, QString* lpszUserId, wavrXmlMessage* pMessage);
  //  QString getFreeFolderName(QString folderName);
  //  QString getFolderPath(QString folderId, QString userId, FileMode mode);

    wavrNetwork*		pNetwork;
    wavrSettings*		pSettings;
    QTimer*				pTimer;
    qint64				msgId;
    QList<ReceivedMsg>	receivedList;
    QList<PendingMsg>	pendingList;
  //  QList<TransFile>    fileList;
  //  QList<TransFolder>  folderList;
    int					nTimeout;
    int					nMaxRetry;
    bool				loopback;
    QMap<QString, QString> userGroupMap;
};


#endif // MESSAGING_H
