#include "core/soundboard/soundboard.h"

Soundboard::Soundboard(sb::audio::IAudioEngine* audioEngine)
    : audioEngine(audioEngine) {}

GroupHandle Soundboard::newSoundGroup(const std::string& name) {
  if (nextId == InvalidGroupHandle) {
    return InvalidGroupHandle;
  }
  auto result = soundGroups.try_emplace(nextId, nextId, name);
  if (!result.second) {
    return InvalidGroupHandle;
  }
  nextId++;
  return result.first->first;
}

void Soundboard::renameSoundGroup(GroupHandle group,
                                  const std::string& newName) {
  if (group == InvalidGroupHandle) {
    return;
  }
  auto it = soundGroups.find(group);
  if (it == soundGroups.end()) {
    return;
  }
  it->second.setName(newName);
}

void Soundboard::deleteSoundGroup(GroupHandle group) {
  if (group == InvalidGroupHandle) {
    return;
  }
  soundGroups.erase(group);
}
bool Soundboard::isValidGroup(GroupHandle group) const {
  return soundGroups.find(group) != soundGroups.end();
}
void Soundboard::playSoundGroup(GroupHandle group) {
  if (group == InvalidGroupHandle) {
    return;
  }
  auto it = soundGroups.find(group);
  if (it != soundGroups.end()) {
    it->second.play(randomEngine);
  }
}
