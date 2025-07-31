#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include "core/soundboard/bundleentry.h"
#include "core/soundboard/soundboardtypes.h"
#include <cstring>
#include <random>
#include <string>

class SoundGroup {
public:
  SoundGroup(const GroupHandle handle, const std::string& name);
  ~SoundGroup() = default;
  void play(std::mt19937& randomEngine);

  std::string getName() const { return name; }
  GroupHandle getHandle() const { return handle; }
  void setName(const std::string& newName) { name = newName; }

private:
  BundleEntry bundle;
  std::string name;
  const GroupHandle handle = -1;
};

#endif // SOUNDGROUP_H
