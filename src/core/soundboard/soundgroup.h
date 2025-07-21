#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include "core/hotkey/hotkey.h"
#include "core/hotkey/ihotkeymanager.h"
#include "core/soundboard/playableentry.h"
#include <cstring>
#include <string>
#include <vector>

class SoundGroup {
public:
  SoundGroup(IHotkeyManager& hkManager, const int id, const std::string& name);
  ~SoundGroup() = default;
  void play();

  std::string getName() const { return name; }
  int getId() const { return id; }
  bool isRandomPlay() const { return randomPlay; }
  const std::vector<std::reference_wrapper<Hotkey>>& getHotkeys() const {
    return hotkeys;
  }
  const std::vector<PlayableEntry>& getEntries() const { return entries; }
  unsigned int getCategory() const { return category; }
  unsigned int getWeightSum() const { return weightSum; }
  unsigned int getOrderTracker() const { return orderTracker; }
  void setName(const std::string& newName) { name = newName; }
  void setRandomPlay(bool random) { randomPlay = random; }

private:
  std::vector<PlayableEntry> entries;
  std::vector<std::reference_wrapper<Hotkey>> hotkeys;
  std::reference_wrapper<IHotkeyManager> hotkeyManager;
  std::string name;
  const int id = -1;
  unsigned int category = 0;
  unsigned int weightSum = 0;
  unsigned int orderTracker = 0;
  bool randomPlay = true;
};

#endif // SOUNDGROUP_H
