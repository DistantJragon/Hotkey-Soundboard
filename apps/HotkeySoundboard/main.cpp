#include "ui/soundboard/hotkeysoundboard.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("DistantJragon");
  QCoreApplication::setOrganizationDomain("distantjragon.net");
  QCoreApplication::setApplicationName("Hotkey Soundboard");
#ifndef HKSBNDEBUG
  QSettings::setDefaultFormat(QSettings::IniFormat);
#endif
#ifdef HKSB_VERSION
  QSettings settings;
  settings.setValue("appVersion", HKSB_VERSION);
#else
  QSettings settings;
  settings.setValue("appVersion", "0.0.0-dev");
#endif
  HotkeySoundboard w;
  w.show();
  return a.exec();
}
