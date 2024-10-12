QT       += core gui pdf

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/breadcrumbswidget.cpp \
    src/drawcurve.cpp \
    src/gidfile.cpp \
    src/graphicsview.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/pagescene.cpp

HEADERS += \
    src/breadcrumbswidget.h \
    src/drawcurve.h \
    src/gidfile.h \
    src/graphicsview.h \
    src/mainwindow.h \
    src/pagescene.h \
    src/settings.h \
    src/version.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images/images.qrc
