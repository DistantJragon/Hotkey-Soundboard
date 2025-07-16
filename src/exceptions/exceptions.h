#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace SbExceptions {

class SoundGroupDoesNotExist : public std::runtime_error {
public:
  explicit SoundGroupDoesNotExist()
      : std::runtime_error("Sound group does not exist.") {}
};

class MissingSoundGroup : public std::runtime_error {
public:
  explicit MissingSoundGroup(const int id)
      : std::runtime_error("Sound group with id '" + std::to_string(id) +
                           "' is missing from the soundboard."),
        id(id) {}
  int getId() const { return id; }

private:
  const int id;
};

class SoundGroupIdExists : public std::runtime_error {
public:
  explicit SoundGroupIdExists(const int id)
      : std::runtime_error("Sound group with id '" + std::to_string(id) +
                           "' already exists."),
        id(id) {}
  int getId() const { return id; }

private:
  const int id;
};

} // namespace SbExceptions

#endif // EXCEPTIONS_H
