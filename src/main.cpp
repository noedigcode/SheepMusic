#include "mainwindow.h"
#include "version.h"

#include <QApplication>

#include <iostream>

void print(QString msg)
{
    std::cout << msg.toStdString() << std::endl;
}

void printVersion()
{
    print(QString(APP_NAME));
    print(QString("Version %1").arg(APP_VERSION));
    print(QString("Gideon van der Kolf %1-%2").arg(APP_YEAR_FROM).arg(APP_YEAR));
    print("");
    print("Compiled with Qt " + QString(QT_VERSION_STR));
}

void printHelp()
{
    //    |--------------------------------------------------------------------------------|
    print("");
    print("Command-line arguments:");
    print("");
    print("help | -h | --help   Show this help message.");
    print("-v | --version       Print version info and exit.");
    print("");
    //    |--------------------------------------------------------------------------------|
}

int main(int argc, char *argv[])
{
    printVersion();

    QStringList helpArgs {"help", "-h", "--help"};
    QStringList versionArgs {"-v", "--version"};

    for (int i=1; i < argc; i++) {
        QString arg(argv[i]);
        if (helpArgs.contains(arg)) {
            printHelp();
            return 0;
        } else if (versionArgs.contains(arg)) {
            // Version info already printed at start. Just exit.
            return 0;
        } else {
            print("Unknown argument: " + arg);
        }
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
