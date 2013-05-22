#include <QDesktopServices>
#include <QTimer>
#include <QUrl>
#include "mainwindow.h"
#include "messagelog.h"

wavrMainWindow::wavrMainWindow(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags) {
    ui->setupUi(this);

    connect(ui.tvUserList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(tvUserList_itemActivated(QTreeWidgetItem*, int)));
    connect(ui.tvUserList, SIGNAL(itemContextMenu(QTreeWidgetItem*, QPoint&)),
        this, SLOT(tvUserList_itemContextMenu(QTreeWidgetItem*, QPoint&)));
    connect(ui.tvUserList, SIGNAL(itemDragDropped(QTreeWidgetItem*)),
        this, SLOT(tvUserList_itemDragDropped(QTreeWidgetItem*)));
    connect(ui.tvUserList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
        this, SLOT(tvUserList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui.tvUserList->installEventFilter(this);

    windowLoaded = false;
}

wavrMainWindow::~wavrMainWindow() {
    delete ui;
}

void wavrMainWindow::init(User *pLocalUser, QList<Group> *pGroupList, bool connected) {
    setWindowIcon(QIcon(IDR_APPICON));

    this->pLocalUser = pLocalUser;

}
