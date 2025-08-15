#include "core/soundboard/soundboard.h"
#include "core/soundboard/bundleentry.h"
#include "core/soundboard/soundfileentry.h"

namespace sb {

Soundboard::Soundboard(audio::IAudioEngine* audioEngine)
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
      nextHandle, std::make_unique<SoundFileEntry>(nextHandle, parentHandle,
                                                   soundHandle, path.string()));
  if (!result.second) {
    audioEngine->unload(soundHandle);
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
  deleteEntryAndChildren(entry);
}

void Soundboard::deleteEntryViaParent(EntryHandle parent, size_t index) {
  if (parent == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(parent);
  if (it == entries.end() ||
      PlayableEntry::isContainerType(it->second->type) == false) {
    return;
  }
  auto& parentEntry = static_cast<ContainerEntry&>(*it->second);
  PlayableEntry* entryToDelete = parentEntry.getChildren().at(index);
  parentEntry.removeChild(index);
  if (!entryToDelete) {
    return;
  }
  deleteEntryAndChildren(entryToDelete->getHandle());
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

void Soundboard::deleteEntryAndChildren(EntryHandle entry) {
  if (entry == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(entry);
  if (it == entries.end()) {
    return;
  }
  if (PlayableEntry::isContainerType(it->second->type)) {
    auto& container = static_cast<ContainerEntry&>(*it->second);
    for (const auto& child : container.getChildren()) {
      deleteEntryAndChildren(child->getHandle());
    }
  } else if (it->second->type == PlayableEntry::Type::SoundFile) {
    auto& soundFileEntry = static_cast<SoundFileEntry&>(*it->second);
    audioEngine->unload(soundFileEntry.getHandleToPlay(randomEngine));
  }
  entries.erase(it);
}

} // namespace sb
