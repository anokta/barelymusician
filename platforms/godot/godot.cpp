#include <barelymusician.h>

#include <atomic>
#include <cstdint>

#include "gdextension_interface.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/audio_stream_playback_resampled.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"
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
using ::godot::Engine;
using ::godot::GDExtensionBinding;
using ::godot::ModuleInitializationLevel;
using ::godot::Node;
using ::godot::Object;
using ::godot::Ref;
using ::godot::SceneTree;

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
      // TODO(#181): temp testing
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

 private:
  GDCLASS(BarelyMusician, Object);

  BarelyMusician() = default;
  ~BarelyMusician() = default;

  static void _bind_methods() {
    ClassDB::bind_static_method(kClassName, D_METHOD("get_tempo"), &BarelyMusician::get_tempo);
    ClassDB::bind_static_method(kClassName, D_METHOD("get_timestamp"),
                                &BarelyMusician::get_timestamp);
    ClassDB::bind_static_method(kClassName, D_METHOD("set_tempo", "tempo"),
                                &BarelyMusician::set_tempo);
  }

  static constexpr const char* kClassName = "BarelyMusician";
  static inline BarelyEngine* engine_ = nullptr;  // singleton
};

class BarelyAudioStreamPlayback : public AudioStreamPlaybackResampled {
 public:
  static double get_audio_timestamp() { return timestamp_.load(std::memory_order_relaxed); }

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
    frame_ += static_cast<int64_t>(frames);
    timestamp_ = static_cast<double>(frame_) / sample_rate_;
    return frames;
  }

  void _start([[maybe_unused]] double p_from_pos) override {}

 private:
  GDCLASS(BarelyAudioStreamPlayback, AudioStreamPlaybackResampled);
  static void _bind_methods() {}

  static_assert(std::atomic<double>::is_always_lock_free);
  static inline std::atomic<double> timestamp_ = 0.0;

  int64_t frame_ = 0;
  double sample_rate_ = 0.0;
};

class BarelyMusicianNode : public Node {
 public:
  void _process([[maybe_unused]] double delta) override {
    static constexpr double kLatency = 0.2;
    BarelyEngine_Update(BarelyMusician::get(),
                        BarelyAudioStreamPlayback::get_audio_timestamp() + kLatency);
  }

 private:
  GDCLASS(BarelyMusicianNode, Node);
  static void _bind_methods() {}
};

class BarelyAudioStream : public AudioStream {
 public:
  Ref<AudioStreamPlayback> _instantiate_playback() const override {
    if (!playback_.is_valid()) {
      if (SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop())) {
        static constexpr const char* kNodeName = "BarelyMusicianNode";
        if (!tree->get_root()->has_node(kNodeName)) {
          BarelyMusicianNode* node = memnew(BarelyMusicianNode);
          node->set_name(kNodeName);
          tree->get_root()->call_deferred("add_child", node);
        }
        playback_.instantiate();
      }
    }
    return playback_;
  }

 private:
  GDCLASS(BarelyAudioStream, AudioStream);
  static void _bind_methods() {}

  static inline Ref<BarelyAudioStreamPlayback> playback_ = {};  // singleton
};

void initialize_barelymusiciangodot(ModuleInitializationLevel p_level) {
  if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  ClassDB::register_class<BarelyMusician>();
  ClassDB::register_class<BarelyAudioStream>();
  ClassDB::register_internal_class<BarelyMusicianNode>();
  ClassDB::register_internal_class<BarelyAudioStreamPlayback>();
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
