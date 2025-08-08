#include "core/soundboard/bundleentry.h"
#include "core/audio/audiotypes.h"
#include <algorithm>

namespace sb {

BundleEntry::BundleEntry(EntryHandle handle)
    : ContainerEntry(Type::Bundle, handle, InvalidEntryHandle) {};

BundleEntry::BundleEntry(EntryHandle handle, EntryHandle parentHandle)
    : ContainerEntry(Type::Bundle, handle, parentHandle) {}

audio::SoundHandle BundleEntry::getHandleToPlay(std::mt19937& randomEngine) {
  if (children.empty()) {
    return audio::ErrorSound;
  }
  if (randomPlay) {
    return playRandom(randomEngine);
  } else {
    return playOrdered(randomEngine);
  }
}

void BundleEntry::addChild(size_t index, PlayableEntry* entry) {
  if (index > children.size()) {
    index = children.size();
  }
  children.insert(children.begin() + index, entry);
  weightSum += entry->getWeight();
}

void BundleEntry::rotateEntries(size_t firstIndex, size_t middleIndex,
                                size_t lastIndex) {
  if (lastIndex > children.size() || middleIndex >= lastIndex ||
      firstIndex >= middleIndex) {
    return;
  }
  std::rotate(children.begin() + firstIndex, children.begin() + middleIndex,
              children.begin() + lastIndex);
}

audio::SoundHandle BundleEntry::playRandom(std::mt19937& randomEngine) {
  if (children.empty()) {
    return audio::ErrorSound;
  }
  std::uniform_int_distribution<unsigned int> dist(0, weightSum - 1);
  unsigned int randomValue = dist(randomEngine);
  unsigned int cumulativeWeight = 0;
  for (const auto& entry : children) {
    cumulativeWeight += entry->getWeight();
    if (randomValue < cumulativeWeight) {
      return entry->getHandleToPlay(randomEngine);
      break;
    }
  }
  return audio::ErrorSound;
}

audio::SoundHandle BundleEntry::playOrdered(std::mt19937& randomEngine) {
  if (children.empty()) {
    return audio::ErrorSound;
  }
  if (orderTracker >= children.size()) {
    orderTracker = 0;
  }
  audio::SoundHandle h = children[orderTracker]->getHandleToPlay(randomEngine);
  orderTracker++;
  return h;
}

void BundleEntry::setChildWeight(size_t index, unsigned int weight) {
  if (index < children.size()) {
    weightSum += weight - children[index]->getWeight();
    children[index]->setWeight(weight);
  }
}
void BundleEntry::removeChild(size_t index) {
  if (index < children.size()) {
    weightSum -= children[index]->getWeight();
    children.erase(children.begin() + index);
  }
}
void BundleEntry::removeChild(PlayableEntry* entry) {
  auto it = std::find(children.begin(), children.end(), entry);
  if (it != children.end()) {
    weightSum -= (*it)->getWeight();
    children.erase(it);
  }
}
} // namespace sb
