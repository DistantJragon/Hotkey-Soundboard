#include "core/soundboard/soundgroup.h"

namespace sb {

SoundGroup::SoundGroup(const GroupHandle handle, const std::string& name)
    : name(name), handle(handle) {
  bundle.setName(name + " (Root Bundle)");
  bundle.setWeight(1);
}
audio::SoundHandle SoundGroup::getHandleToPlay(std::mt19937& randomEngine) {
  return bundle.getHandleToPlay(randomEngine);
}

} // namespace sb
