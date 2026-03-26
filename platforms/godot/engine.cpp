#include "godot/engine.h"

#include <barelymusician.h>

#include <atomic>
#include <cstdint>

#include "godot_cpp/classes/audio_frame.hpp"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/audio_stream_player.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"

namespace barely::godot {

using ::godot::AudioFrame;
using ::godot::AudioServer;
using ::godot::AudioStreamPlayback;
using ::godot::AudioStreamPlayer;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::Engine;
using ::godot::Object;
using ::godot::PropertyHint;
using ::godot::PropertyInfo;
using ::godot::Ref;
using ::godot::SceneTree;
using ::godot::Variant;

#define BARELY_BIND_GODOT_ENGINE_CONTROL(Name, name, ...)                                   \
  ClassDB::bind_method(D_METHOD(BARELY_STR(set_##name), #name), &BarelyEngine::set_##name); \
  ClassDB::bind_method(D_METHOD(BARELY_STR(get_##name)), &BarelyEngine::get_##name);
#define BARELY_SET_DEFAULT_GODOT_ENGINE_CONTROL(Name, name, type, default) set_##name(default);

double BarelyAudioStreamPlayback::get_audio_timestamp() {
  return timestamp_.load(std::memory_order_relaxed);
}

BarelyAudioStreamPlayback::BarelyAudioStreamPlayback() {
  sample_rate_ = static_cast<double>(AudioServer::get_singleton()->get_mix_rate());
}

int32_t BarelyAudioStreamPlayback::_mix_resampled(AudioFrame* buffer, int32_t frame_count) {
  BarelyEngine::get_singleton()->process(buffer, frame_count, timestamp_);
  frame_ += static_cast<int64_t>(frame_count);
  timestamp_ = static_cast<double>(frame_) / sample_rate_;
  return frame_count;
}

Ref<AudioStreamPlayback> BarelyAudioStream::_instantiate_playback() const {
  if (!playback_.is_valid()) {
    if (SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop())) {
      if (static constexpr const char* kNodeName = "BarelyEngineNode";
          !tree->get_root()->has_node(kNodeName)) {
        BarelyEngineNode* node = memnew(BarelyEngineNode);
        node->set_name(kNodeName);
        tree->get_root()->call_deferred("add_child", node);
        node->set_owner(nullptr);
      }
      playback_.instantiate();
    }
  }
  return playback_;
}

void BarelyEngineNode::_process([[maybe_unused]] double delta) {
  BarelyEngine::get_singleton()->update();
}

BarelyEngine::BarelyEngine() {
  ERR_FAIL_COND(singleton_ != nullptr);
  singleton_ = this;
}

BarelyEngine::~BarelyEngine() {
  ERR_FAIL_COND(singleton_ != this);
  singleton_ = nullptr;
  if (audio_player_ != nullptr) {
    memdelete(audio_player_);
    audio_player_ = nullptr;
  }
  BarelyEngine_Destroy(engine_);
  engine_ = nullptr;
}

double BarelyEngine::get_tempo() {
  double tempo = 0.0;
  BarelyEngine_GetTempo(get(), &tempo);
  return tempo;
}

double BarelyEngine::get_timestamp() {
  double timestamp = 0.0;
  BarelyEngine_GetTimestamp(get(), &timestamp);
  return timestamp;
}

void BarelyEngine::set_tempo(double tempo) { BarelyEngine_SetTempo(get(), tempo); }

::BarelyEngine* BarelyEngine::get() {
  if (engine_ == nullptr) {
    // TODO(#181): Support sample rate changes after initialization.
    BarelyEngine_Create(static_cast<int32_t>(AudioServer::get_singleton()->get_mix_rate()),
                        &engine_);
    BARELY_GODOT_ENGINE_CONTROLS(BARELY_SET_DEFAULT_GODOT_ENGINE_CONTROL);
    if (SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop())) {
      if (audio_player_ == nullptr) {  // start audio processing
        audio_player_ = memnew(AudioStreamPlayer);
        audio_player_->set_name("BarelyAudioPlayer");
        audio_player_->set_stream(memnew(BarelyAudioStream));
        tree->get_root()->call_deferred("add_child", audio_player_);
        audio_player_->set_owner(nullptr);
        audio_player_->call_deferred("play");
      }
    }
  }
  return engine_;
}

void BarelyEngine::process(AudioFrame* buffer, int32_t frame_count, double timestamp) {
  static constexpr int32_t kStereoChannelCount = 2;
  float output[BARELY_MAX_FRAME_COUNT * kStereoChannelCount];
  BarelyEngine_Process(engine_, output, kStereoChannelCount, frame_count, timestamp);
  for (int32_t frame = 0; frame < frame_count; ++frame) {
    buffer[frame].left = output[frame * kStereoChannelCount];
    buffer[frame].right = output[frame * kStereoChannelCount + 1];
  }
}

void BarelyEngine::update() {
  static constexpr double kLatency = 0.1;
  BarelyEngine_Update(get(), BarelyAudioStreamPlayback::get_audio_timestamp() + kLatency);
}

void BarelyEngine::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_tempo"), &BarelyEngine::get_tempo);
  ClassDB::bind_method(D_METHOD("get_timestamp"), &BarelyEngine::get_timestamp);
  ClassDB::bind_method(D_METHOD("set_tempo", "tempo"), &BarelyEngine::set_tempo);

  BARELY_GODOT_ENGINE_CONTROLS(BARELY_BIND_GODOT_ENGINE_CONTROL);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tempo"), "set_tempo", "get_tempo");

  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "comp_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_comp_mix", "get_comp_mix");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "comp_attack", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
      "set_comp_attack", "get_comp_attack");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "comp_release", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
      "set_comp_release", "get_comp_release");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "comp_threshold", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_comp_threshold", "get_comp_threshold");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "comp_ratio", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_comp_ratio", "get_comp_ratio");

  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "delay_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_delay_mix", "get_delay_mix");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "delay_time", PropertyHint::PROPERTY_HINT_RANGE, "0,8,0.01"),
      "set_delay_time", "get_delay_time");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "delay_feedback", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_delay_feedback", "get_delay_feedback");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "delay_lpf_cutoff", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_delay_lpf_cutoff", "get_delay_lpf_cutoff");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "delay_hpf_cutoff", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_delay_hpf_cutoff", "get_delay_hpf_cutoff");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "delay_ping_pong", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_delay_ping_pong", "get_delay_ping_pong");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "delay_reverb_send", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,2,0.01"),
               "set_delay_reverb_send", "get_delay_reverb_send");

  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "reverb_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_reverb_mix", "get_reverb_mix");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "reverb_damping", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_reverb_damping", "get_reverb_damping");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_room_size", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_reverb_room_size", "get_reverb_room_size");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "reverb_stereo_width",
                            PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_reverb_stereo_width", "get_reverb_stereo_width");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reverb_freeze"), "set_reverb_freeze",
               "get_reverb_freeze");

  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "sidechain_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_sidechain_mix", "get_sidechain_mix");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_attack", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,8,0.01"),
               "set_sidechain_attack", "get_sidechain_attack");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_release", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,8,0.01"),
               "set_sidechain_release", "get_sidechain_release");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_threshold",
                            PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
               "set_sidechain_threshold", "get_sidechain_threshold");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_ratio", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,1,0.01"),
               "set_sidechain_ratio", "get_sidechain_ratio");
}

}  // namespace barely::godot
