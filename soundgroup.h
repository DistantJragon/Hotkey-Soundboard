#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include <string>
#include <vector>

#include "hotkey.h"
#include "hotkeymanager.h"
#include "playableentry.h"

#define UNNAMED_SOUND_GROUP_NAME "New Sound Group"
#define UNNAMED_SOUND_GROUP_PREFIX "New Sound Group "
#define UNNAMED_SOUND_GROUP_NAME_LENGTH 15

class SoundGroup {
public:
  SoundGroup(HotkeyManager& hkManager, const std::string &name);
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
