#include "src/core/soundboard.h"

#include "src/exceptions/exceptions.h"
#include <algorithm>

Soundboard::Soundboard() {}

std::optional<std::reference_wrapper<SoundGroup>>
Soundboard::getSoundGroupByName(const std::string &name) {
  auto it = std::lower_bound(
      soundGroups.begin(), soundGroups.end(), name,
      [](const std::unique_ptr<SoundGroup> &group, const std::string &name) {
        return group->getName() < name;
      });
  if (it != soundGroups.end() && (*it)->getName() == name) {
    return std::ref(**it);
  } else {
    return std::nullopt;
  }
}

int Soundboard::findMaxUnnamedSoundGroupName() const {
  // The sound groups are sorted by name, so we can start from the end and find
  // the first unnamed group. (Could be optimized with binary search if
  // necessary.)
  for (auto it = soundGroups.rbegin(); it != soundGroups.rend(); ++it) {
    std::string name = (*it)->getName();
    if (name.substr(0, SoundGroupDefaults::NAME_LENGTH) ==
        SoundGroupDefaults::NAME) {
      if (name.length() == SoundGroupDefaults::NAME_LENGTH) {
        // The name has no number part, so we can return 0.
        return 0;
      } else if (name.length() <= SoundGroupDefaults::PREFIX_LENGTH) {
        // The name has no number part, but has a length that can't be created
        // by the program, so it isn't unnamed
        return -1;
      }
      std::string numberPart = name.substr(SoundGroupDefaults::PREFIX_LENGTH);
      try {
        unsigned int number = std::stoul(numberPart);
        return number;
      } catch (const std::invalid_argument &) {
        // If the conversion fails, then the sound group isn't really unnamed
        continue;
      }
      // The function will throw an out_of_range exception if necessary.
    }
  }
  return -1;
}

void Soundboard::newSoundGroup() {
  int maxUnnamedNumber = findMaxUnnamedSoundGroupName();
  std::string newName;
  if (maxUnnamedNumber >= 0) {
    newName = std::string(SoundGroupDefaults::PREFIX) +
              std::to_string(maxUnnamedNumber + 1);
  } else {
    newName = SoundGroupDefaults::NAME;
  }
  if (getSoundGroupByName(newName).has_value()) {
    throw SbExceptions::SoundGroupNameExists(newName);
  }
  // Emplace the new group in the correct position to keep the list sorted.
  auto it = std::lower_bound(
      soundGroups.begin(), soundGroups.end(), newName,
      [](const std::unique_ptr<SoundGroup> &group, const std::string &name) {
        return group->getName() < name;
      });
  soundGroups.emplace(it, std::make_unique<SoundGroup>(hotkeyManager, newName));
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
