#ifndef SOUNDBOARD_H
#define SOUNDBOARD_H

#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "soundgroup.h"

class Soundboard {
public:
  Soundboard();
  ~Soundboard() = default;

  /**
   * Adds a new unnamed sound group to the soundboard.
   */
  void newSoundGroup();

  /**
   * Finds the unnamed sound group with the highest number in its named
   * @return The highest number in the name of an unnamed sound group, or -1 if
   * no such group exists, or 0 if the group has no number part.
   */
  int findMaxUnnamedSoundGroupName() const;

  /**
   * Finds a sound group by its name.
   * @param name The name of the sound group to find.
   * @return A reference to the sound group if found, or an empty optional if
   * not
   */
  std::optional<std::reference_wrapper<SoundGroup>> getSoundGroupByName(
      const std::string &name);


  /**
   * Reassigns IDs to all sound groups in the soundboard, starting from 0.
   * This is useful after adding, removing, renaming, or reordering sound
   * groups to ensure that all IDs are unique and sequential.
   *
   * @overload
   * @param start An iterator to the sound group from which to start
   */
  void reassignSoundGroupIDs();
  void reassignSoundGroupIDs(
      std::vector<std::unique_ptr<SoundGroup>>::iterator start);

  /**
   * Sorts the sound groups in the soundboard by their names. Hopefully this is
   * used rarely.
   */
  void sortSoundGroups();

  /**
   * Checks if the sound groups are sorted by name.
   * @return true if the sound groups are sorted by name, false otherwise.
   */
  bool checkSoundGroupsSorted() const;

private:
  std::vector<std::unique_ptr<SoundGroup>> soundGroups;
  HotkeyManager hotkeyManager;
};

struct SoundGroupNameExistsException : public std::runtime_error {
public:
  explicit SoundGroupNameExistsException(const SoundGroup &group)
      : std::runtime_error("Sound group with name '" + group.getName() +
                           "' already exists.") {}
  explicit SoundGroupNameExistsException(const std::string &name)
      : std::runtime_error("Sound group with name '" + name +
                           "' already exists.") {}
};

#endif // SOUNDBOARD_H
