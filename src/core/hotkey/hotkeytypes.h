#ifndef HOTKEYTYPES_H
#define HOTKEYTYPES_H

namespace sb::hotkey {

using HotkeyHandle = unsigned int;
static constexpr HotkeyHandle InvalidHotkeyHandle =
    static_cast<HotkeyHandle>(-1);

} // namespace sb::hotkey

#endif // HOTKEYTYPES_H
