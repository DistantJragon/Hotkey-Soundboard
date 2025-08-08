#ifndef SOUNDGROUP_H
#define SOUNDGROUP_H

#include "core/audio/audiotypes.h"
#include "core/soundboard/bundleentry.h"
#include "core/soundboard/soundboardtypes.h"
#include <cstring>
#include <random>
#include <string>

namespace sb {

class SoundGroup {
public:
  SoundGroup(const GroupHandle handle, const std::string& name);
  ~SoundGroup() = default;
  audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine);

  const BundleEntry& getBundle() const { return bundle; }
  std::string getName() const { return name; }
  GroupHandle getHandle() const { return handle; }
  void setName(const std::string& newName) { name = newName; }

private:
  BundleEntry bundle;
  std::string name;
  const GroupHandle handle = -1;
};

} // namespace sb

#endif // SOUNDGROUP_H
