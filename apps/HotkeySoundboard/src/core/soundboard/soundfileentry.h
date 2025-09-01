#ifndef SOUNDFILEENTRY_H
#define SOUNDFILEENTRY_H

#include "core/audio/audiotypes.h"
#include "core/soundboard/playableentry.h"

namespace sb {

class SoundFileEntry : public PlayableEntry {
public:
  SoundFileEntry(EntryHandle handle, EntryHandle parentHandle,
                 audio::SoundHandle soundHandle, const std::string& path = "");
  audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine) override;

private:
  const audio::SoundHandle soundHandle;
};

} // namespace sb

#endif // SOUNDFILEENTRY_H
