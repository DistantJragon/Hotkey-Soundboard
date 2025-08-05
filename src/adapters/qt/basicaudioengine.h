#ifndef QTBASICAUDIOENGINE_H
#define QTBASICAUDIOENGINE_H

#include "core/audio/iaudioengine.h"
#include <QMediaDevices>
#include <QSoundEffect>
#include <unordered_map>

namespace sb::adapters::qt {

class BasicAudioEngine : public audio::IAudioEngine {
public:
  BasicAudioEngine();
  ~BasicAudioEngine() override;

  audio::SoundHandle load(const std::filesystem::path& filePath) override;
  audio::SoundHandle load(const audio::RawAudioBuffer& buffer) override {
    return audio::ErrorSound;
  }
  void unload(audio::SoundHandle sound) override;

  audio::VoiceHandle play(audio::SoundHandle sound,
                          audio::DeviceHandle device = audio::ErrorDevice,
                          float volume = 1.0f, bool loop = false) override;
  void stop(audio::VoiceHandle voice) override;
  void stopAll() override;

  std::vector<audio::DeviceHandle> listOutputDevices() override;
  void setEngineDefaultOutputDevice(audio::DeviceHandle device) override;

  void setMaxGlobalVoices(size_t max) override;
  void setMaxVoicesPerDevice(audio::DeviceHandle device, size_t max) override;

  void setPlayCooldown(audio::Milliseconds cooldown) override;

  bool isPlaying(audio::VoiceHandle voice) const override;
  size_t activeVoices() const override;

  bool isValidSound(audio::SoundHandle sound) const override;
  bool isValidVoice(audio::VoiceHandle voice) const override;
  bool isValidDevice(audio::DeviceHandle device) const override;

  audio::EngineCapabilities getCapabilities() const override;

private:
  std::unordered_map<audio::SoundHandle, QSoundEffect> soundEffects;
  std::unordered_map<audio::DeviceHandle, QAudioDevice> audioDevices;

  QMediaDevices mediaDevices;
  audio::SoundHandle nextSoundHandle = 0;
  audio::DeviceHandle nextDeviceHandle = 0;
  audio::DeviceHandle defaultDevice = audio::ErrorDevice;
  audio::EngineCapabilities capabilities = {
      .deviceChangeable = true,
      .supportsSoundCoodown = false,
      .supportsFileLoading = true,
      .supportsRawBufferLoading = false,
      .voiceStoppable = true,
  };

  /*! \brief Refreshes the list of available audio devices.
   * This function is called to update the internal list of audio devices
   * after any changes in the system audio configuration.
   */
  void refreshAudioDevices();
};
} // namespace sb::adapters::qt

#endif // QTBASICAUDIOENGINE_H
