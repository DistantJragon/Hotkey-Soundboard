#ifndef PLAYABLEENTRY_H
#define PLAYABLEENTRY_H

#include "core/audio/audiotypes.h"
#include "core/soundboard/soundboardtypes.h"
#include <random>
#include <string>

namespace sb {

class PlayableEntry {
public:
  const enum class Type {
    SoundFile = 0,
    Bundle = (1 << 2) | (1 << 0),
    String = (1 << 2) | (1 << 1)
  } type;
  PlayableEntry(Type type, EntryHandle handle, EntryHandle parentHandle);
  static bool isContainerType(Type type);
  virtual ~PlayableEntry() = default;
  virtual audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine) = 0;

  std::string getPath() const { return path; }
  std::string getName() const { return name; }
  EntryHandle getHandle() const { return handle; }
  EntryHandle getParentHandle() const { return parentHandle; }
  unsigned int getWeight() const { return weight; }
  bool isValid() const { return valid; }
  void setPath(const std::string& newPath) { path = newPath; }
  void setName(const std::string& newName) { name = newName; }
  void setWeight(unsigned int newWeight) { weight = newWeight; }

protected:
  std::string path = "";
  std::string name = "";
  const EntryHandle handle = InvalidEntryHandle;
  const EntryHandle parentHandle = InvalidEntryHandle;
  unsigned int weight = 1;
  bool valid = false;
};

} // namespace sb

#endif // PLAYABLEENTRY_H
