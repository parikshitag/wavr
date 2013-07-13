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



#ifndef THEME_H
#define THEME_H

#include "libWavr/stdlocation.h"

struct Theme {
    QString name;
    QString path;

    Theme() {}
    Theme(QString szName, QString szPath) {
        name = szName;
        path = szPath;
    }
};

typedef QList<Theme> Themes;

struct ThemeData {
    QString themePath;
    QString document;
    QString inMsg;
    QString inNextMsg;
    QString outMsg;
    QString outNextMsg;
    QString pubMsg;
    QString sysMsg;
    QString sysNextMsg;
    QString reqMsg;
    QString stateMsg;
};

class wavrTheme
{
public:
    static const Themes availableThemes(void);
    static const ThemeData loadTheme(const QString& path);
};

#endif // THEME_H
