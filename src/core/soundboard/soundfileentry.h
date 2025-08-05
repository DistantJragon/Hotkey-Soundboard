#ifndef SOUNDFILEENTRY_H
#define SOUNDFILEENTRY_H

#include "core/soundboard/playableentry.h"

namespace sb {

class SoundFileEntry : public PlayableEntry {
public:
  SoundFileEntry();
  void play(std::mt19937& randomEngine) override;
};

} // namespace sb

#endif // SOUNDFILEENTRY_H
