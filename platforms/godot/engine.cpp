#include "godot/engine.h"

#include <barelymusician.h>

#include <atomic>
#include <cstdint>

#include "godot_cpp/classes/audio_frame.hpp"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
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
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::Engine;
using ::godot::Object;
using ::godot::Ref;
using ::godot::SceneTree;

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
      BarelyEngine::get_singleton()->get();  // force initialize
      if (static constexpr const char* kNodeName = "BarelyEngineNode";
          !tree->get_root()->has_node(kNodeName)) {
        BarelyEngineNode* node = memnew(BarelyEngineNode);
        node->set_name(kNodeName);
        tree->get_root()->call_deferred("add_child", node);
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
}

}  // namespace barely::godot
