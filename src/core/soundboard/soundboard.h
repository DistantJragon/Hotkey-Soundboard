#ifndef SOUNDBOARD_H
#define SOUNDBOARD_H

#include "core/soundboard/soundgroup.h"
#include <memory>
#include <string>
#include <unordered_map>

class Soundboard {
public:
  Soundboard();
  ~Soundboard() = default;

  /*!
   * Adds a new unnamed sound group to the soundboard.
   * \param name The name of the new sound group.
   * \return The non-owning raw pointer to the new sound group.
   * \throws SoundGroupIdExists if a sound group with the same ID already exists
   */
  SoundGroup* newSoundGroup(const std::string& name);

  /*!
   * Renames a sound group.
   * \param soundGroup The raw pointer to the sound group to rename.
   * \param newName The new name for the sound group.
   * \throws SoundGroupDoesNotExist if the sound group does not exist
   */
  void renameSoundGroup(SoundGroup* soundGroup, const std::string& newName);

  /*!
   * Removes a sound group from the soundboard.
   * \param soundGroup A reference to the raw pointer to the sound group to
   * delete.
   * \throws SoundGroupDoesNotExist if the sound group does not exist
   */
  void deleteSoundGroup(SoundGroup*& soundGroup);

  int getNextId() const { return nextId; }

private:
  std::unordered_map<int, std::unique_ptr<SoundGroup>> soundGroups;
  IHotkeyManager hotkeyManager;
  int nextId = 0;
};

#endif // SOUNDBOARD_H
