#ifndef DIRECTORYENTRY_H
#define DIRECTORYENTRY_H

#include "core/soundboard/playableentry.h"

class DirectoryEntry : public PlayableEntry {
public:
  DirectoryEntry();
  void play() override;
};

#endif // DIRECTORYENTRY_H
