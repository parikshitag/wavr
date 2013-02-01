#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void init();
    void start(void);
    void addUser(User* pUser);  //add new user discoverd to the list
    void updateUser(User* pUser); //update user value
    void removeUser(QString* lpszUserId);
    void receiveMessage(MessageType type, QString* lpszUserId, XmlMessage* pMessage);

signals:
    void appExiting(void);



private:
    void createMainMenu(void);
    void createAvatarMenu(void);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
