#include "core/soundboard/playableentry.h"

namespace sb {

PlayableEntry::PlayableEntry(Type type, EntryHandle handle,
                             EntryHandle parentHandle)
    : type(type), handle(handle), parentHandle(parentHandle) {}

bool PlayableEntry::isContainerType(Type type) {
  return (static_cast<int>(type) & (1 << 2)) != 0;
}

} // namespace sb
