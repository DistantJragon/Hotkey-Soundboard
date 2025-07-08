#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "src/core/hotkey.h"
#include "src/core/hotkeymanager.h"
#include "src/core/playableentry.h"

namespace SoundGroupDefaults {
constexpr std::string_view NAME = "New Sound Group";
constexpr std::string_view PREFIX = "New Sound Group ";
constexpr size_t NAME_LENGTH = NAME.length();
constexpr size_t PREFIX_LENGTH = PREFIX.length();
} // namespace SoundGroupDefaults

class SoundGroup {
public:
  SoundGroup(HotkeyManager &hkManager, const std::string &name);
  ~SoundGroup() = default;
  void play();

  std::string getName() const { return name; }
  bool isRandomPlay() const { return randomPlay; }
  const std::vector<std::reference_wrapper<Hotkey>> &getHotkeys() const {
    return hotkeys;
  }
  const std::vector<PlayableEntry> &getEntries() const { return entries; }
  unsigned int getCategory() const { return category; }
  unsigned int getWeightSum() const { return weightSum; }
  unsigned int getOrderTracker() const { return orderTracker; }
  void setName(const std::string &newName) { name = newName; }
  void setRandomPlay(bool random) { randomPlay = random; }

private:
  std::vector<PlayableEntry> entries;
  std::vector<std::reference_wrapper<Hotkey>> hotkeys;
  std::reference_wrapper<HotkeyManager> hotkeyManager;
  std::string name;
  unsigned int category = 0;
  unsigned int weightSum = 0;
  unsigned int orderTracker = 0;
  bool randomPlay = true;

  // friend Soundboard::reassignSoundGroupIDs();
};

#endif // SOUNDGROUP_H
