#include "adapters/qt/hotkey/dbushotkeymanager.h"

namespace sb::adapters::qt {

DBusHotkeyManager::DBusHotkeyManager(QObject* parent)
    : QObject(parent), QAbstractNativeEventFilter() {
  QCoreApplication::instance()->installNativeEventFilter(this);
}

DBusHotkeyManager::~DBusHotkeyManager() {
  unregisterAllHotkeys();
  QCoreApplication::instance()->removeNativeEventFilter(this);
}

hotkey::HotkeyHandle
DBusHotkeyManager::registerHotkey(const hotkey::Hotkey& hotkey) {
  if (hotkey.nativeVirtualKey == 0) {
    return hotkey::InvalidHotkeyHandle;
  }

  if (nextHandle == hotkey::InvalidHotkeyHandle) {
    return hotkey::InvalidHotkeyHandle;
  }

  // RegisterHotKey reserves handles past 0xBFFF for shared DLLs
  if (nextHandle > static_cast<hotkey::HotkeyHandle>(0xBFFF)) {
    return hotkey::InvalidHotkeyHandle;
  }
  hotkey::HotkeyHandle h = nextHandle;

  auto result = hotkeys.try_emplace(h, hotkey, h);

  if (!result.second) {
    return hotkey::InvalidHotkeyHandle;
  }

  // if (!RegisterHotKey(nullptr, h, hotkey.nativeModifiers,
  //                     hotkey.nativeVirtualKey)) {
  //   hotkeys.erase(result.first);
  //   return hotkey::InvalidHotkeyHandle;
  // }

  result.first->second.active = true;
  nextHandle++;
  return h;
}

void DBusHotkeyManager::unregisterHotkey(hotkey::HotkeyHandle handle) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return;
  // UnregisterHotKey(nullptr, handle);
  hotkeys.erase(it);
}

void DBusHotkeyManager::unregisterAllHotkeys() {
  for (const auto& pair : hotkeys) {
    // UnregisterHotKey(nullptr, pair.first);
  }
  hotkeys.clear();
  nextHandle = 0;
}

void DBusHotkeyManager::activateHotkey(hotkey::HotkeyHandle handle) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return;

  if (it->second.active)
    return;

  // if (RegisterHotKey(nullptr, handle, it->second.nativeModifiers,
  //                    it->second.nativeVirtualKey)) {
  //   it->second.active = true;
  // }
}

void DBusHotkeyManager::deactivateHotkey(hotkey::HotkeyHandle handle) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return;

  if (!it->second.active)
    return;

  // UnregisterHotKey(nullptr, handle);
  it->second.active = false;
}

void DBusHotkeyManager::activateAllHotkeys() {
  for (auto& pair : hotkeys) {
    if (!pair.second.active) {
      // if (RegisterHotKey(nullptr, pair.first, pair.second.nativeModifiers,
      //                    pair.second.nativeVirtualKey)) {
      //   pair.second.active = true;
      // }
    }
  }
}

void DBusHotkeyManager::deactivateAllHotkeys() {
  for (auto& pair : hotkeys) {
    // if (pair.second.active) {
    //   UnregisterHotKey(nullptr, pair.first);
    //   pair.second.active = false;
    // }
  }
}

void DBusHotkeyManager::manualTriggerHotkey(hotkey::HotkeyHandle handle) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return;

  auto cb = it->second.callback;
  auto userData = it->second.userData;

  QMetaObject::invokeMethod(
      this,
      [cb, userData]() {
        if (cb) {
          cb(userData);
        }
      },
      Qt::QueuedConnection);
}

const hotkey::Hotkey*
DBusHotkeyManager::getHotkey(hotkey::HotkeyHandle handle) const {
  auto it = hotkeys.find(handle);
  if (it != hotkeys.end()) {
    return &it->second;
  }
  return nullptr;
}

bool DBusHotkeyManager::nativeEventFilter(const QByteArray& eventType,
                                         void* message, qintptr* result) {
  // MSG* msg = static_cast<MSG*>(message);
  // qDebug("Detected msg: %u", msg->message);
  // if (msg->message == WM_HOTKEY) {
  //   int winId = (int)msg->wParam;
  //   qDebug("Hotkey pressed: %d", winId);
  //   auto it = hotkeys.find(winId);
  //   if (it != hotkeys.end()) {
  //     auto hotkey = it->second;
  //     QMetaObject::invokeMethod(
  //         this,
  //         [hotkey]() {
  //           if (hotkey.callback) {
  //             qDebug("Running hotkey %s, %d", hotkey.humanReadable.c_str(),
  //                    hotkey.handle);
  //             hotkey.callback(hotkey.userData);
  //           }
  //         },
  //         Qt::QueuedConnection);
  //   }
  // }
  return false;
}

bool DBusHotkeyManager::isValidHotkey(hotkey::HotkeyHandle handle) {
  return hotkeys.find(handle) != hotkeys.end();
}

hotkey::HotkeyHandle DBusHotkeyManager::setUserData(hotkey::HotkeyHandle handle,
                                                   void* userData) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return hotkey::InvalidHotkeyHandle;
  it->second.userData = userData;
  return handle;
}

} // namespace sb::adapters::qt
