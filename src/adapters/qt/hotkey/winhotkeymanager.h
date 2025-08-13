#ifndef WINHOTKEYMANAGER_H
#define WINHOTKEYMANAGER_H

#include "core/hotkey/ihotkeymanager.h"
#include <QAbstractNativeEventFilter>
#include <QCoreApplication>
#include <QObject>

namespace sb::adapters::qt {

class WinHotkeyManager : public QObject,
                         public sb::hotkey::IHotkeyManager,
                         public QAbstractNativeEventFilter {
public:
  WinHotkeyManager(QObject* parent = nullptr);
  ~WinHotkeyManager() override;

  hotkey::HotkeyHandle registerHotkey(const hotkey::Hotkey& hotkey) override;

  void unregisterHotkey(hotkey::HotkeyHandle handle) override;

  void unregisterAllHotkeys() override;

  void manualTriggerHotkey(hotkey::HotkeyHandle handle) override;

  bool isValidHotkey(hotkey::HotkeyHandle handle) override;

  hotkey::HotkeyHandle setUserData(hotkey::HotkeyHandle handle,
                                   void* userData) override;

  hotkey::HotkeyHandle getNextHandle() override { return nextHandle++; }

  bool nativeEventFilter(const QByteArray& eventType, void* message,
                         qintptr* result) override;

private:
  std::unordered_map<hotkey::HotkeyHandle, hotkey::Hotkey> hotkeys;
  hotkey::HotkeyHandle nextHandle = 0;
};

} // namespace sb::adapters::qt

#endif // WINHOTKEYMANAGER_H
