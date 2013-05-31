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

private:
    static QString m_appName;
};

#endif // STRINGS_H
