#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <libWavr/shared.h>

#include "libWavr/xmlmessage.h"

namespace Ui {
class wavrMainWindow;
}

class wavrMainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit wavrMainWindow(QWidget *parent = 0);
    ~wavrMainWindow();
    
    void init(User* pLocalUser, QList<Group>* pGroupList, bool connected);
    void start(void);
    void addUser(User* pUser);  //add new user discoverd to the list
    void updateUser(User* pUser); //update user value
    void removeUser(QString* lpszUserId);
    void receiveMessage(MessageType type, QString* lpszUserId, wavrXmlMessage* pMessage);

signals:
    void appExiting(void);



private:
    void createMainMenu(void);
    void createAvatarMenu(void);

    Ui::wavrMainWindow *ui;
};

#endif // MAINWINDOW_H
