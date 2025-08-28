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
    src/adapters/qt/audio/basicaudioengine.cpp \
    src/core/soundboard/bundleentry.cpp \
    src/core/soundboard/containerentry.cpp \
    src/core/soundboard/playableentry.cpp \
    src/core/soundboard/soundboard.cpp \
    src/core/soundboard/soundfileentry.cpp \
    src/persistence/configmanager.cpp \
    src/ui/clickablelabel.cpp \
    src/ui/flowlayout.cpp \
    src/ui/hotkey/actionchooserdialog.cpp \
    src/ui/hotkey/actiondelegate.cpp \
    src/ui/hotkey/categorychooserdialog.cpp \
    src/ui/hotkey/categorydelegate.cpp \
    src/ui/hotkey/hotkeycapturedialog.cpp \
    src/ui/hotkey/hotkeydelegate.cpp \
    src/ui/hotkey/hotkeyeditbutton.cpp \
    src/ui/hotkey/hotkeymanagerdialog.cpp \
    src/ui/hotkey/hotkeytablemodel.cpp \
    src/ui/hotkey/platkeysequenceedit.cpp \
    src/ui/soundboard/hotkeysoundboard.cpp \
    src/ui/soundboard/playableentryframe.cpp \
    src/ui/soundboard/renamerootbundledialog.cpp \
    src/ui/soundboard/rootbundleframe.cpp \
    src/ui/soundboard/soundentrywidget.cpp \
    src/ui/soundboard/rootbundlecontrolwidget.cpp

HEADERS += \
    src/adapters/qt/audio/basicaudioengine.h \
    src/core/audio/audiotypes.h \
    src/core/audio/iaudioengine.h \
    src/core/hotkey/ihotkeymanager.h \
    src/core/soundboard/bundleentry.h \
    src/core/soundboard/containerentry.h \
    src/core/soundboard/playableentry.h \
    src/core/soundboard/soundboard.h \
    src/core/soundboard/soundfileentry.h \
    src/core/soundboard/exceptions.h \
    src/persistence/configmanager.h \
    src/ui/clickablelabel.h \
    src/ui/flowlayout.h \
    src/ui/hotkey/actionchooserdialog.h \
    src/ui/hotkey/actiondelegate.h \
    src/ui/hotkey/categorychooserdialog.h \
    src/ui/hotkey/categorydelegate.h \
    src/ui/hotkey/hotkeycapturedialog.h \
    src/ui/hotkey/hotkeydelegate.h \
    src/ui/hotkey/hotkeyeditbutton.h \
    src/ui/hotkey/hotkeyguitypes.h \
    src/ui/hotkey/hotkeymanagerdialog.h \
    src/ui/hotkey/hotkeytablemodel.h \
    src/ui/hotkey/platkeysequenceedit.h \
    src/ui/soundboard/bundledefaults.h \
    src/ui/soundboard/hotkeysoundboard.h \
    src/ui/soundboard/playableentryframe.h \
    src/ui/soundboard/renamerootbundledialog.h \
    src/ui/soundboard/rootbundleframe.h \
    src/ui/soundboard/soundentrywidget.h \
    src/ui/soundboard/rootbundlecontrolwidget.h

FORMS += \
    src/forms/actionchooserdialog.ui \
    src/forms/categorychooserdialog.ui \
    src/forms/hotkeycapturedialog.ui \
    src/forms/hotkeymanagerdialog.ui \
    src/forms/hotkeysoundboard.ui \
    src/forms/playableentryframe.ui \
    src/forms/renamerootbundledialog.ui \
    src/forms/rootbundlecontrolwidget.ui

INCLUDEPATH += \
    $$PWD/src

win32:{

LIBS += -luser32

SOURCES += \
    src/adapters/qt/hotkey/winhotkeymanager.cpp
HEADERS += \
    src/adapters/qt/hotkey/winhotkeymanager.h
}

RESOURCES += \
  resources/hotkeysoundboard.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

