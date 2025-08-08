#include "core/soundboard/playableentry.h"

namespace sb {

PlayableEntry::PlayableEntry(Type type) : type(type) {}

PlayableEntry::PlayableEntry(Type type, const std::string& path)
    : type(type), path(path) {}

} // namespace sb
