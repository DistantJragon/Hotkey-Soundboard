#include "adapters/qt/basicaudioengine.h"

#include <QAudioDevice>
#include <unordered_set>

namespace sb::adapters::qt {

BasicAudioEngine::BasicAudioEngine() {}

BasicAudioEngine::~BasicAudioEngine() {}

audio::SoundHandle
BasicAudioEngine::load(const std::filesystem::path& filePath) {
  audio::SoundHandle handle = nextSoundHandle;
  auto result = soundEffects.try_emplace(handle);
  if (result.second) {
    QSoundEffect& soundEffect = result.first->second;
    soundEffect.setSource(
        QUrl::fromLocalFile(QString::fromStdString(filePath.string())));
    soundEffect.setLoopCount(1);
    nextSoundHandle++;
    return handle;
  }
  return audio::ErrorSound;
}

void BasicAudioEngine::unload(audio::SoundHandle sound) {
  auto it = soundEffects.find(sound);
  if (it != soundEffects.end()) {
    soundEffects.erase(it);
  }
}

audio::VoiceHandle BasicAudioEngine::play(audio::SoundHandle sound,
                                          audio::DeviceHandle device,
                                          float volume, bool loop) {
  // TODO: Should probably refresh the audio devices somewhere here
  if (sound == audio::ErrorSound) {
    return audio::ErrorVoice;
  }
  auto it = soundEffects.find(sound);
  if (it == soundEffects.end()) {
    return audio::ErrorVoice;
  }
  QSoundEffect& soundEffect = it->second;
  if (!soundEffect.isLoaded()) {
    return audio::ErrorVoice;
  }
  soundEffect.setVolume(volume);
  soundEffect.setLoopCount(loop ? QSoundEffect::Infinite : 1);
  if (defaultDevice == audio::ErrorDevice) {
    if (device == audio::ErrorDevice) {
      QAudioDevice defaultAudioDevice = mediaDevices.defaultAudioOutput();
      soundEffect.setAudioDevice(defaultAudioDevice);
      soundEffect.play();
      return static_cast<audio::VoiceHandle>(sound);
    }
    device = defaultDevice;
  }
  auto deviceIt = audioDevices.find(device);
  if (deviceIt == audioDevices.end()) {
    return audio::ErrorVoice;
  }
  const QAudioDevice& audioDevice = deviceIt->second;
  soundEffect.setAudioDevice(audioDevice);
  soundEffect.play();
  return static_cast<audio::VoiceHandle>(sound);
}

void BasicAudioEngine::stop(audio::VoiceHandle voice) {
  auto it = soundEffects.find(static_cast<audio::SoundHandle>(voice));
  if (it != soundEffects.end()) {
    it->second.stop();
  }
}

void BasicAudioEngine::stopAll() {
  for (auto& [_, soundEffect] : soundEffects) {
    soundEffect.stop();
  }
}

std::vector<audio::DeviceHandle> BasicAudioEngine::listOutputDevices() {
  refreshAudioDevices();
  std::vector<audio::DeviceHandle> devices;
  for (const auto& [handle, device] : audioDevices) {
    devices.push_back(handle);
  }
  return devices;
}

void BasicAudioEngine::setEngineDefaultOutputDevice(
    audio::DeviceHandle device) {
  if (device == audio::ErrorDevice) {
    defaultDevice = audio::ErrorDevice;
  } else {
    auto it = audioDevices.find(device);
    if (it != audioDevices.end()) {
      defaultDevice = device;
    }
  }
}

void BasicAudioEngine::setMaxGlobalVoices(size_t max) {
  // TODO
}

void BasicAudioEngine::setMaxVoicesPerDevice(audio::DeviceHandle device,
                                             size_t max) {
  // TODO
}

void BasicAudioEngine::setPlayCooldown(audio::Milliseconds cooldown) {
  // Not supported in this engine
}

bool BasicAudioEngine::isPlaying(audio::VoiceHandle voice) const {
  auto it = soundEffects.find(static_cast<audio::SoundHandle>(voice));
  if (it != soundEffects.end()) {
    return it->second.isPlaying();
  }
  return false;
}

size_t BasicAudioEngine::activeVoices() const {
  // TODO
  return 0;
}

bool BasicAudioEngine::isValidSound(audio::SoundHandle sound) const {
  return soundEffects.find(sound) != soundEffects.end();
}

bool BasicAudioEngine::isValidVoice(audio::VoiceHandle voice) const {
  return soundEffects.find(static_cast<audio::SoundHandle>(voice)) !=
         soundEffects.end();
}

bool BasicAudioEngine::isValidDevice(audio::DeviceHandle device) const {
  return audioDevices.find(device) != audioDevices.end();
}

audio::EngineCapabilities BasicAudioEngine::getCapabilities() const {
  return capabilities;
}

void BasicAudioEngine::refreshAudioDevices() {
  std::unordered_set<audio::DeviceHandle> invalidDevices;
  for (const auto& [handle, device] : audioDevices) {
    invalidDevices.insert(handle);
  }
  // Double for loop is a little inefficient, but there are usually not
  // many devices
  const auto outputs = mediaDevices.audioOutputs();
  for (const auto& audioDevice : outputs) {
    bool found = false;
    for (const auto& [handle, device] : audioDevices) {
      if (device == audioDevice) {
        invalidDevices.erase(handle);
        found = true;
        break;
      }
    }
    if (!found) {
      auto result = audioDevices.insert({nextDeviceHandle, audioDevice});
      if (result.second) {
        nextDeviceHandle++;
      }
    }
  }
  for (const auto& handle : invalidDevices) {
    audioDevices.erase(handle);
  }
  auto it = audioDevices.find(defaultDevice);
  if (it == audioDevices.end()) {
    defaultDevice = audio::ErrorDevice;
  } else {
    defaultDevice = it->first;
  }
}
} // namespace sb::adapters::qt
