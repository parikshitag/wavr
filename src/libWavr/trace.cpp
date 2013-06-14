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

#include "trace.h"
#include <QDebug>

bool wavrTrace::traceMode;
QString wavrTrace::fileName;

wavrTrace::wavrTrace(void) {
}

wavrTrace::~wavrTrace(void) {
}

void wavrTrace::init(wavrXmlMessage* pInitParams) {
    traceMode = wavrHelper::stringToBool(pInitParams->data(XML_TRACEMODE));
    fileName = pInitParams->data(XML_LOGFILE);

    write("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"\
          "         "IDA_TITLE" "IDA_VERSION" application log\n"\
          "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
}

void wavrTrace::write(const QString& string, bool verbose) {
    //if(!traceMode || !verbose)
      //  return;

    QDir logDir(StdLocation::logDir());
    if(!logDir.exists())
        logDir.mkdir(logDir.absolutePath());
    QFile file(fileName);
    if(!file.open(QIODevice::Text | QIODevice::Append))
        return;

    QTextStream stream(&file);

    QString timeStamp = "[" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + "] ";
    QStringList stringList = string.split("\n", QString::SkipEmptyParts);
    for(int index = 0; index < stringList.count(); index++)
        stream << timeStamp << stringList[index] << "\n";

    file.close();
}

