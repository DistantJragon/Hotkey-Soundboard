QT       += core gui multimedia

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
    src/adapters/qt/basicaudioengine.cpp \
    src/core/soundboard/bundleentry.cpp \
    src/core/soundboard/containerentry.cpp \
    src/core/soundboard/playableentry.cpp \
    src/core/soundboard/soundboard.cpp \
    src/core/soundboard/soundfileentry.cpp \
    src/ui/clickablelabel.cpp \
    src/ui/flowlayout.cpp \
    src/ui/hotkeysoundboard.cpp \
    src/ui/playableentryframe.cpp \
    src/ui/renamerootbundledialog.cpp \
    src/ui/rootbundleframe.cpp \
    src/ui/soundentrywidget.cpp \
    src/ui/rootbundlecontrolwidget.cpp

HEADERS += \
    src/adapters/qt/basicaudioengine.h \
    src/core/audio/audiotypes.h \
    src/core/audio/iaudioengine.h \
    src/core/hotkey/ihotkeymanager.h \
    src/core/soundboard/bundleentry.h \
    src/core/soundboard/containerentry.h \
    src/core/soundboard/playableentry.h \
    src/core/soundboard/soundboard.h \
    src/core/soundboard/soundfileentry.h \
    src/core/soundboard/exceptions.h \
    src/ui/clickablelabel.h \
    src/ui/flowlayout.h \
    src/ui/hotkeysoundboard.h \
    src/ui/playableentryframe.h \
    src/ui/renamerootbundledialog.h \
    src/ui/rootbundleframe.h \
    src/ui/soundentrywidget.h \
    src/ui/soundgroupdefaults.h \
    src/ui/rootbundlecontrolwidget.h

FORMS += \
    src/forms/hotkeysoundboard.ui \
    src/forms/playableentryframe.ui \
    src/forms/renamerootbundledialog.ui \
    src/forms/rootbundlecontrolwidget.ui

INCLUDEPATH += \
    $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  hotkeysoundboard.qrc
