#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QCoreApplication>

class Settings
{
public:

    Settings(QString org, QString domain, QString appName, QString version)
    {
        QCoreApplication::setOrganizationName(org);
        QCoreApplication::setOrganizationDomain(domain);
        QCoreApplication::setApplicationName(appName);
        QCoreApplication::setApplicationVersion(version);
    }

    class Setting
    {
    public:
        Setting(QString key, QVariant defaultValue = QVariant())
        {
            mKey = key;
            mDefaultValue = defaultValue;
        }

        QVariant value()
        {
            return QSettings().value(mKey, mDefaultValue);
        }
        QString string()
        {
            return value().toString();
        }

        void set(QVariant value)
        {
            QSettings().setValue(mKey, value);
        }

    private:
        QString mKey;
        QVariant mDefaultValue;
    };

    // =========================================================================
    // Setting objects

    Setting lastSession {"lastSession", ""};
    Setting fullscreen {"fullscreen", false};
};

#endif // SETTINGS_H
