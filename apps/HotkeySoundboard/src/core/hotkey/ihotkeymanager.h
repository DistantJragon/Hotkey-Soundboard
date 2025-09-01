#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include "core/hotkey/hotkey.h"
#include "core/hotkey/hotkeytypes.h"

namespace sb::hotkey {

class IHotkeyManager {
public:
  virtual ~IHotkeyManager() = default;

  virtual HotkeyHandle registerHotkey(const Hotkey& hotkey) = 0;
  virtual void unregisterHotkey(HotkeyHandle handle) = 0;
  virtual void unregisterAllHotkeys() = 0;

  virtual void activateHotkey(HotkeyHandle handle) = 0;
  virtual void deactivateHotkey(HotkeyHandle handle) = 0;
  virtual void activateAllHotkeys() = 0;
  virtual void deactivateAllHotkeys() = 0;

  virtual void manualTriggerHotkey(HotkeyHandle handle) = 0;

  virtual bool isValidHotkey(HotkeyHandle handle) = 0;

  virtual HotkeyHandle setUserData(HotkeyHandle handle, void* userData) = 0;

  virtual HotkeyHandle getNextHandle() = 0;

  virtual const Hotkey* getHotkey(HotkeyHandle handle) const = 0;
};

} // namespace sb::hotkey

#endif // HOTKEYMANAGER_H
