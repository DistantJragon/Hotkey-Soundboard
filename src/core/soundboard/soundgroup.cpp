#include "core/soundboard/soundgroup.h"

SoundGroup::SoundGroup(IHotkeyManager& hkManager, const int id,
                       const std::string& name)
    : hotkeyManager(hkManager), name(name), id(id) {}

// TODO: Implement the destructor. Need to deregister hotkeys once they are
// implemented.
