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
    qDebug() << "pleae" << fileName;

    write("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"\
          "         "IDA_TITLE" "IDA_VERSION" application log\n"\
          "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
}

void wavrTrace::write(const QString& string, bool verbose) {
    //if(!traceMode || !verbose)
      //  return;

    QDir logDir(StdLocation::logDir());
    qDebug() << "logdir " << logDir.exists() << logDir.absolutePath();
    if(!logDir.exists())
        logDir.mkdir(logDir.absolutePath());
    QFile file(fileName);
    qDebug() << "hello " << file.exists();
    if(!file.open(QIODevice::Text | QIODevice::Append))
        return;

    qDebug() << "hello1 " << file.exists();
    QTextStream stream(&file);

    QString timeStamp = "[" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + "] ";
    QStringList stringList = string.split("\n", QString::SkipEmptyParts);
    for(int index = 0; index < stringList.count(); index++)
        stream << timeStamp << stringList[index] << "\n";

    file.close();
}

