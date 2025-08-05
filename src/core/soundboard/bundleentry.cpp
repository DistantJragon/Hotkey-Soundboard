#include "core/soundboard/bundleentry.h"

namespace sb {

BundleEntry::BundleEntry() : PlayableEntry(Type::Bundle) {}
void BundleEntry::play(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return;
  }
  if (randomPlay) {
    playRandom(randomEngine);
  } else {
    playOrdered(randomEngine);
  }
}

void BundleEntry::playRandom(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return;
  }
  std::uniform_int_distribution<unsigned int> dist(0, weightSum - 1);
  unsigned int randomValue = dist(randomEngine);
  unsigned int cumulativeWeight = 0;
  for (const auto& entry : entries) {
    cumulativeWeight += entry->getWeight();
    if (randomValue < cumulativeWeight) {
      entry->play(randomEngine);
      break;
    }
  }
}

void BundleEntry::playOrdered(std::mt19937& randomEngine) {
  if (entries.empty()) {
    return;
  }
  if (orderTracker >= entries.size()) {
    orderTracker = 0; // Reset to the beginning
  }
  entries[orderTracker]->play(randomEngine);
  orderTracker++;
}

} // namespace sb
