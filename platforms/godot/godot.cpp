#include <barelymusician.h>

#include <cstdint>

#include "gdextension_interface.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/audio_stream_playback_resampled.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using ::godot::AudioFrame;
using ::godot::AudioServer;
using ::godot::AudioStream;
using ::godot::AudioStreamPlayback;
using ::godot::AudioStreamPlaybackResampled;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::GDExtensionBinding;
using ::godot::ModuleInitializationLevel;
using ::godot::Object;
using ::godot::Ref;

class BarelyMusician : public Object {
 public:
  static void destroy() {
    BarelyEngine_Destroy(engine_);
    engine_ = nullptr;
  }

  static BarelyEngine* get() {
    if (engine_ == nullptr) {
      // TODO(#181): Support sample rate changes after initialization.
      BarelyEngine_Create(static_cast<int32_t>(AudioServer::get_singleton()->get_mix_rate()),
                          &engine_);
      // todo: test
      uint32_t instrument_id = 0;
      BarelyEngine_CreateInstrument(engine_, &instrument_id);
      BarelyInstrument_SetNoteOn(engine_, instrument_id, 0.0f);
      BarelyInstrument_SetControl(engine_, instrument_id, BarelyInstrumentControlType_kOscMix,
                                  0.5f);
    }
    return engine_;
  }

  static double get_tempo() {
    double tempo = 0.0;
    BarelyEngine_GetTempo(get(), &tempo);
    return tempo;
  }

  static double get_timestamp() {
    double timestamp = 0.0;
    BarelyEngine_GetTimestamp(get(), &timestamp);
    return timestamp;
  }

  static void set_tempo(double tempo) { BarelyEngine_SetTempo(get(), tempo); }

  static void update(double timestamp) { BarelyEngine_Update(get(), timestamp); }

 private:
  GDCLASS(BarelyMusician, Object);

  static void _bind_methods() {
    ClassDB::bind_static_method(kClassName, D_METHOD("get_tempo"), &BarelyMusician::get_tempo);
    ClassDB::bind_static_method(kClassName, D_METHOD("get_timestamp"),
                                &BarelyMusician::get_timestamp);
    ClassDB::bind_static_method(kClassName, D_METHOD("set_tempo", "tempo"),
                                &BarelyMusician::set_tempo);
    ClassDB::bind_static_method(kClassName, D_METHOD("update", "timestamp"),
                                &BarelyMusician::update);
  }

  static constexpr const char* kClassName = "BarelyMusician";
  static inline BarelyEngine* engine_ = nullptr;  // singleton
};

class BarelyAudioStreamPlayback : public AudioStreamPlaybackResampled {
 public:
  BarelyAudioStreamPlayback() {
    sample_rate_ = static_cast<double>(AudioServer::get_singleton()->get_mix_rate());
  }

  float _get_stream_sampling_rate() const override { return static_cast<float>(sample_rate_); }

  bool _is_playing() const override { return true; }

  int32_t _mix_resampled(AudioFrame* buffer, int32_t frames) override {
    static constexpr int32_t kStereoChannelCount = 2;
    float output[BARELY_MAX_FRAME_COUNT * kStereoChannelCount];
    BarelyEngine_Process(BarelyMusician::get(), output, kStereoChannelCount, frames, timestamp_);
    for (int32_t frame = 0; frame < frames; ++frame) {
      buffer[frame].left = output[frame * kStereoChannelCount];
      buffer[frame].right = output[frame * kStereoChannelCount + 1];
    }
    timestamp_ += static_cast<double>(frames) / sample_rate_;
    return frames;
  }

 private:
  GDCLASS(BarelyAudioStreamPlayback, AudioStreamPlaybackResampled);

  static void _bind_methods() {}

  double sample_rate_ = 0.0;
  double timestamp_ = 0.0;
};

class BarelyAudioStream : public AudioStream {
 public:
  Ref<AudioStreamPlayback> _instantiate_playback() const override {
    Ref<BarelyAudioStreamPlayback> playback;
    playback.instantiate();
    return playback;
  }

 private:
  GDCLASS(BarelyAudioStream, AudioStream);

  static void _bind_methods() {}
};

void initialize_barelymusiciangodot(ModuleInitializationLevel p_level) {
  if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  ClassDB::register_class<BarelyMusician>();
  ClassDB::register_class<BarelyAudioStream>();
  ClassDB::register_class<BarelyAudioStreamPlayback>();
}

void terminate_barelymusiciangodot(ModuleInitializationLevel p_level) {
  if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  BarelyMusician::destroy();
}

extern "C" {

GDExtensionBool GDE_EXPORT barelymusiciangodot_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
  GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
  init_obj.register_initializer(initialize_barelymusiciangodot);
  init_obj.register_terminator(terminate_barelymusiciangodot);
  init_obj.set_minimum_library_initialization_level(
      ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);
  return init_obj.init();
}

}  // extern "C"
