#ifndef SOUNDBOARD_H
#define SOUNDBOARD_H

#include "src/core/soundgroup.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Soundboard {
public:
  Soundboard();
  ~Soundboard() = default;

  /**
   * Adds a new unnamed sound group to the soundboard.
   * @return The non-owning raw pointer to the new sound group
   */
  SoundGroup *newSoundGroup();

  /**
   * Renames a sound group.
   * @param soundGroup The raw pointer to the sound group to rename.
   * @param newName The new name for the sound group.
   * @throws SoundGroupNameExists if the new name already exists
   * @throws SoundGroupDoesNotExist if the sound group does not exist
   */
  void renameSoundGroup(SoundGroup *soundGroup, const std::string &newName);

  /**
   * Finds the unnamed sound group with the highest number in its named
   * @return The highest number in the name of an unnamed sound group, or -1 if
   * no such group exists, or 0 if the group has no number part.
   */
  int findMaxUnnamedSoundGroupName() const;

  /**
   * Gets an iterator to the lower bound of the sound group with the given name.
   * @param name The name of the sound group to find.
   * @return An iterator to the sound group if found, or end() if not found.
   */
  std::vector<std::unique_ptr<SoundGroup>>::iterator
  searchSoundGroupsByName(const std::string &name);

  /**
   * Gets an iterator to the sound group with the given name.
   * @param name The name of the sound group to find.
   * @return An optional iterator to the sound group if found, or an empty
   * optional if not found.
   */
  std::optional<std::vector<std::unique_ptr<SoundGroup>>::iterator>
  getSoundGroupIteratorByName(const std::string &name);

  /**
   * Finds a sound group by its name.
   * @param name The name of the sound group to find.
   * @return A reference to the sound group if found, or an empty optional if
   * not
   */
  std::optional<std::reference_wrapper<SoundGroup>>
  getSoundGroupByName(const std::string &name);

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

#endif // SOUNDBOARD_H
