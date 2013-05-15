/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QProgressIndicator.h"

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout;
    QComboBox *cmbStatus;
    QProgressIndicator *qProgressSpinner;
    QFrame *frame_2;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *searchEdit;
    QPushButton *searchClose;
    QTreeWidget *userList;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(276, 265);
        widget = new QWidget(Form);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 10, 258, 226));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(widget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        widget1 = new QWidget(frame);
        widget1->setObjectName(QStringLiteral("widget1"));
        widget1->setGeometry(QRect(50, 10, 103, 29));
        horizontalLayout = new QHBoxLayout(widget1);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        cmbStatus = new QComboBox(widget1);
        cmbStatus->setObjectName(QStringLiteral("cmbStatus"));
        cmbStatus->setMinimumSize(QSize(85, 0));

        horizontalLayout->addWidget(cmbStatus);

        qProgressSpinner = new QProgressIndicator(widget1);
        qProgressSpinner->setObjectName(QStringLiteral("qProgressSpinner"));

        horizontalLayout->addWidget(qProgressSpinner);


        verticalLayout->addWidget(frame);

        frame_2 = new QFrame(widget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        widget2 = new QWidget(frame_2);
        widget2->setObjectName(QStringLiteral("widget2"));
        widget2->setGeometry(QRect(31, 20, 239, 29));
        horizontalLayout_2 = new QHBoxLayout(widget2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        searchEdit = new QLineEdit(widget2);
        searchEdit->setObjectName(QStringLiteral("searchEdit"));

        horizontalLayout_2->addWidget(searchEdit);

        searchClose = new QPushButton(widget2);
        searchClose->setObjectName(QStringLiteral("searchClose"));

        horizontalLayout_2->addWidget(searchClose);


        verticalLayout->addWidget(frame_2);

        userList = new QTreeWidget(widget);
        userList->setObjectName(QStringLiteral("userList"));

        verticalLayout->addWidget(userList);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        cmbStatus->clear();
        cmbStatus->insertItems(0, QStringList()
         << QApplication::translate("Form", "Available", 0)
         << QApplication::translate("Form", "Busy", 0)
         << QApplication::translate("Form", "Away", 0)
         << QApplication::translate("Form", "Invisible", 0)
         << QApplication::translate("Form", "Offline", 0)
        );
        searchClose->setText(QApplication::translate("Form", "P", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
