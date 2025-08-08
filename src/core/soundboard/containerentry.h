#ifndef CONTAINERENTRY_H
#define CONTAINERENTRY_H

#include "core/soundboard/soundboardtypes.h"
#include "playableentry.h"

namespace sb {

class ContainerEntry : public PlayableEntry {
public:
  ContainerEntry(Type type, EntryHandle handle, EntryHandle parentHandle);

  virtual void addChild(size_t index, PlayableEntry* entry) = 0;
  virtual void setChildWeight(size_t index, unsigned int weight) = 0;
  virtual void removeChild(size_t index) = 0;
  virtual void removeChild(PlayableEntry* entry) = 0;

protected:
  std::vector<PlayableEntry*> children;
};

} // namespace sb

#endif // CONTAINERENTRY_H
