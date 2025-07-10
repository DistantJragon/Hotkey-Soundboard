#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace SbExceptions {
class SoundGroupNameExists : public std::runtime_error {
public:
  explicit SoundGroupNameExists(const std::string &name)
      : std::runtime_error("Sound group with name '" + name +
                           "' already exists."),
        name(name) {}
  const std::string &getName() const { return name; }

private:
  std::string name;
};
class SoundGroupDoesNotExist : public std::runtime_error {
public:
  explicit SoundGroupDoesNotExist()
      : std::runtime_error("Sound group does not exist.") {}
};
class MissingSoundGroup : public std::runtime_error {
public:
  explicit MissingSoundGroup(const std::string &name)
      : std::runtime_error("Sound group with name '" + name +
                           "' is missing from the soundboard."),
        name(name) {}
  const std::string &getName() const { return name; }

private:
  std::string name;
};
} // namespace SbExceptions

#endif // EXCEPTIONS_H
