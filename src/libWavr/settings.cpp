
#include "settings.h"
#include "stdlocation.h"

wavrSettingsBase::wavrSettingsBase(void) : QSettings() {
}

wavrSettingsBase::wavrSettingsBase(const QString& fileName, Format format) :
    QSettings(fileName, format) {
}

wavrSettingsBase::wavrSettingsBase(Format format, Scope scope, const QString& organization, const QString& application) :
    QSettings(format, scope, organization, application) {
}

wavrSettingsBase::~wavrSettingsBase(void) {
}

void wavrSettingsBase::setValue(const QString& key, const QVariant& value, const QVariant& defaultValue) {
    if(value != defaultValue)
        QSettings::setValue(key, value);
    else
        remove(key);
}

void wavrSettings::setAutoStart(bool on) {
#ifdef Q_WS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    if(on)
        settings.setValue(IDA_TITLE, QDir::toNativeSeparators(QApplication::applicationFilePath()));
    else
        settings.remove(IDA_TITLE);
#endif

#ifdef Q_WS_MAC
    Q_UNUSED(on);
#endif

#ifdef Q_WS_X11
    //  get the path of .desktop file
    QString autoStartDir;
    char* buffer = getenv("XDG_CONFIG_HOME");
    if(buffer) {
        autoStartDir = QString(buffer);
        autoStartDir.append("/autostart");
    } else {
        buffer = getenv("HOME");
        autoStartDir = QString(buffer);
        autoStartDir.append("/.config/autostart");
    }
    QDir dir(autoStartDir);
    QString fileName = dir.absoluteFilePath("wavr.desktop");
    //	delete the file if autostart is set to false
    if(!on) {
        QFile::remove(fileName);
        return;
    }

    if(!dir.exists())
        dir.mkpath(dir.absolutePath());
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(false);
    stream << "[Desktop Entry]\n";
    stream << "Encoding=UTF-8\n";
    stream << "Type=Application\n";
    stream << "Name=" << IDA_TITLE << "\n";
    stream << "Comment=Send and receive instant messages\n";
    stream << "Icon=wavr\n";
    stream << "Exec=sh " << qApp->applicationDirPath() << "/wavr.sh\n";
    stream << "Terminal=false\n";
    file.close();
#endif
}
