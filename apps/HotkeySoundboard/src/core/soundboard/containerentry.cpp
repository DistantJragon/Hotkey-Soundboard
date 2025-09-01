#include "containerentry.h"

namespace sb {

ContainerEntry::ContainerEntry(Type type, EntryHandle handle,
                               EntryHandle parentHandle)
    : PlayableEntry(type, handle, parentHandle) {
  valid = true;
}

} // namespace sb
