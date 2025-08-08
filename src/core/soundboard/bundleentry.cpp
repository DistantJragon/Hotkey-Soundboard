#include "core/soundboard/bundleentry.h"
#include "core/audio/audiotypes.h"
#include <algorithm>

namespace sb {

BundleEntry::BundleEntry() : PlayableEntry(Type::Bundle, "") {};
BundleEntry::BundleEntry(const std::string& path)
    : PlayableEntry(Type::Bundle, path) {}

audio::SoundHandle BundleEntry::getHandleToPlay(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return audio::ErrorSound;
  }
  if (randomPlay) {
    return playRandom(randomEngine);
  } else {
    return playOrdered(randomEngine);
  }
}

void BundleEntry::rotateEntries(size_t firstIndex, size_t middleIndex,
                                size_t lastIndex) {
  if (lastIndex > entries.size() || middleIndex >= lastIndex ||
      firstIndex >= middleIndex) {
    return;
  }
  std::rotate(entries.begin() + firstIndex, entries.begin() + middleIndex,
              entries.begin() + lastIndex);
}

audio::SoundHandle BundleEntry::playRandom(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return audio::ErrorSound;
  }
  std::uniform_int_distribution<unsigned int> dist(0, weightSum - 1);
  unsigned int randomValue = dist(randomEngine);
  unsigned int cumulativeWeight = 0;
  for (const auto& entry : entries) {
    cumulativeWeight += entry->getWeight();
    if (randomValue < cumulativeWeight) {
      return entry->getHandleToPlay(randomEngine);
      break;
    }
  }
  return audio::ErrorSound;
}

audio::SoundHandle BundleEntry::playOrdered(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return audio::ErrorSound;
  }
  if (orderTracker >= entries.size()) {
    orderTracker = 0;
  }
  audio::SoundHandle h = entries[orderTracker]->getHandleToPlay(randomEngine);
  orderTracker++;
  return h;
}

} // namespace sb
