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
  Hotkey(HotkeyHandle handle) : handle(handle) {}
  Hotkey(const Hotkey& other, HotkeyHandle handle) : Hotkey(other) {
    this->handle = handle;
  }

  void setKeyData(const Hotkey& other) {
    humanReadable = other.humanReadable;
    portableModifiers = other.portableModifiers;
    portableKey = other.portableKey;
    nativeModifiers = other.nativeModifiers;
    nativeVirtualKey = other.nativeVirtualKey;
    nativeScanCode = other.nativeScanCode;
  }

  bool portableEquals(const Hotkey& other) const {
    return portableModifiers == other.portableModifiers &&
           portableKey == other.portableKey;
  }

  bool nativeEquals(const Hotkey& other) const {
    return nativeModifiers == other.nativeModifiers &&
           nativeVirtualKey == other.nativeVirtualKey &&
           nativeScanCode == other.nativeScanCode;
  }

  std::string humanReadable;

  unsigned long portableModifiers = static_cast<unsigned int>(Modifier::None);
  unsigned long portableKey = 0;

  unsigned long nativeModifiers = 0;
  unsigned long nativeVirtualKey = 0;
  unsigned long nativeScanCode = 0;

  std::function<void(void*)> callback = nullptr;
  void* userData = nullptr;

  HotkeyHandle handle = InvalidHotkeyHandle;
  bool active = false;
};

} // namespace sb::hotkey

#endif // HOTKEY_H
