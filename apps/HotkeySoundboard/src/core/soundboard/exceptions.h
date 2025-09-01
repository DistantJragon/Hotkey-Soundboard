#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "core/soundboard/soundboardtypes.h"
#include <stdexcept>
#include <string>

namespace sb::exceptions {

class SoundGroupDoesNotExist : public std::runtime_error {
public:
  explicit SoundGroupDoesNotExist()
      : std::runtime_error("Sound group does not exist.") {}
};

class MissingSoundGroup : public std::runtime_error {
public:
  explicit MissingSoundGroup(const GroupHandle id)
      : std::runtime_error("Sound group with handle '" + std::to_string(id) +
                           "' is missing from the soundboard."),
        handle(id) {}
  int getHandle() const { return handle; }

private:
  const GroupHandle handle;
};

class SoundGroupIdExists : public std::runtime_error {
public:
  explicit SoundGroupIdExists(const GroupHandle id)
      : std::runtime_error("Sound group with id '" + std::to_string(id) +
                           "' already exists."),
        handle(id) {}
  int getHandle() const { return handle; }

private:
  const GroupHandle handle;
};

} // namespace sb::exceptions

#endif // EXCEPTIONS_H
