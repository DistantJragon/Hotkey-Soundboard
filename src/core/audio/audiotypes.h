#ifndef AUDIO_TYPES_H
#define AUDIO_TYPES_H

#include <chrono>

namespace sb::audio {

using SoundHandle = unsigned int;
using VoiceHandle = unsigned int;
using DeviceHandle = unsigned int;
static constexpr SoundHandle ErrorSound = static_cast<SoundHandle>(-1);
static constexpr VoiceHandle ErrorVoice = static_cast<VoiceHandle>(-1);
static constexpr DeviceHandle ErrorDevice = static_cast<DeviceHandle>(-1);

enum class BufferStrategy {
  None,
  Full,
};

using Milliseconds = std::chrono::milliseconds;

typedef struct {
  bool deviceChangeable;
  bool supportsSoundCoodown;
  bool supportsFileLoading;
  bool supportsRawBufferLoading;
  bool voiceStoppable;
} EngineCapabilities;

enum class SampleFormat {
  Unknown,
  Int16,
  Int24,
  Int32,
  Float32,
};

typedef struct {
  const std::byte* data;
  size_t size;
  unsigned int sampleRate;
  unsigned int channels;
  SampleFormat format;
} RawAudioBuffer;

} // namespace sb::audio

#endif // AUDIO_TYPES_H
