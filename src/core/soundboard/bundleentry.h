#ifndef BUNDLEENTRY_H
#define BUNDLEENTRY_H

#include "core/soundboard/playableentry.h"
#include <memory>
#include <vector>

namespace sb {

class BundleEntry : public PlayableEntry {
public:
  BundleEntry();
  void play(std::mt19937& randomEngine) override;

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

  void playRandom(std::mt19937& randomEngine);
  void playOrdered(std::mt19937& randomEngine);
};

} // namespace sb

#endif // BUNDLEENTRY_H
