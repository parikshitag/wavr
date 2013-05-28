#ifndef STDLOCATION_H
#define STDLOCATION_H

#include <QStandardPaths>

#define SL_AVATARFILE			"avt_local.png"

class StdLocation {
public:
    static QString avatarFile(void) {
        return QDir::toNativeSeparators(QStandardPaths::writableLocation(
            QStandardPaths::DataLocation) + "/"SL_AVATARFILE);
    }
};

#endif // STDLOCATION_H
