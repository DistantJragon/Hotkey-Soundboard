#ifndef SOUNDBOARD_H
#define SOUNDBOARD_H

#include "core/audio/iaudioengine.h"
#include "core/soundboard/soundboardtypes.h"
#include "core/soundboard/soundgroup.h"
#include <random>
#include <string>
#include <unordered_map>

namespace sb {

class Soundboard {
public:
  Soundboard(sb::audio::IAudioEngine* audioEngine);
  ~Soundboard() = default;

  /*!
   * Adds a new unnamed sound group to the soundboard.
   * \param name The name of the new sound group.
   * \return A handle to the newly created sound group.
   */
  GroupHandle newSoundGroup(const std::string& name);

  /*!
   * Renames a sound group.
   * \param soundGroup The handle to the sound group to rename.
   * \param newName The new name for the sound group.
   */
  void renameSoundGroup(GroupHandle group, const std::string& newName);

  /*!
   * Removes a sound group from the soundboard.
   * \param soundGroup The handle to the sound group to remove.
   */
  void deleteSoundGroup(GroupHandle soundGroup);

  /*!
   * Checks if a sound group is valid.
   * \param group The handle of the sound group to check.
   * \return True if the sound group exists, false otherwise.
   */
  bool isValidGroup(GroupHandle group) const;

  /*!
   * Retrieves a sound group by its handle.
   * \param group The handle of the sound group to retrieve.
   * \return A reference to the sound group.
   * \throws MissingSoundGroup if the group does not exist.
   */
  SoundGroup& getSoundGroup(GroupHandle group);

  /*!
   * Plays a sound group.
   * \param group The handle of the sound group to play.
   */
  void playSoundGroup(GroupHandle group);

  GroupHandle getNextHandle() const { return nextHandle; }

private:
  std::unordered_map<GroupHandle, SoundGroup> soundGroups;
  std::mt19937 randomEngine{std::random_device{}()};
  sb::audio::IAudioEngine* const audioEngine = nullptr;
  GroupHandle nextHandle = 0;
};

} // namespace sb

#endif // SOUNDBOARD_H
