#include "core/soundboard/soundfileentry.h"

namespace sb {

SoundFileEntry::SoundFileEntry() : PlayableEntry(Type::SoundFile) {}

void SoundFileEntry::play(std::mt19937& randomEngine) {}
} // namespace sb
