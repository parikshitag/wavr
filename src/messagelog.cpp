#include "messagelog.h"

wavrMessageLog::wavrMessageLog(QWidget *parent) : QWebView(parent) {
    connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(log_LinkClicked(QUrl)));
    connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
            this, SLOT(log_contentsSizeChanged(QSize)));
    connect(this->page(), SIGNAL(linkHovered(QString, QString, QString)),
            this, SLOT(log_linkHovered(QString, QString, QString)));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    createContextMenu();

    participantAvatars.clear();
    hasData = false;
    messageTime = false;
    messageDate = false;
    allowLinks = false;
    pathToLink = false;
    trimMessage = true;
    fontSizeVal = 0;
    lastId = QString::null;
    messageLog.clear();
    linkHovered = false;
    outStyle = false;
    autoScroll = true;
}

wavrMessageLog::~wavrMessageLog() {
}

void wavrMessageLog::initMessageLog(QString themePath, bool clearLog) {
    if (clearLog)
        messageLog.clear();
    lastId = QString::null;
    this->themePath = themePath;
    themeData = wavrTheme::localTheme(themePath);
    setHtml(themeData.document);
}

void wavrMessageLog::createContextMenu(void) {
    QAction* action = pageAction(QWebPage::Copy);
    action->setShortcut(QKeySequence::Copy);
    addAction(action);
    action = pageAction(QWebPage::CopyLinkToClipboard);
    addAction(action);
    action = pageAction(QWebPage::SelectAll);
    action->setShortcut(QKeySequence::SelectAll);
    addAction(action);

    contextMenu = new QMenu(this);
    copyAction = contextMenu->addAction("&Copy", this, SLOT(copyAction_triggered()), QKeySequence::Copy);
    copyLinkAction = contextMenu->addAction("&Copy Link", this, SLOT(copyLinkAction_triggered()));
    contextMenu->addSeparator();
    selectAllAction = contextMenu->addAction("Select &All", this,
                                             SLOT(selectAllAction_triggered()), QKeySequence::SelectAll);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void wavrMessageLog::appendMessageLog(MessageType type, QString *lpszUserId, QString *lpszUserName, wavrXmlMessage *pMessage,
                                      bool bReload) {
    if(!pMessage && type != MT_ERROR)
        return;

    QString message;
    QString html;
    QString caption;
    QDateTime time;
    QFont font;
    QColor color;
    QString fontStyle;
    QString id = QString::null;
    bool addToLog = true;

    //removeMessageLog("_wavr_statediv"); theme part

    switch(type) {
    case MT_Message:
        time
    }
}
