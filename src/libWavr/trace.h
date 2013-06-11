#ifndef TRACE_H
#define TRACE_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include "stdlocation.h"
#include "xmlmessage.h"

class wavrTrace
{
public:
    wavrTrace(void);
    ~wavrTrace(void);

    static void init(wavrXmlMessage* pInitParams);
    static void write(const QString& string, bool verbose = true);

private:
    static bool traceMode;
    static QString fileName;
};

#endif // TRACE_H
