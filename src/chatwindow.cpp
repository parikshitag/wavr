#include "chatwindow.h"
#include "ui_chatwindow.h"

wavrChatWindow::wavrChatWindow(QWidget *parent, Qt::WindowFlags flags) :
    QWidget(parent, flags)
{
    ui->setupUi(this);
    // Destroy the window when it closes
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAcceptDrops(true);

}

wavrChatWindow::~wavrChatWindow()
{
    delete ui;
}
