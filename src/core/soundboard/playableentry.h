#ifndef PLAYABLEENTRY_H
#define PLAYABLEENTRY_H

#include "core/audio/audiotypes.h"
#include <random>
#include <string>

namespace sb {

class PlayableEntry {
public:
  const enum class Type {
    Bundle,
    SoundFile,
    String,
  } type;
  PlayableEntry(Type type);
  PlayableEntry(Type type, const std::string& path = "");
  virtual ~PlayableEntry() = default;
  virtual audio::SoundHandle getHandleToPlay(std::mt19937& randomEngine) = 0;
  std::string getPath() const { return path; }
  std::string getName() const { return name; }
  unsigned int getWeight() const { return weight; }
  bool isValid() const { return valid; }
  void setPath(const std::string& newPath) { path = newPath; }
  void setName(const std::string& newName) { name = newName; }
  void setWeight(unsigned int newWeight) { weight = newWeight; }

protected:
  std::string path = "";
  std::string name = "";
  unsigned int weight = 1;
  bool valid = false;
};

} // namespace sb

#endif // PLAYABLEENTRY_H
