/******************************************************************************
 *
 * This file is part of SheepMusic.
 * Copyright (C) 2025 Gideon van der Kolf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCoreApplication>
#include <QSettings>

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
