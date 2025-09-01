#ifndef IAUDIOENGINE_H
#define IAUDIOENGINE_H

#include "core/audio/audiotypes.h"

#include <filesystem>
#include <vector>

namespace sb::audio {

class IAudioEngine {
public:
  virtual ~IAudioEngine() = default;

  /*! \brief Loads an audio file or raw buffer into the engine.
   * \param filePath The path to the audio file.
   * \returns A handle to the loaded sound.
   *
   * \overload
   * \param buffer A raw audio buffer containing the audio data.
   * \returns A handle to the loaded sound.
   */
  virtual SoundHandle load(const std::filesystem::path& filePath) = 0;
  virtual SoundHandle load(const RawAudioBuffer& buffer) = 0;

  /*! \brief Unloads a sound from the engine.
   * \param sound The handle to the sound to unload.
   */
  virtual void unload(SoundHandle sound) = 0;

  /*! \brief Plays a sound on the specified output device.
   * \param sound The handle to the sound to play.
   * \param device The output device to play the sound on.
   * \param volume The volume level for the sound. Defaults to 1.0 (max volume).
   * \param loop Whether the sound should loop. Defaults to false.
   * \returns A handle to the voice playing the sound.
   */
  virtual VoiceHandle play(SoundHandle sound, DeviceHandle device = ErrorDevice,
                           float volume = 1.0f, bool loop = false) = 0;

  /*! \brief Stops a voice that is currently playing.
   * \param vh The handle to the voice to stop.
   */
  virtual void stop(VoiceHandle voice) = 0;

  /*! \brief Stops all voices currently playing.
   */
  virtual void stopAll() = 0;

  /*! \brief Lists all available output devices.
   * \returns A vector of handles to the available output devices.
   */
  virtual std::vector<DeviceHandle> listOutputDevices() = 0;

  /*! \brief Sets the default output device for the engine.
   * \param device The handle to the device to set as default.
   * \note Set to ErrorDevice to reset to the system default.
   */
  virtual void setEngineDefaultOutputDevice(DeviceHandle device) = 0;

  /*! \brief Sets the maximum number of voices that can play globally.
   * \param max The maximum number of global voices.
   */
  virtual void setMaxGlobalVoices(size_t max) = 0;

  /*! \brief Sets the maximum number of voices that can play on a specific
   * device.
   * \param device The handle to the device.
   * \param max The maximum number of voices for the device.
   */
  virtual void setMaxVoicesPerDevice(DeviceHandle device, size_t max) = 0;

  /*! \brief Sets the cooldown period for playing a sound.
   * \param cooldown The cooldown period in milliseconds.
   */
  virtual void setPlayCooldown(Milliseconds cooldown) = 0;

  /*! \brief Checks if a voice is currently playing.
   * \param vh The handle to the voice to check.
   * \returns True if the voice is playing, false otherwise.
   */
  virtual bool isPlaying(VoiceHandle voice) const = 0;

  /*! \brief Gets the number of currently active voices.
   * \returns The number of active voices.
   */
  virtual size_t activeVoices() const = 0;

  /*! \brief Checks if the given sound is valid.
   * \param sound The handle to the sound to check.
   * \returns True if the sound is valid, false otherwise.
   */
  virtual bool isValidSound(SoundHandle sound) const = 0;

  /*! \brief Checks if the given voice is valid.
   * \param voice The handle to the voice to check.
   * \returns True if the voice is valid, false otherwise.
   */
  virtual bool isValidVoice(VoiceHandle voice) const = 0;

  /*! \brief Checks if the given device is valid.
   * \param device The handle to the device to check.
   * \returns True if the device is valid, false otherwise.
   */
  virtual bool isValidDevice(DeviceHandle device) const = 0;

  /*! \brief Gets the capabilities of the audio engine.
   * \returns A struct containing the capabilities of the engine.
   */
  virtual EngineCapabilities getCapabilities() const = 0;
};

} // namespace sb::audio

#endif // IAUDIOENGINE_H
