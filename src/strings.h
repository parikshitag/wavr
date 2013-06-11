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


#ifndef STRINGS_H
#define STRINGS_H

#include <QStringList>
#include <QCoreApplication>

class wavrStrings
{
    Q_DECLARE_TR_FUNCTIONS(wavrStrings)

public:
    wavrStrings(void);
    ~wavrStrings(void);

    static const QString appName(void);
    static const QStringList statusDesc(void);

private:
    static QString m_appName;
    static QStringList m_statusDesc;
};

#endif // STRINGS_H
