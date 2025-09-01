#include "core/soundboard/soundfileentry.h"
#include "core/audio/audiotypes.h"

namespace sb {

SoundFileEntry::SoundFileEntry(EntryHandle handle, EntryHandle parentHandle,
                               audio::SoundHandle soundHandle,
                               const std::string& path)
    : PlayableEntry(Type::SoundFile, handle, parentHandle),
      soundHandle(soundHandle) {
  if (parentHandle != InvalidEntryHandle && soundHandle != audio::ErrorSound &&
      handle != InvalidEntryHandle) {
    this->valid = true;
  }
  this->setPath(path);
  this->setName(path);
}

audio::SoundHandle SoundFileEntry::getHandleToPlay(std::mt19937& randomEngine) {
  return soundHandle;
}
} // namespace sb
