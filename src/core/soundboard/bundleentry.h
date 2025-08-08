#ifndef BUNDLEENTRY_H
#define BUNDLEENTRY_H

#include "core/audio/audiotypes.h"
#include "core/soundboard/playableentry.h"
#include <memory>
#include <vector>

namespace sb {

class BundleEntry : public PlayableEntry {
public:
  BundleEntry();
  BundleEntry(const std::string& path);
  audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine) override;

  /*! \brief Slides entries within the bundle.
   *
   * This function moves entries [firstIndex, middleIndex) to lastIndex and
   * [middleIndex, lastIndex) to firstIndex.]
   * \param firstIndex The starting index of the range to slide.
   * \param middleIndex The index at which the entries will be split.
   * \param lastIndex The index where the first part will be moved to.
   */
  void rotateEntries(size_t firstIndex, size_t middleIndex, size_t lastIndex);

  void setChildWeight(size_t index, unsigned int weight) {
    if (index < entries.size()) {
      weightSum += weight - entries[index]->getWeight();
      entries[index]->setWeight(weight);
    }
  }

  const std::vector<std::unique_ptr<PlayableEntry>>& getEntries() const {
    return entries;
  }
  bool isRandomPlay() const { return randomPlay; }
  unsigned int getCategory() const { return category; }
  unsigned int getWeightSum() const { return weightSum; }
  unsigned int getOrderTracker() const { return orderTracker; }
  void setRandomPlay(bool random) { randomPlay = random; }

private:
  std::vector<std::unique_ptr<PlayableEntry>> entries;
  unsigned int category = 0;
  unsigned int weightSum = 0;
  unsigned int orderTracker = 0;
  bool randomPlay = true;

  audio::SoundHandle playRandom(std::mt19937& randomEngine);
  audio::SoundHandle playOrdered(std::mt19937& randomEngine);
};

} // namespace sb

#endif // BUNDLEENTRY_H
