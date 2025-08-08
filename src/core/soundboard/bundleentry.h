#ifndef BUNDLEENTRY_H
#define BUNDLEENTRY_H

#include "core/audio/audiotypes.h"
#include "core/soundboard/containerentry.h"
#include <vector>

namespace sb {

class BundleEntry : public ContainerEntry {
public:
  BundleEntry(EntryHandle handle);
  BundleEntry(EntryHandle handle, EntryHandle parentHandle);
  audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine) override;

  void addChild(size_t index, PlayableEntry* entry) override;
  void setChildWeight(size_t index, unsigned int weight) override;
  void removeChild(size_t index) override;
  void removeChild(PlayableEntry* entry) override;

  /*! \brief Rotates entries within the bundle.
   *
   * This function moves entries [firstIndex, middleIndex) to lastIndex and
   * [middleIndex, lastIndex) to firstIndex.]
   * \param firstIndex The starting index of the range to slide.
   * \param middleIndex The index at which the entries will be split.
   * \param lastIndex The index where the first part will be moved to.
   */
  void rotateEntries(size_t firstIndex, size_t middleIndex, size_t lastIndex);

  const std::vector<PlayableEntry*> getEntries() const { return children; }
  bool isRandomPlay() const { return randomPlay; }
  unsigned int getCategory() const { return category; }
  unsigned int getWeightSum() const { return weightSum; }
  unsigned int getOrderTracker() const { return orderTracker; }
  void setRandomPlay(bool random) { randomPlay = random; }

private:
  unsigned int category = 0;
  unsigned int weightSum = 0;
  unsigned int orderTracker = 0;
  bool randomPlay = true;

  audio::SoundHandle playRandom(std::mt19937& randomEngine);
  audio::SoundHandle playOrdered(std::mt19937& randomEngine);
};

} // namespace sb

#endif // BUNDLEENTRY_H
