QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += export_compile_commands

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

ReleaseBuild {
    DEFINES += HKSBNDEBUG
}
SOURCES += \
    main.cpp \
    src/core/directoryentry.cpp \
    src/core/hotkeymanager.cpp \
    src/core/playableentry.cpp \
    src/core/soundboard.cpp \
    src/core/soundfileentry.cpp \
    src/core/soundgroup.cpp \
    src/ui/flowlayout.cpp \
    src/ui/hotkeysoundboard.cpp \
    src/ui/soundgroupwidget.cpp

HEADERS += \
    src/core/directoryentry.h \
    src/core/hotkey.h \
    src/core/hotkeymanager.h \
    src/core/playableentry.h \
    src/core/soundboard.h \
    src/core/soundfileentry.h \
    src/core/soundgroup.h \
    src/exceptions/exceptions.h \
    src/ui/flowlayout.h \
    src/ui/hotkeysoundboard.h \
    src/ui/soundgroupwidget.h

FORMS += \
    src/forms/hotkeysoundboard.ui \
    src/forms/soundgroupwidget.ui

INCLUDEPATH += \
    $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
