#include "adapters/qt/hotkey/winhotkeymanager.h"
#include <windows.h>

namespace sb::adapters::qt {

WinHotkeyManager::WinHotkeyManager(QObject* parent)
    : QObject(parent), QAbstractNativeEventFilter() {
  QCoreApplication::instance()->installNativeEventFilter(this);
}

WinHotkeyManager::~WinHotkeyManager() {
  unregisterAllHotkeys();
  QCoreApplication::instance()->removeNativeEventFilter(this);
}

hotkey::HotkeyHandle
WinHotkeyManager::registerHotkey(const hotkey::Hotkey& hotkey) {
  if (hotkey.nativeKey == 0) {
    return hotkey::InvalidHotkeyHandle;
  }

  if (nextHandle == hotkey::InvalidHotkeyHandle) {
    return hotkey::InvalidHotkeyHandle;
  }

  // RegisterHotKey can't accept a handle greater than 0xBFFF
  if (nextHandle >= static_cast<hotkey::HotkeyHandle>(0xBFFF)) {
    return hotkey::InvalidHotkeyHandle;
  }
  hotkey::HotkeyHandle h = nextHandle;

  auto result = hotkeys.try_emplace(h, hotkey, h);

  if (!result.second) {
    // Handle already exists, unregister the hotkey
    UnregisterHotKey(nullptr, h);
    return hotkey::InvalidHotkeyHandle;
  }

  if (!RegisterHotKey(nullptr, h, hotkey.nativeModifiers, hotkey.nativeKey)) {
    hotkeys.erase(result.first);
    return hotkey::InvalidHotkeyHandle;
  }

  nextHandle++;

  return h;
}

void WinHotkeyManager::unregisterHotkey(hotkey::HotkeyHandle handle) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return;
  UnregisterHotKey(nullptr, handle);
  hotkeys.erase(it);
}

void WinHotkeyManager::unregisterAllHotkeys() {
  for (const auto& pair : hotkeys) {
    UnregisterHotKey(nullptr, pair.first);
  }
  hotkeys.clear();
  nextHandle = 0;
}

void WinHotkeyManager::manualTriggerHotkey(hotkey::HotkeyHandle handle) {
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

bool WinHotkeyManager::nativeEventFilter(const QByteArray& eventType,
                                         void* message, qintptr* result) {
  MSG* msg = static_cast<MSG*>(message);
  if (msg->message == WM_HOTKEY) {
    int winId = (int)msg->wParam;
    auto it = hotkeys.find(winId);
    if (it != hotkeys.end()) {
      auto hotkey = it->second;
      QMetaObject::invokeMethod(
          this,
          [hotkey]() {
            if (hotkey.callback) {
              hotkey.callback(hotkey.userData);
            }
          },
          Qt::QueuedConnection);
    }
  }
  return false;
}

bool WinHotkeyManager::isValidHotkey(hotkey::HotkeyHandle handle) {
  return hotkeys.find(handle) != hotkeys.end();
}

hotkey::HotkeyHandle WinHotkeyManager::setUserData(hotkey::HotkeyHandle handle,
                                                   void* userData) {
  auto it = hotkeys.find(handle);
  if (it == hotkeys.end())
    return hotkey::InvalidHotkeyHandle;
  it->second.userData = userData;
  return handle;
}

} // namespace sb::adapters::qt
