#include "src/core/soundboard.h"
#include "src/exceptions/exceptions.h"
#include <algorithm>

Soundboard::Soundboard() {}

std::vector<std::unique_ptr<SoundGroup>>::iterator
Soundboard::searchSoundGroupsByName(const std::string &name) {
  return std::lower_bound(
      soundGroups.begin(), soundGroups.end(), name,
      [](const std::unique_ptr<SoundGroup> &group, const std::string &name) {
        return group->getName() < name;
      });
}

std::optional<std::reference_wrapper<SoundGroup>>
Soundboard::getSoundGroupByName(const std::string &name) {
  auto it = searchSoundGroupsByName(name);
  if (it != soundGroups.end() && (*it)->getName() == name) {
    return std::ref(**it);
  } else {
    return std::nullopt;
  }
}

std::optional<std::vector<std::unique_ptr<SoundGroup>>::iterator>
Soundboard::getSoundGroupIteratorByName(const std::string &name) {
  auto it = searchSoundGroupsByName(name);
  if (it != soundGroups.end() && (*it)->getName() == name) {
    return it;
  } else {
    return std::nullopt;
  }
}

int Soundboard::findMaxUnnamedSoundGroupName() const {
  // Find the last sound group with a name starting with the default name
  auto it = std::upper_bound(
      soundGroups.rbegin(), soundGroups.rend(), SoundGroupDefaults::NAME,
      [](const std::string_view &name, const std::unique_ptr<SoundGroup> &group) {
        return group->getName().substr(0, name.size()) <= name;
      });
  if (it == soundGroups.rend()) {
    // No possible unnamed sound groups found
    return -1;
  }
  // Iterate backwards to find the highest numbered unnamed sound group
  for (; it != soundGroups.rend(); ++it) {
    const std::string &groupName = (*it)->getName();
    if (groupName == SoundGroupDefaults::NAME) {
      // Found a sound group with the default name
      return 0;
    }
    if (groupName.substr(0, SoundGroupDefaults::NAME.size()) !=
        SoundGroupDefaults::NAME) {
      // No unnamed sound group found
      return -1;
    }
    if (groupName.size() > SoundGroupDefaults::PREFIX.size()) {
      // Extract the number part from the name
      std::string numberPart =
          groupName.substr(SoundGroupDefaults::PREFIX.size());
      try {
        int number = std::stoi(numberPart);
        // Return the extracted number
        return number;
      } catch (const std::invalid_argument &) {
        // Not a valid number, continue searching
        continue;
      }
    }
  }
  return -1;
}

SoundGroup *Soundboard::newSoundGroup() {
  int maxUnnamedNumber = findMaxUnnamedSoundGroupName();
  std::string newName;
  if (maxUnnamedNumber >= 0) {
    newName = std::string(SoundGroupDefaults::PREFIX) +
              std::to_string(maxUnnamedNumber + 1);
  } else {
    newName = SoundGroupDefaults::NAME;
  }
  if (getSoundGroupIteratorByName(newName).has_value()) {
    throw SbExceptions::SoundGroupNameExists(newName);
  }
  std::unique_ptr<SoundGroup> newGroup =
      std::make_unique<SoundGroup>(hotkeyManager, newName);
  SoundGroup *newGroupPtr = newGroup.get();
  auto it = searchSoundGroupsByName(newName);
  soundGroups.emplace(it, std::move(newGroup));
  return newGroupPtr;
}

void Soundboard::sortSoundGroups() {
  std::sort(soundGroups.begin(), soundGroups.end(),
            [](const std::unique_ptr<SoundGroup> &a,
               const std::unique_ptr<SoundGroup> &b) {
              return a->getName() < b->getName();
            });
}

bool Soundboard::checkSoundGroupsSorted() const {
  return std::is_sorted(soundGroups.begin(), soundGroups.end(),
                        [](const std::unique_ptr<SoundGroup> &a,
                           const std::unique_ptr<SoundGroup> &b) {
                          return a->getName() < b->getName();
                        });
}
