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
    src/core/audio/iaudioengine.cpp \
    src/core/hotkey/ihotkeymanager.cpp \
    src/core/soundboard/directoryentry.cpp \
    src/core/soundboard/playableentry.cpp \
    src/core/soundboard/soundboard.cpp \
    src/core/soundboard/soundfileentry.cpp \
    src/core/soundboard/soundgroup.cpp \
    src/ui/clickablelabel.cpp \
    src/ui/flowlayout.cpp \
    src/ui/hotkeysoundboard.cpp \
    src/ui/renamesoundgroupdialog.cpp \
    src/ui/soundgroupwidget.cpp

HEADERS += \
    src/core/audio/iaudioengine.h \
    src/core/hotkey/hotkey.h \
    src/core/hotkey/ihotkeymanager.h \
    src/core/soundboard/directoryentry.h \
    src/core/soundboard/playableentry.h \
    src/core/soundboard/soundboard.h \
    src/core/soundboard/soundfileentry.h \
    src/core/soundboard/soundgroup.h \
    src/core/soundboard/exceptions.h \
    src/ui/clickablelabel.h \
    src/ui/flowlayout.h \
    src/ui/hotkeysoundboard.h \
    src/ui/renamesoundgroupdialog.h \
    src/ui/soundgroupdefaults.h \
    src/ui/soundgroupwidget.h

FORMS += \
    src/forms/hotkeysoundboard.ui \
    src/forms/renamesoundgroupdialog.ui \
    src/forms/soundgroupwidget.ui

INCLUDEPATH += \
    $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
