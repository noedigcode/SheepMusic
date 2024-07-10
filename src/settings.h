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

        void setDefaultValue(QVariant value)
        {
            mDefaultValue = value;
        }

    private:
        QString mKey;
        QVariant mDefaultValue;
    };

    // =========================================================================
    // Setting objects

    Setting lastSession {"lastSession", ""};
    Setting fullscreen {"fullscreen", false};
    Setting iconsHorizontalSize {"iconsHorizontalSize", 54};
    Setting iconsVerticalSize {"iconsVerticalSize", 32};
};

#endif // SETTINGS_H
