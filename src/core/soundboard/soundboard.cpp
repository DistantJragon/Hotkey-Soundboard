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
  // TODO: Check if index is valid
  parentEntry.addChild(index, result.first->second.get());
  nextHandle++;
  return result.first->first;
}

EntryHandle Soundboard::newBundle(const std::string& name,
                                  EntryHandle parentHandle, size_t index,
                                  const std::string& path, bool recursive) {
  if (parentHandle == InvalidEntryHandle || nextHandle == InvalidEntryHandle) {
    return InvalidEntryHandle;
  }
  auto parentIt = entries.find(parentHandle);
  if (parentIt == entries.end() ||
      PlayableEntry::isContainerType(parentIt->second->type) == false) {
    return InvalidEntryHandle;
  }
  auto [pairIt, success] = entries.try_emplace(
      nextHandle, std::make_unique<BundleEntry>(nextHandle, parentHandle));
  if (!success) {
    return InvalidEntryHandle;
  }
  nextHandle++;
  auto& bundleEntry = static_cast<BundleEntry&>(*pairIt->second);
  bundleEntry.setName(name);
  bundleEntry.setPath(path);
  bundleEntry.setRecursive(recursive);
  bundleEntry.setRandomPlay(true);
  reloadBundle(pairIt->first);
  auto& parentEntry = static_cast<ContainerEntry&>(*parentIt->second);
  // TODO: Check if index is valid
  parentEntry.addChild(index, pairIt->second.get());
  return pairIt->first;
}

void Soundboard::reloadBundle(EntryHandle bundleHandle) {
  if (bundleHandle == InvalidEntryHandle) {
    return;
  }
  auto it = entries.find(bundleHandle);
  if (it == entries.end() || it->second->type != PlayableEntry::Type::Bundle) {
    return;
  }
  auto& bundleEntry = static_cast<BundleEntry&>(*it->second);
  if (bundleEntry.getPath().empty()) {
    return;
  }
  std::error_code ec;
  if (!std::filesystem::is_directory(bundleEntry.getPath(), ec) || ec) {
    return;
  }
  // Clear existing children by index
  for (int i = bundleEntry.getChildren().size() - 1; i >= 0; --i) {
    deleteEntryViaParent(bundleHandle, i);
  }
  size_t currentIndex = 0;
  EntryHandle newHandle = InvalidEntryHandle;
  for (const auto& dirEntry :
       std::filesystem::directory_iterator(bundleEntry.getPath(), ec)) {
    if (ec) {
      continue;
    }
    if (dirEntry.is_regular_file()) {
      newHandle =
          newSoundFile(dirEntry.path(), dirEntry.path().filename().string(),
                       bundleHandle, bundleEntry.getChildren().size());
      if (newHandle != InvalidEntryHandle) {
        currentIndex++;
      }
    } else if (dirEntry.is_directory() && bundleEntry.isRecursive()) {
      newHandle = newBundle(dirEntry.path().filename().string(), bundleHandle,
                            bundleEntry.getChildren().size(),
                            dirEntry.path().string(), true);
      if (newHandle != InvalidEntryHandle) {
        auto newBundleIt = entries.find(newHandle);
        if (newBundleIt != entries.end() &&
            newBundleIt->second->type == PlayableEntry::Type::Bundle) {
          BundleEntry* newBundlePtr =
              static_cast<BundleEntry*>(newBundleIt->second.get());
          bundleEntry.setChildWeight(currentIndex,
                                     newBundlePtr->getWeightSum());
          currentIndex++;
        }
      }
    }
  }
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
  // TODO: Check if index is valid
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

void Soundboard::stopAllEntries() {
  if (audioEngine) {
    audioEngine->stopAll();
  }
}
} // namespace sb
