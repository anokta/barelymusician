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
#include "godot_cpp/classes/audio_stream_player.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace barely::godot {

#define BARELY_STR(x) #x

#define BARELY_GODOT_ENGINE_CONTROLS(X)                   \
  X(Gain, gain, float, 1.0f)                              \
  X(CompMix, comp_mix, float, 1.0f)                       \
  X(CompAttack, comp_attack, float, 0.0f)                 \
  X(CompRelease, comp_release, float, 0.0f)               \
  X(CompThreshold, comp_threshold, float, 1.0f)           \
  X(CompRatio, comp_ratio, float, 0.0f)                   \
  X(DelayMix, delay_mix, float, 1.0f)                     \
  X(DelayTime, delay_time, float, 0.0f)                   \
  X(DelayFeedback, delay_feedback, float, 0.0f)           \
  X(DelayLpfCutoff, delay_lpf_cutoff, float, 1.0f)        \
  X(DelayHpfCutoff, delay_hpf_cutoff, float, 0.0f)        \
  X(DelayPingPong, delay_ping_pong, float, 0.0f)          \
  X(DelayReverbSend, delay_reverb_send, float, 0.0f)      \
  X(ReverbMix, reverb_mix, float, 1.0f)                   \
  X(ReverbDamping, reverb_damping, float, 0.0f)           \
  X(ReverbRoomSize, reverb_room_size, float, 0.0f)        \
  X(ReverbStereoWidth, reverb_stereo_width, float, 1.0f)  \
  X(ReverbFreeze, reverb_freeze, bool, 0)                 \
  X(SidechainMix, sidechain_mix, float, 1.0f)             \
  X(SidechainAttack, sidechain_attack, float, 0.0f)       \
  X(SidechainRelease, sidechain_release, float, 0.0f)     \
  X(SidechainThreshold, sidechain_threshold, float, 1.0f) \
  X(SidechainRatio, sidechain_ratio, float, 0.0f)

#define BARELY_DEFINE_GODOT_ENGINE_CONTROL(Name, name, type, default) \
 private:                                                             \
  type name##_;                                                       \
                                                                      \
 public:                                                              \
  void set_##name(type name) {                                        \
    name##_ = name;                                                   \
    BarelyEngine_SetControl(engine_, BarelyEngineControlType_k##Name, \
                            static_cast<float>(name##_));             \
  }                                                                   \
  type get_##name() const { return name##_; }

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
  static BarelyEngine* get_singleton() { return singleton_; }

  BarelyEngine();
  ~BarelyEngine();

  double get_tempo();
  double get_timestamp();
  void set_tempo(double tempo);

  ::BarelyEngine* get();
  void process(::godot::AudioFrame* buffer, int32_t frames, double timestamp);
  void update();

  double get_lookahead() const { return lookahead_; }
  void set_lookahead(double lookahead) { lookahead_ = lookahead; }

 private:
  GDCLASS(BarelyEngine, ::godot::Object);

  static void _bind_methods();
  static inline BarelyEngine* singleton_ = nullptr;

  ::BarelyEngine* engine_ = nullptr;
  ::godot::AudioStreamPlayer* audio_player_ = nullptr;
  double lookahead_ = 0.02;

  BARELY_GODOT_ENGINE_CONTROLS(BARELY_DEFINE_GODOT_ENGINE_CONTROL);
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_ENGINE_H_
