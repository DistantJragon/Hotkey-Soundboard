#include "core/soundboard/soundfileentry.h"
#include "core/audio/audiotypes.h"

namespace sb {

SoundFileEntry::SoundFileEntry(const audio::SoundHandle& soundHandle,
                               const std::string& path)
    : PlayableEntry(Type::SoundFile, path), soundHandle(soundHandle) {
  if (soundHandle == audio::ErrorSound) {
    valid = false;
    weight = 0;
  }
}

audio::SoundHandle SoundFileEntry::getHandleToPlay(std::mt19937& randomEngine) {
  return soundHandle;
}
} // namespace sb
