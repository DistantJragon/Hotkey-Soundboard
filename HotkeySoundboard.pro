QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    directoryentry.cpp \
    flowlayout.cpp \
    hotkeymanager.cpp \
    main.cpp \
    hotkeysoundboard.cpp \
    playableentry.cpp \
    soundboard.cpp \
    soundfileentry.cpp \
    soundgroup.cpp \
    soundgroupwidget.cpp

HEADERS += \
    directoryentry.h \
    flowlayout.h \
    hotkey.h \
    hotkeymanager.h \
    hotkeysoundboard.h \
    playableentry.h \
    soundboard.h \
    soundfileentry.h \
    soundgroup.h \
    soundgroupwidget.h

FORMS += \
    hotkeysoundboard.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
