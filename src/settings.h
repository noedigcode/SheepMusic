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
#include <QKeySequence>
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

    QStringList serializeKeySequences(QList<QKeySequence> keySequences)
    {
        QStringList ret;
        foreach (QKeySequence s, keySequences) {
            ret.append(s.toString(QKeySequence::PortableText));
        }
        return ret;
    }
    QList<QKeySequence> unserializeKeySequences(QStringList serialized)
    {
        QList<QKeySequence> ret;
        foreach (QString t, serialized) {
            ret.append(QKeySequence(t, QKeySequence::PortableText));
        }
        return ret;
    }

    Setting lastSession {"lastSession", ""};
    Setting fullscreen {"fullscreen", false};
    Setting iconsHorizontalSize {"iconsHorizontalSize", 54};
    Setting iconsVerticalSize {"iconsVerticalSize", 32};
    Setting shortcutPrevPage {"shortcutPrevPage", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_Left),
                        QKeySequence(Qt::Key_PageUp),
                        QKeySequence(Qt::Key_Backspace)
                    })};
    Setting shortcutNextPage {"shortcutNextPage", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_Right),
                        QKeySequence(Qt::Key_PageDown),
                        QKeySequence(Qt::Key_Space)
                    })};
    Setting shortcutFirstPage {"shortcutFirstPage", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_Home),
                        QKeySequence(Qt::Key_1)
                    })};
    Setting shortcutLastPage {"shortcutLastPage", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_End)
                    })};
    Setting shortcutNextDoc {"shortuctNextDoc", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_Down),
                        QKeySequence(Qt::Key_N)
                    })};
    Setting shortcutPrevDoc {"shortcutPrevDoc", serializeKeySequences(
                    {
                        QKeySequence(Qt::Key_Up),
                        QKeySequence(Qt::Key_P)
                    })};

};

#endif // SETTINGS_H
