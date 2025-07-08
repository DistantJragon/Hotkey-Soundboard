#include "src/core/soundgroup.h"

SoundGroup::SoundGroup(HotkeyManager &hkManager, const std::string &name)
    : hotkeyManager(hkManager), name(name) {}

// TODO: Implement the destructor. Need to deregister hotkeys once they are
// implemented.
