#include "core/soundboard/soundboard.h"
#include "core/soundboard/bundleentry.h"

namespace sb {

Soundboard::Soundboard(sb::audio::IAudioEngine* audioEngine)
    : audioEngine(audioEngine) {}

EntryHandle Soundboard::newBundle(const std::string& name) {
  if (nextHandle == InvalidEntryHandle) {
    return InvalidEntryHandle;
  }
  auto result = entries.try_emplace(nextHandle,
                                    std::make_unique<BundleEntry>(nextHandle));
  if (!result.second) {
    return InvalidEntryHandle;
  }
  result.first->second->setName(name);
  nextHandle++;
  return result.first->first;
}

EntryHandle Soundboard::newBundle(const std::string& name,
                                  EntryHandle parentHandle, size_t index) {
  if (parentHandle == InvalidEntryHandle || nextHandle == InvalidEntryHandle) {
    return InvalidEntryHandle;
  }
  auto parentIt = entries.find(parentHandle);
  if (parentIt == entries.end() ||
      PlayableEntry::isContainerType(parentIt->second->type) == false) {
    return InvalidEntryHandle;
  }
  auto result = entries.try_emplace(
      nextHandle, std::make_unique<BundleEntry>(nextHandle, parentHandle));
  if (!result.second) {
    return InvalidEntryHandle;
  }
  result.first->second->setName(name);
  auto& parentEntry = static_cast<ContainerEntry&>(*parentIt->second);
  parentEntry.addChild(index, result.first->second.get());
  nextHandle++;
  return result.first->first;
}

EntryHandle Soundboard::newSoundFile(std::filesystem::path path,
                                     const std::string& name,
                                     EntryHandle parentHandle, size_t index) {
  if (parentHandle == InvalidEntryHandle || nextHandle == InvalidEntryHandle) {
    return InvalidEntryHandle;
  }
  auto parentIt = entries.find(parentHandle);
  if (parentIt == entries.end() ||
      PlayableEntry::isContainerType(parentIt->second->type) == false) {
    return InvalidEntryHandle;
  }
  auto soundHandle = audioEngine->load(path);
  if (soundHandle == audio::ErrorSound) {
    return InvalidEntryHandle;
  }
  auto result = entries.try_emplace(
      nextHandle, std::make_unique<SoundFileEntry>(soundHandle, path.string()));
  if (!result.second) {
    return InvalidEntryHandle;
  }
  result.first->second->setName(name);
  auto& parentEntry = static_cast<ContainerEntry&>(*parentIt->second);
  parentEntry.addChild(index, result.first->second.get());
  nextHandle++;
  return result.first->first;
}

void Soundboard::renameEntry(EntryHandle entry, const std::string& newName) {
  if (entry == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(entry);
  if (it == entries.end()) {
    return;
  }
  it->second->setName(newName);
}

void Soundboard::deleteEntry(EntryHandle entry) {
  if (entry == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(entry);
  if (it == entries.end()) {
    return;
  }
  if (it->second->getParentHandle() != InvalidEntryHandle) {
    auto parentIt = entries.find(it->second->getParentHandle());
    if (parentIt != entries.end() &&
        PlayableEntry::isContainerType(parentIt->second->type)) {
      auto& parentEntry = static_cast<ContainerEntry&>(*parentIt->second);
      parentEntry.removeChild(it->second.get());
    }
  }
  entries.erase(entry);
}

void Soundboard::deleteEntryFromParent(EntryHandle parent, size_t index) {
  if (parent == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(parent);
  if (it == entries.end() ||
      PlayableEntry::isContainerType(it->second->type) == false) {
    return;
  }
  auto& parentEntry = static_cast<ContainerEntry&>(*it->second);
  parentEntry.removeChild(index);
}

bool Soundboard::isValidEntry(EntryHandle entry) const {
  return entries.find(entry) != entries.end();
}

PlayableEntry* Soundboard::getEntry(EntryHandle entry) {
  if (entry == InvalidEntryHandle) {
    return nullptr;
  }
  auto it = entries.find(entry);
  if (it == entries.end()) {
    return nullptr;
  }
  return it->second.get();
}

void Soundboard::playEntry(EntryHandle entry) {
  if (!audioEngine) {
    return;
  }
  if (entry == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(entry);
  if (it != entries.end()) {
    audioEngine->play(it->second->getHandleToPlay(randomEngine));
  }
}

} // namespace sb
