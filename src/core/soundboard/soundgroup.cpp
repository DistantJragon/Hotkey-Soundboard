#include "core/soundboard/soundgroup.h"

namespace sb {

SoundGroup::SoundGroup(const GroupHandle handle, const std::string& name)
    : name(name), handle(handle) {
  bundle.setName(name);
  bundle.setWeight(1);
}
void SoundGroup::play(std::mt19937& randomEngine) { bundle.play(randomEngine); }

} // namespace sb
