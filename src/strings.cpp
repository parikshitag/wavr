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


#include "strings.h"

QString wavrStrings::m_appName;
QStringList wavrStrings::m_statusDesc;
QStringList wavrStrings::m_userListView;

wavrStrings::wavrStrings(void) {
}

wavrStrings::~wavrStrings(void){
}

void wavrStrings::retranslate(void) {
    m_appName.clear();
    //m_appDesc.clear();
    //m_autoConn.clear();
    //m_fontSize.clear();
    m_statusDesc.clear();
    //m_soundDesc.clear();
    //m_awayTimeDesc.clear();
    m_userListView.clear();
}

const QString wavrStrings::appName(void) {
    if(m_appName.isEmpty())
        m_appName = tr("Wavr");
    return m_appName;
}

const QStringList wavrStrings::statusDesc(void) {
    if(m_statusDesc.isEmpty()) {
        m_statusDesc.append(tr("Available"));
        m_statusDesc.append(tr("Busy"));
        m_statusDesc.append(tr("Away"));
        m_statusDesc.append(tr("Extended Away"));
        m_statusDesc.append(tr("Invisible"));
        m_statusDesc.append(tr("Offline"));
    }
    return m_statusDesc;
}

const QStringList wavrStrings::userListView(void) {
    if(m_userListView.isEmpty()) {
        m_userListView.append(tr("Detailed"));
        m_userListView.append(tr("Compact"));
    }
    return m_userListView;
}
