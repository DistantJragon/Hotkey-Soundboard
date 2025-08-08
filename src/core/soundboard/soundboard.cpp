#include "core/soundboard/soundboard.h"
#include "core/soundboard/exceptions.h"

namespace sb {

Soundboard::Soundboard(sb::audio::IAudioEngine* audioEngine)
    : audioEngine(audioEngine) {}

GroupHandle Soundboard::newSoundGroup(const std::string& name) {
  if (nextHandle == InvalidGroupHandle) {
    return InvalidGroupHandle;
  }
  auto result = soundGroups.try_emplace(nextHandle, nextHandle, name);
  if (!result.second) {
    return InvalidGroupHandle;
  }
  nextHandle++;
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

SoundGroup& Soundboard::getSoundGroup(GroupHandle group) {
  if (group == InvalidGroupHandle) {
    throw exceptions::MissingSoundGroup(group);
  }
  auto it = soundGroups.find(group);
  if (it == soundGroups.end()) {
    throw exceptions::MissingSoundGroup(group);
  }
  return it->second;
}

void Soundboard::playSoundGroup(GroupHandle group) {
  if (!audioEngine) {
    return;
  }
  if (group == InvalidGroupHandle) {
    return;
  }
  auto it = soundGroups.find(group);
  if (it != soundGroups.end()) {
    audioEngine->play(it->second.getHandleToPlay(randomEngine));
  }
}

} // namespace sb
