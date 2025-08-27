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

  void rotateEntries(size_t firstIndex, size_t middleIndex,
                     size_t lastIndex) override;

  const std::vector<PlayableEntry*> getEntries() const { return children; }
  bool isRandomPlay() const { return randomPlay; }
  bool isRecursive() const { return recursive; }
  unsigned int getWeightSum() const { return weightSum; }
  unsigned int getOrderTracker() const { return orderTracker; }
  void setRandomPlay(bool random) { randomPlay = random; }
  void setRecursive(bool rec) { recursive = rec; }

private:
  unsigned int weightSum = 0;
  unsigned int orderTracker = 0;
  bool randomPlay = true;
  bool recursive = false;

  audio::SoundHandle playRandom(std::mt19937& randomEngine);
  audio::SoundHandle playOrdered(std::mt19937& randomEngine);
};

} // namespace sb

#endif // BUNDLEENTRY_H
