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

  /*! \brief Rotates entries within the bundle.
   *
   * This function moves entries [firstIndex, middleIndex) to lastIndex and
   * [middleIndex, lastIndex) to firstIndex.]
   * \param firstIndex The starting index of the range to slide.
   * \param middleIndex The index at which the entries will be split.
   * \param lastIndex The index where the first part will be moved to.
   */
  virtual void rotateEntries(size_t firstIndex, size_t middleIndex,
                             size_t lastIndex) = 0;

  const std::vector<PlayableEntry*>& getChildren() const { return children; }

protected:
  std::vector<PlayableEntry*> children;
};

} // namespace sb

#endif // CONTAINERENTRY_H
