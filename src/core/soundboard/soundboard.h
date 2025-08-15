#ifndef SOUNDBOARD_H
#define SOUNDBOARD_H

#include "core/audio/iaudioengine.h"
#include "core/soundboard/playableentry.h"
#include "core/soundboard/soundboardtypes.h"
#include <random>
#include <string>
#include <unordered_map>

namespace sb {

class Soundboard {
public:
  Soundboard(sb::audio::IAudioEngine* audioEngine);
  ~Soundboard() = default;

  /*!
   * \brief Adds a bundle entry to the soundboard.
   * \param name The name of the new bundle.
   * \return A handle to the new bundle entry.
   */
  EntryHandle newBundle(const std::string& name);

  /*!
   * \brief Adds a bundle entry to the soundboard under a parent entry.
   * \param name The name of the new bundle.
   * \param parentHandle The handle of the parent entry.
   * \param index The index at which to insert the new bundle in the parent's
   * list.
   * \return A handle to the new bundle entry.
   */
  EntryHandle newBundle(const std::string& name, EntryHandle parentHandle,
                        size_t index);

  /*!
   * \brief Adds a sound file entry to the soundboard.
   * \param path The path to the sound file.
   * \param name The name of the new sound file.
   * \param parentHandle The handle of the parent entry.
   * \param index The index at which to insert the new sound file in the
   * parent's list.
   * \return A handle to the new sound file entry.
   */
  EntryHandle newSoundFile(std::filesystem::path path, const std::string& name,
                           EntryHandle parentHandle, size_t index);

  /*!
   * \brief Renames an entry
   * \param entry The handle to the entry to rename.
   * \param newName The new name for the sound group.
   */
  void renameEntry(EntryHandle entry, const std::string& newName);

  /*!
   * \brief Deletes an entry from the soundboard.
   * \param entry The handle to the entry to delete.
   */
  void deleteEntry(EntryHandle entry);

  /*!
   * \brief Deletes an entry from the soundboard.
   * \param parent The handle of the parent entry.
   * \param index The index of the entry to delete in the parent's list.
   */
  void deleteEntryViaParent(EntryHandle parent, size_t index);

  /*!
   * \brief Checks if an entry exists in the soundboard.
   * \param entry The handle of the entry to check.
   * \return True if the entry is valid, false otherwise.
   */
  bool isValidEntry(EntryHandle entry) const;

  /*!
   * \brief Retrieves a non-owning pointer to an entry in the soundboard.
   * \param entry The handle of the entry to retrieve.
   * \return A pointer to the PlayableEntry if it exists, nullptr otherwise.
   */
  PlayableEntry* getEntry(EntryHandle entry);

  /*!
   * \brief Plays an entry from the soundboard.
   * \param entry The handle of the entry to play.
   */
  void playEntry(EntryHandle entry);

  EntryHandle getNextHandle() const { return nextHandle; }

private:
  std::unordered_map<EntryHandle, std::unique_ptr<PlayableEntry>> entries;
  std::mt19937 randomEngine{std::random_device{}()};
  audio::IAudioEngine* const audioEngine = nullptr;
  EntryHandle nextHandle = 0;

  /*!
   * \brief Deletes an entry from the soundboard, including its children.
   * This function does not handle removing the entry from its parent's
   * children list if the entry has a parent.
   * \param entry The handle of the entry to delete.
   */
  void deleteEntryAndChildren(EntryHandle entry);
};

} // namespace sb

#endif // SOUNDBOARD_H
