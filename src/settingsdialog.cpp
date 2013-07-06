/****************************************************************************
**
** This file is part of Wavr IM Application.
** 
** Copyright (c) 2013 Parikshit Agarwal.
** 
** Contact:  parikshit.ag@gmail.com
** 
** Wavr is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Wavr is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Wavr.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#include <QSystemTrayIcon>
#include <QMessageBox>
#include "settingsdialog.h"

wavrSettingsDialog::wavrSettingsDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags)  {
    ui.setupUi(this);

    //  remove the help button from window button group
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    //  Destroy the window when it closes
    setAttribute(Qt::WA_DeleteOnClose, true);

    connect(ui.lvCategories, SIGNAL(currentRowChanged(int)), this, SLOT(lvCategories_currentRowChanged(int)));
    connect(ui.btnOK, SIGNAL(clicked()), this, SLOT(btnOk_clicked()));
    connect(ui.chkMessageTime, SIGNAL(toggled(bool)), this, SLOT(chkMessageTime_toggled(bool)));
    connect(ui.chkAllowLinks, SIGNAL(toggled(bool)), this, SLOT(chkAllowLinks_toggled(bool)));
    connect(ui.btnReset, SIGNAL(clicked()), this, SLOT(btnReset_clicked()));
    connect(ui.lvBroadcasts, SIGNAL(currentRowChanged(int)), this, SLOT(lvBroadcasts_currentRowChanged(int)));
    connect(ui.txtBroadcast, SIGNAL(textEdited(QString)), this, SLOT(txtBroadcast_textEdited(QString)));
    connect(ui.txtBroadcast, SIGNAL(returnPressed()), this, SLOT(btnAddBroadcast_clicked()));
    connect(ui.btnAddBroadcast, SIGNAL(clicked()), this, SLOT(btnAddBroadcast_clicked()));
    connect(ui.btnDeleteBroadcast, SIGNAL(clicked()), this, SLOT(btnDeleteBroadcast_clicked()));

}

wavrSettingsDialog::~wavrSettingsDialog()
{
}

void wavrSettingsDialog::init(void) {

    setWindowIcon(QIcon(IDR_APPICON));

    createAvatarMenu();

    ui.lvCategories->setIconSize(QSize(32, 32));
    ui.lvCategories->item(0)->setIcon(QIcon(QPixmap(IDR_GENERALSET, "PNG")));
    ui.lvCategories->item(1)->setIcon(QIcon(QPixmap(IDR_ACCOUNTSET, "PNG")));
    ui.lvCategories->item(2)->setIcon(QIcon(QPixmap(IDR_MESSAGESET, "PNG")));
    ui.lvCategories->item(3)->setIcon(QIcon(QPixmap(IDR_HISTORYSET, "PNG")));
    ui.lvCategories->item(4)->setIcon(QIcon(QPixmap(IDR_ALERTSET, "PNG")));
    ui.lvCategories->item(5)->setIcon(QIcon(QPixmap(IDR_NETWORKSET, "PNG")));
    ui.lvCategories->item(6)->setIcon(QIcon(QPixmap(IDR_TRANSFERSET, "PNG")));
    ui.lvCategories->item(7)->setIcon(QIcon(QPixmap(IDR_THEMESET, "PNG")));
    ui.lvCategories->item(8)->setIcon(QIcon(QPixmap(IDR_HOTKEYSET, "PNG")));

    setPageHeaderStyle(ui.lblGeneralPage);
    setPageHeaderStyle(ui.lblAccountPage);
    setPageHeaderStyle(ui.lblMessagesPage);
    setPageHeaderStyle(ui.lblNetworkPage);

    pPortValidator = new QIntValidator(1, 65535, this);
    ui.txtUDPPort->setValidator(pPortValidator);
    ui.txtTCPPort->setValidator(pPortValidator);

    ipRegExp = QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    pIpValidator = new QRegExpValidator(ipRegExp, this);
    ui.txtMulticast->setValidator(pIpValidator);

    pSettings = new wavrSettings();
    setUIText();
    loadSettings();


//    //	if no avatar is set, select a random avatar (useful when running for the first time)
    nAvatar = pSettings->value(IDS_AVATAR, IDS_AVATAR_VAL).toInt();
    if(nAvatar > AVT_COUNT) {
        nAvatar = 0;
    }
    setAvatar();
}


void wavrSettingsDialog::lvCategories_currentRowChanged(int currentRow) {
    ui.stackedWidget->setCurrentIndex(currentRow);
}

void wavrSettingsDialog::btnOk_clicked(void) {
    saveSettings();
}

void wavrSettingsDialog::chkMessageTime_toggled(bool checked) {
    Q_UNUSED(checked);

    ui.chkMessageDate->setEnabled(ui.chkMessageTime->isChecked());
}

void wavrSettingsDialog::chkAllowLinks_toggled(bool checked) {
    Q_UNUSED(checked);

    ui.chkPathToLink->setEnabled(ui.chkAllowLinks->isChecked());
}

void wavrSettingsDialog::btnReset_clicked(void) {
    QString message = tr("Are you sure you want to reset your %1 preferences?");
    if(QMessageBox::question(this, tr("Reset Preferences"), message.arg(wavrStrings::appName()), QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::Yes) {
        QFile::remove(pSettings->fileName());
        pSettings->sync();
        accept();
    }
}

void wavrSettingsDialog::lvBroadcasts_currentRowChanged(int index) {
    ui.btnDeleteBroadcast->setEnabled(!(index < 0));
}

void wavrSettingsDialog::txtBroadcast_textEdited(const QString &text) {
    ui.btnAddBroadcast->setEnabled(ipRegExp.exactMatch(text));
}

void wavrSettingsDialog::btnAddBroadcast_clicked(void) {
    QString address = ui.txtBroadcast->text();
    //	Do not add if not a valid ip address
    if(!ipRegExp.exactMatch(address))
        return;

    //	Check if the same address is already present in the list
    for(int index = 0; index < ui.lvBroadcasts->count(); index++) {
        QString text = ui.lvBroadcasts->item(index)->text();
        if(text.compare(address) == 0)
            return;
    }

    QListWidgetItem* item = new QListWidgetItem(ui.lvBroadcasts);
    item->setText(address);

    ui.txtBroadcast->clear();
    ui.btnAddBroadcast->setEnabled(false);
    ui.txtBroadcast->setFocus();
}

void wavrSettingsDialog::btnDeleteBroadcast_clicked(void) {
    if(ui.lvBroadcasts->currentRow() < 0)
        return;

    QListWidgetItem* item = ui.lvBroadcasts->takeItem(ui.lvBroadcasts->currentRow());
    delete item;
}

void wavrSettingsDialog::avatarAction_triggered(void) {
    setAvatar();
}

void wavrSettingsDialog::avatarBrowseAction_triggered(void) {
    QString dir = pSettings->value(IDS_OPENPATH, IDS_OPENPATH_VAL).toString();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select avatar picture"), dir,
        "Images (*.bmp *.gif *.jpg *.jpeg *.png *.tif *.tiff)");
    if(!fileName.isEmpty()) {
        pSettings->setValue(IDS_OPENPATH, QFileInfo(fileName).dir().absolutePath());
        setAvatar(fileName);
    }
}

void wavrSettingsDialog::setAvatar(QString fileName) {
    //	create cache folder if it does not exist
    QDir cacheDir(StdLocation::cacheDir());
    if(!cacheDir.exists())
        cacheDir.mkdir(cacheDir.absolutePath());
    QString filePath = StdLocation::avatarFile();

    //	Save the image as a file in the data folder
    QPixmap avatar;
    bool loadFromStdPath = false;
    if(!fileName.isEmpty()) {
        //	save a backup of the image in the cache folder
        avatar = QPixmap(fileName);
        nAvatar = -1;
    } else {
        //	nAvatar = -1 means custom avatar is set, otherwise load from resource
        if(nAvatar < 0) {
            //	load avatar from image file if file exists, else load default
            if(QFile::exists(filePath)) {
                avatar = QPixmap(filePath);
                loadFromStdPath = true;
            }
            else
                avatar = QPixmap(AVT_DEFAULT);
        } else
            avatar = QPixmap(avtPic[nAvatar]);
    }

    if(!loadFromStdPath) {
        avatar = avatar.scaled(QSize(AVT_WIDTH, AVT_HEIGHT), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        avatar.save(filePath);
    }

    ui.btnAvatar->setIcon(QIcon(QPixmap(filePath, "PNG")));
    //pLocalUser->avatar = nAvatar;
    //sendAvatar(NULL);
}

void wavrSettingsDialog::createAvatarMenu(void) {
    pAvatarMenu = new QMenu(this);

    wavrImagePickerAction* pAction = new wavrImagePickerAction(this, avtPic, AVT_COUNT, 48, 4, &nAvatar);
    connect(pAction, SIGNAL(triggered()), this, SLOT(avatarAction_triggered()));
    pAvatarMenu->addAction(pAction);
    pAvatarMenu->addSeparator();
    avatarBrowseAction = pAvatarMenu->addAction(tr("&Select picture..."), this, SLOT(avatarBrowseAction_triggered()));

    ui.btnAvatar->setMenu(pAvatarMenu);
}

void wavrSettingsDialog::setPageHeaderStyle(QLabel* pLabel) {
    QFont font = pLabel->font();
    int fontSize = pLabel->fontInfo().pixelSize();
    fontSize += (fontSize * 0.2);
    font.setPixelSize(fontSize);
    font.setBold(true);
    pLabel->setFont(font);
}

void wavrSettingsDialog::setUIText(void) {
    ui.retranslateUi(this);

    setWindowTitle(tr("Preferences"));

    ui.chkAutoStart->setText(ui.chkAutoStart->text().arg(wavrStrings::appName()));
    ui.chkAutoShow->setText(ui.chkAutoShow->text().arg(wavrStrings::appName()));

    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
        ui.grpSysTray->setEnabled(false);
        ui.grpSysTray->setTitle(tr("System Tray (Not Available)"));
    }

    //	set minimum possible size
    layout()->setSizeConstraint(QLayout::SetMinimumSize);
}

void wavrSettingsDialog::loadSettings(void) {
        //	Auto start function not implemented on Mac since Mac itself provides an easy UI for it
    #ifdef Q_WS_MAC
        ui.chkAutoStart->setChecked(false);
        ui.chkAutoStart->hide();
    #else
        ui.chkAutoStart->setChecked(pSettings->value(IDS_AUTOSTART, IDS_AUTOSTART_VAL).toBool());
    #endif
        ui.chkAutoShow->setChecked(pSettings->value(IDS_AUTOSHOW, IDS_AUTOSHOW_VAL).toBool());
        //ui.chkSysTray->setChecked(pSettings->value(IDS_SYSTRAY, IDS_SYSTRAY_VAL).toBool());
        //ui.chkMinimizeTray->setChecked(pSettings->value(IDS_MINIMIZETRAY, IDS_MINIMIZETRAY_VAL).toBool());
        //ui.chkSingleClickTray->setChecked(pSettings->value(IDS_SINGLECLICKTRAY, IDS_SINGLECLICKTRAY_VAL).toBool());
        //ui.chkSysTrayMsg->setChecked(pSettings->value(IDS_SYSTRAYMSG, IDS_SYSTRAYMSG_VAL).toBool());
        //ui.chkAllowSysTrayMin->setChecked(pSettings->value(IDS_ALLOWSYSTRAYMIN, IDS_ALLOWSYSTRAYMIN_VAL).toBool());

        ui.txtUserName->setText(pSettings->value(IDS_USERNAME, IDS_USERNAME_VAL).toString());
        ui.txtFirstName->setText(pSettings->value(IDS_USERFIRSTNAME, IDS_USERFIRSTNAME_VAL).toString());
        ui.txtLastName->setText(pSettings->value(IDS_USERLASTNAME, IDS_USERLASTNAME_VAL).toString());
        ui.txtAbout->setPlainText(pSettings->value(IDS_USERABOUT, IDS_USERABOUT_VAL).toString());
        ui.spnRefreshTime->setValue(pSettings->value(IDS_REFRESHTIME, IDS_REFRESHTIME_VAL).toInt());

        ui.rdbMessageTop->setChecked(pSettings->value(IDS_MESSAGETOP, IDS_MESSAGETOP_VAL).toBool());
        ui.rdbMessageBottom->setChecked(!pSettings->value(IDS_MESSAGETOP, IDS_MESSAGETOP_VAL).toBool());
        //ui.chkPublicMessagePop->setChecked(pSettings->value(IDS_PUBMESSAGEPOP, IDS_PUBMESSAGEPOP_VAL).toBool());
        ui.chkEmoticon->setChecked(pSettings->value(IDS_EMOTICON, IDS_EMOTICON_VAL).toBool());
        ui.chkMessageTime->setChecked(pSettings->value(IDS_MESSAGETIME, IDS_MESSAGETIME_VAL).toBool());
        ui.chkMessageDate->setChecked(pSettings->value(IDS_MESSAGEDATE, IDS_MESSAGEDATE_VAL).toBool());
        ui.chkAllowLinks->setChecked(pSettings->value(IDS_ALLOWLINKS, IDS_ALLOWLINKS_VAL).toBool());
        ui.chkPathToLink->setChecked(pSettings->value(IDS_PATHTOLINK, IDS_PATHTOLINK_VAL).toBool());
        ui.chkTrimMessage->setChecked(pSettings->value(IDS_TRIMMESSAGE, IDS_TRIMMESSAGE_VAL).toBool());
        ui.chkClearOnClose->setChecked(pSettings->value(IDS_CLEARONCLOSE, IDS_CLEARONCLOSE_VAL).toBool());

        ui.spnTimeout->setValue(pSettings->value(IDS_TIMEOUT, IDS_TIMEOUT_VAL).toInt());
        ui.spnMaxRetries->setValue(pSettings->value(IDS_MAXRETRIES, IDS_MAXRETRIES_VAL).toInt());
        int size = pSettings->beginReadArray(IDS_BROADCASTHDR);
        for(int index = 0; index < size; index++) {
            pSettings->setArrayIndex(index);
            QListWidgetItem* item = new QListWidgetItem(ui.lvBroadcasts);
            item->setText(pSettings->value(IDS_BROADCAST).toString());
        }
        pSettings->endArray();
        ui.txtMulticast->setText(pSettings->value(IDS_MULTICAST, IDS_MULTICAST_VAL).toString());
        ui.txtUDPPort->setText(pSettings->value(IDS_UDPPORT, IDS_UDPPORT_VAL).toString());
        ui.txtTCPPort->setText(pSettings->value(IDS_TCPPORT, IDS_TCPPORT_VAL).toString());
}

void wavrSettingsDialog::saveSettings(void) {
    pSettings->setValue(IDS_VERSION, IDA_VERSION);

    pSettings->setValue(IDS_AUTOSTART, ui.chkAutoStart->isChecked(), IDS_AUTOSTART_VAL);
    pSettings->setValue(IDS_AUTOSHOW, ui.chkAutoShow->isChecked(), IDS_AUTOSHOW_VAL);
    //pSettings->setValue(IDS_SYSTRAY, ui.chkSysTray->isChecked(), IDS_SYSTRAY_VAL);
    //pSettings->setValue(IDS_MINIMIZETRAY, ui.chkMinimizeTray->isChecked(), IDS_MINIMIZETRAY_VAL);
    //pSettings->setValue(IDS_SINGLECLICKTRAY, ui.chkSingleClickTray->isChecked(), IDS_SINGLECLICKTRAY_VAL);
    //pSettings->setValue(IDS_SYSTRAYMSG, ui.chkSysTrayMsg->isChecked(), IDS_SYSTRAYMSG_VAL);
    //pSettings->setValue(IDS_ALLOWSYSTRAYMIN, ui.chkAllowSysTrayMin->isChecked(), IDS_ALLOWSYSTRAYMIN_VAL);
    //QString langCode = ui.cboLanguage->itemData(ui.cboLanguage->currentIndex(), Qt::UserRole).toString();
    //pSettings->setValue(IDS_LANGUAGE, langCode, IDS_LANGUAGE_VAL);

    pSettings->setValue(IDS_USERNAME, ui.txtUserName->text(), IDS_USERNAME_VAL);
    pSettings->setValue(IDS_USERFIRSTNAME, ui.txtFirstName->text(), IDS_USERFIRSTNAME_VAL);
    pSettings->setValue(IDS_USERLASTNAME, ui.txtLastName->text(), IDS_USERLASTNAME_VAL);
    pSettings->setValue(IDS_USERABOUT, ui.txtAbout->toPlainText(), IDS_USERABOUT_VAL);
    pSettings->setValue(IDS_REFRESHTIME, ui.spnRefreshTime->value(), IDS_REFRESHTIME_VAL);

    pSettings->setValue(IDS_MESSAGETOP, ui.rdbMessageTop->isChecked(), IDS_MESSAGETOP_VAL);
    //pSettings->setValue(IDS_PUBMESSAGEPOP, ui.chkPublicMessagePop->isChecked(), IDS_PUBMESSAGEPOP_VAL);
    pSettings->setValue(IDS_EMOTICON, ui.chkEmoticon->isChecked(), IDS_EMOTICON_VAL);
    pSettings->setValue(IDS_MESSAGETIME, ui.chkMessageTime->isChecked(), IDS_MESSAGETIME_VAL);
    pSettings->setValue(IDS_MESSAGEDATE, ui.chkMessageDate->isChecked(), IDS_MESSAGEDATE_VAL);
    pSettings->setValue(IDS_ALLOWLINKS, ui.chkAllowLinks->isChecked(), IDS_ALLOWLINKS_VAL);
    pSettings->setValue(IDS_PATHTOLINK, ui.chkPathToLink->isChecked(), IDS_PATHTOLINK_VAL);
    pSettings->setValue(IDS_TRIMMESSAGE, ui.chkTrimMessage->isChecked(), IDS_TRIMMESSAGE_VAL);
    pSettings->setValue(IDS_CLEARONCLOSE, ui.chkClearOnClose->isChecked(), IDS_CLEARONCLOSE_VAL);

    pSettings->setValue(IDS_TIMEOUT, ui.spnTimeout->value(), IDS_TIMEOUT_VAL);
    pSettings->setValue(IDS_MAXRETRIES, ui.spnMaxRetries->value(), IDS_MAXRETRIES_VAL);
    //	If any broadcast address is specified, settings written to settings file
    //	Otherwise, the entire group is removed from the settings file
    if(ui.lvBroadcasts->count() > 0) {
        pSettings->beginWriteArray(IDS_BROADCASTHDR);
        for(int index = 0; index < ui.lvBroadcasts->count(); index++) {
            pSettings->setArrayIndex(index);
            pSettings->setValue(IDS_BROADCAST, ui.lvBroadcasts->item(index)->text());
        }
        pSettings->endArray();
    }
    if(ui.lvBroadcasts->count() == 0){
        pSettings->beginGroup(IDS_BROADCASTHDR);
        pSettings->remove("");
        pSettings->endGroup();
    }
    pSettings->setValue(IDS_MULTICAST, ui.txtMulticast->text(), IDS_MULTICAST_VAL);
    pSettings->setValue(IDS_UDPPORT, ui.txtUDPPort->text(), IDS_UDPPORT_VAL);
    pSettings->setValue(IDS_TCPPORT, ui.txtTCPPort->text(), IDS_TCPPORT_VAL);

    //if(pSettings->value(IDS_AVATAR, IDS_AVATAR_VAL).toInt() != nAvatar)
        pSettings->setValue(IDS_AVATAR, nAvatar, IDS_AVATAR_VAL);

    pSettings->sync();
}
