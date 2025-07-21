#ifndef PLAYABLEENTRY_H
#define PLAYABLEENTRY_H

#include <string>

class PlayableEntry {
public:
  PlayableEntry();
  virtual ~PlayableEntry() = default;
  virtual void play() = 0;
  std::string getPath() const { return path; }
  std::string getName() const { return name; }
  void setName(const std::string& newName) { name = newName; }
  unsigned int getWeight() const { return weight; }
  void setWeight(unsigned int newWeight) { weight = newWeight; }
  bool isValid() const { return valid; }

private:
  std::string path;
  std::string name;
  unsigned int weight = 1;
  bool valid = false;
};

#endif // PLAYABLEENTRY_H
