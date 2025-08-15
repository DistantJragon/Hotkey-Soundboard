#ifndef HOTKEY_H
#define HOTKEY_H

#include "core/hotkey/hotkeytypes.h"
#include <functional>
#include <string>

namespace sb::hotkey {

enum class Modifier {
  None = 0,
  Shift = 1 << 0,
  Control = 1 << 1,
  Alt = 1 << 2,
  Meta = 1 << 3
};

class Hotkey {
public:
  Hotkey() = default;
  Hotkey(const HotkeyHandle& handle) : handle(handle) {}
  Hotkey(const Hotkey& other, HotkeyHandle handle)
      : humanReadable(other.humanReadable),
        portableModifiers(other.portableModifiers),
        portableKey(other.portableKey), nativeModifiers(other.nativeModifiers),
        nativeKey(other.nativeKey), callback(other.callback),
        userData(other.userData), handle(handle) {}

  std::string humanReadable;

  unsigned long portableModifiers = static_cast<unsigned int>(Modifier::None);
  unsigned long portableKey = 0;

  unsigned long nativeModifiers = 0;
  unsigned long nativeKey = 0;

  std::function<void(void*)> callback = nullptr;
  void* userData = nullptr;

  const HotkeyHandle handle = InvalidHotkeyHandle;
};

} // namespace sb::hotkey

#endif // HOTKEY_H
