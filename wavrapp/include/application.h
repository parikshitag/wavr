#ifndef APPLICATION_H
#define APPLICATION_H

#include <qtsingleapplication.h>

class QT_QTSINGLEAPPLICATION_EXPORT Application : public QtSingleApplication {
public:
    explicit Application(const QString& id, int& argc, char** argv);
    ~Application(void);
};

#endif // APPLICATION_H
