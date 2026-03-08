#ifndef BARELYMUSICIAN_GODOT_ENGINE_H_
#define BARELYMUSICIAN_GODOT_ENGINE_H_

#include <barelymusician.h>

#include <atomic>
#include <cstdint>

#include "godot_cpp/classes/audio_frame.hpp"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/audio_stream_playback_resampled.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace barely::godot {

class BarelyAudioStreamPlayback : public ::godot::AudioStreamPlaybackResampled {
 public:
  static double get_audio_timestamp();

  BarelyAudioStreamPlayback();
  int32_t _mix_resampled(::godot::AudioFrame* buffer, int32_t frames) override;

  float _get_stream_sampling_rate() const override { return static_cast<float>(sample_rate_); }
  bool _is_playing() const override { return true; }
  void _start([[maybe_unused]] double p_from_pos) override {}

 private:
  GDCLASS(BarelyAudioStreamPlayback, ::godot::AudioStreamPlaybackResampled);
  static void _bind_methods() {}

  static_assert(std::atomic<double>::is_always_lock_free);
  static inline std::atomic<double> timestamp_ = 0.0;

  int64_t frame_ = 0;
  double sample_rate_ = 0.0;
};

class BarelyAudioStream : public ::godot::AudioStream {
 public:
  ::godot::Ref<::godot::AudioStreamPlayback> _instantiate_playback() const override;

 private:
  GDCLASS(BarelyAudioStream, ::godot::AudioStream);
  static void _bind_methods() {}

  static inline ::godot::Ref<BarelyAudioStreamPlayback> playback_ = {};
};

class BarelyEngineNode : public ::godot::Node {
 public:
  void _process(double delta) override;

 private:
  GDCLASS(BarelyEngineNode, ::godot::Node);
  static void _bind_methods() {}
};

class BarelyEngine : public ::godot::Object {
 public:
  static void destroy();
  static ::BarelyEngine* get();

  static double get_tempo();
  static double get_timestamp();
  static void set_tempo(double tempo);

 private:
  GDCLASS(BarelyEngine, ::godot::Object);

  BarelyEngine() = default;
  ~BarelyEngine() = default;

  static void _bind_methods();
  static inline ::BarelyEngine* engine_ = nullptr;
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_ENGINE_H_
