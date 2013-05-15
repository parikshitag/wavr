/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QVBoxLayout *verticalLayout_3;
    QSplitter *splitter;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QWidget *wgtLog;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *logLayout;
    QLabel *lblInfo;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_2;
    QWidget *wgtToolBar;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *toolBarLayout;
    QTextEdit *txtMessage;

    void setupUi(QWidget *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QStringLiteral("ChatWindow"));
        ChatWindow->resize(344, 402);
        verticalLayout_3 = new QVBoxLayout(ChatWindow);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        splitter = new QSplitter(ChatWindow);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        widget = new QWidget(splitter);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        wgtLog = new QWidget(widget);
        wgtLog->setObjectName(QStringLiteral("wgtLog"));
        horizontalLayout = new QHBoxLayout(wgtLog);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        logLayout = new QVBoxLayout();
        logLayout->setObjectName(QStringLiteral("logLayout"));

        horizontalLayout->addLayout(logLayout);


        verticalLayout->addWidget(wgtLog);

        lblInfo = new QLabel(widget);
        lblInfo->setObjectName(QStringLiteral("lblInfo"));

        verticalLayout->addWidget(lblInfo);

        splitter->addWidget(widget);
        widget1 = new QWidget(splitter);
        widget1->setObjectName(QStringLiteral("widget1"));
        verticalLayout_2 = new QVBoxLayout(widget1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        wgtToolBar = new QWidget(widget1);
        wgtToolBar->setObjectName(QStringLiteral("wgtToolBar"));
        verticalLayout_4 = new QVBoxLayout(wgtToolBar);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        toolBarLayout = new QHBoxLayout();
        toolBarLayout->setObjectName(QStringLiteral("toolBarLayout"));

        verticalLayout_4->addLayout(toolBarLayout);


        verticalLayout_2->addWidget(wgtToolBar);

        txtMessage = new QTextEdit(widget1);
        txtMessage->setObjectName(QStringLiteral("txtMessage"));

        verticalLayout_2->addWidget(txtMessage);

        splitter->addWidget(widget1);

        verticalLayout_3->addWidget(splitter);


        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QWidget *ChatWindow)
    {
        ChatWindow->setWindowTitle(QApplication::translate("ChatWindow", "Form", 0));
        lblInfo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
