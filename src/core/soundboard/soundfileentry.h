#ifndef SOUNDFILEENTRY_H
#define SOUNDFILEENTRY_H

#include "core/soundboard/playableentry.h"

class SoundFileEntry : public PlayableEntry {
public:
  SoundFileEntry();
  void play() override;
};

#endif // SOUNDFILEENTRY_H
