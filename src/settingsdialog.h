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



#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMenu>
#include <QFileDialog>
#include "ui_settingsdialog.h"
#include "uidefinitions.h"
#include "libWavr/settings.h"
#include "libWavr/stdlocation.h"
#include "imagepickeraction.h"

class wavrSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit wavrSettingsDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~wavrSettingsDialog(void);

    void init(void);

private slots:
    void lvCategories_currentRowChanged(int currentRow);
    void btnOk_clicked(void);
    void chkMessageTime_toggled(bool checked);
    void chkAllowLinks_toggled(bool checked);
    void btnReset_clicked(void);

    void lvBroadcasts_currentRowChanged(int index);
    void txtBroadcast_textEdited(const QString& text);
    void btnAddBroadcast_clicked(void);
    void btnDeleteBroadcast_clicked(void);
    void avatarAction_triggered(void);
    void avatarBrowseAction_triggered(void);

private:
    void setAvatar(QString fileName = QString());
    void createAvatarMenu(void);
    void setPageHeaderStyle(QLabel* pLabel);
    void setUIText(void);
    void loadSettings(void);
    void saveSettings(void);

    int nAvatar;
    Ui::SettingsDialog  ui;
    wavrSettings*       pSettings;
    QIntValidator*      pPortValidator;
    QRegExp             ipRegExp;
    QRegExpValidator*   pIpValidator;
    QMenu* pAvatarMenu;
    QAction* avatarBrowseAction;
};

#endif // SETTINGSDIALOG_H
