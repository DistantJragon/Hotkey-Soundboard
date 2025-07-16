#include "core/soundboard.h"
#include "exceptions/exceptions.h"

Soundboard::Soundboard() {}

SoundGroup* Soundboard::newSoundGroup(const std::string& name) {
  auto result = soundGroups.try_emplace(
      nextId, std::make_unique<SoundGroup>(hotkeyManager, nextId, name));
  if (!result.second) {
    throw SbExceptions::SoundGroupIdExists(nextId);
  }
  nextId++;
  return result.first->second.get();
}

void Soundboard::renameSoundGroup(SoundGroup* soundGroup,
                                  const std::string& newName) {
  if (!soundGroup) {
    throw SbExceptions::SoundGroupDoesNotExist();
  }
  soundGroup->setName(newName);
}

void Soundboard::deleteSoundGroup(SoundGroup*& soundGroup) {
  if (!soundGroup) {
    throw SbExceptions::SoundGroupDoesNotExist();
  }
  int id = soundGroup->getId();
  soundGroups.erase(id);
  soundGroup = nullptr;
}
