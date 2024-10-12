/* GidFile
 *
 * Safe writing of file to prevent data loss during crashes or full disks.
 *
 * G. van der Kolf, September 2024
 *
 * Provides a safe write function that first writes to a temporary file, renames
 * the original file to a backup file, and finally renames the temporary file to
 * the original file. If an error occurs at any time (i.e. computer crash or
 * disk full), the original file will remain in tact, either at its original
 * file name or as the backup file. Errors should not result in the original
 * file containing partially written new data.
 *
 * A read function is also provided that attempts to read the specified file and
 * falls back to the backup file if the specified file does not exist.
 *
 * This basically does what QSaveFile is supposed to do. However, a bug has been
 * found in QSaveFile where it could result in data loss of the original file if
 * the disk is full.
 * See: https://bugreports.qt.io/browse/QTBUG-75077
 */

#ifndef GIDFILE_H
#define GIDFILE_H

#include <QByteArray>
#include <QString>


class GidFile
{
public:
    // Suffix added to the original file name to create the temporary file
    static const QString newSuffix;
    // Suffix added to the original file name to create the backup file
    static const QString oldSuffix;

    struct Result {
        QString filename;
        bool success = false;
        QString errorString;
    };

    struct ReadResult {
        Result result;
        QByteArray data;
    };

    static Result write(QString filename, QByteArray data);
    static ReadResult read(QString filename);
};

#endif // GIDFILE_H
