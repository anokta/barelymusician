#include "godot/instrument.h"

#include <barelymusician.h>

#include <algorithm>
#include <cmath>

#include "godot/engine.h"
#include "godot_cpp/classes/audio_frame.hpp"
#include "godot_cpp/classes/audio_listener2d.hpp"
#include "godot_cpp/classes/audio_listener3d.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/audio_stream_player3d.hpp"
#include "godot_cpp/classes/audio_stream_wav.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"

namespace barely::godot {

using ::godot::AudioFrame;
using ::godot::AudioStreamPlayback;
using ::godot::AudioStreamWAV;
using ::godot::Callable;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::MethodInfo;
using ::godot::Node;
using ::godot::Object;
using ::godot::PropertyHint;
using ::godot::PropertyInfo;
using ::godot::Ref;
using ::godot::StringName;
using ::godot::TypedArray;
using ::godot::Variant;

#define BARELY_BIND_GODOT_ENUM_VALUE(EnumType, Name, Value)                \
  ClassDB::bind_integer_constant(get_class_static(), StringName(), #Value, \
                                 Barely##EnumType##_k##Name);
#define BARELY_BIND_GODOT_INSTRUMENT_CONTROL(ClassName, Name, name, ...)                 \
  ClassDB::bind_method(D_METHOD(BARELY_STR(set_##name), #name), &ClassName::set_##name); \
  ClassDB::bind_method(D_METHOD(BARELY_STR(get_##name)), &ClassName::get_##name);
#define BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL(ClassName, Name, name, type, default)       \
  BarelyInstrument_SetControl(BarelyEngine::get_singleton()->get(), state_.get_instrument_id(), \
                              BarelyInstrumentControlType_k##Name, static_cast<float>(name##_));

#define BARELY_BIND_GODOT_INSTRUMENT_COMMON(ClassName)                                             \
  ClassDB::bind_method(D_METHOD("set_all_notes_off"), &ClassName::set_all_notes_off);              \
  ClassDB::bind_method(D_METHOD("set_note_off", "pitch"), &ClassName::set_note_off);               \
  ClassDB::bind_method(D_METHOD("set_note_on", "pitch", "gain", "pitch_shift"),                    \
                       &ClassName::set_note_on, DEFVAL(1.0f), DEFVAL(0.0f));                       \
  ClassDB::bind_method(D_METHOD("is_note_on", "pitch"), &ClassName::is_note_on);                   \
  ClassDB::bind_method(D_METHOD("set_slices", "slices"), &ClassName::set_slices);                  \
  ClassDB::bind_method(D_METHOD("get_slices"), &ClassName::get_slices);                            \
  ClassDB::bind_method(D_METHOD("_on_slice_changed"), &ClassName::on_slice_changed);               \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Crossfade, OSC_MODE_CROSSFADE);                            \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Am, OSC_MODE_AM);                                          \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Fm, OSC_MODE_FM);                                          \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Ma, OSC_MODE_MA);                                          \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Mf, OSC_MODE_MF);                                          \
  BARELY_BIND_GODOT_ENUM_VALUE(OscMode, Ring, OSC_MODE_RING);                                      \
  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Sustain, SLICE_MODE_SUSTAIN);                            \
  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Once, SLICE_MODE_ONCE);                                  \
  BARELY_BIND_GODOT_ENUM_VALUE(SliceMode, Loop, SLICE_MODE_LOOP);                                  \
  BARELY_GODOT_INSTRUMENT_CONTROLS(ClassName, BARELY_BIND_GODOT_INSTRUMENT_CONTROL);               \
  ADD_SIGNAL(MethodInfo("note_on", PropertyInfo(Variant::FLOAT, "pitch")));                        \
  ADD_SIGNAL(MethodInfo("note_off", PropertyInfo(Variant::FLOAT, "pitch")));                       \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "gain", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),         \
      "set_gain", "get_gain");                                                                     \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "pitch_shift", PropertyHint::PROPERTY_HINT_RANGE, "-2,2,0.01"), \
      "set_pitch_shift", "get_pitch_shift");

#define BARELY_BIND_GODOT_INSTRUMENT_GROUPS(ClassName)                                             \
  ADD_GROUP("Envelope", "");                                                                       \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack", PropertyHint::PROPERTY_HINT_RANGE,           \
                            "0,8,0.01,suffix:s"),                                                  \
               "set_attack", "get_attack");                                                        \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay", PropertyHint::PROPERTY_HINT_RANGE,            \
                            "0,8,0.01,suffix:s"),                                                  \
               "set_decay", "get_decay");                                                          \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "sustain", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),      \
      "set_sustain", "get_sustain");                                                               \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "release", PropertyHint::PROPERTY_HINT_RANGE,          \
                            "0,8,0.01,suffix:s"),                                                  \
               "set_release", "get_release");                                                      \
  ADD_GROUP("Slice", "");                                                                          \
  ADD_PROPERTY(PropertyInfo(Variant::INT, "slice_mode", PropertyHint::PROPERTY_HINT_ENUM,          \
                            "Sustain,Loop,Once"),                                                  \
               "set_slice_mode", "get_slice_mode");                                                \
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "slices", PropertyHint::PROPERTY_HINT_ARRAY_TYPE,      \
                            "BarelySliceResource"),                                                \
               "set_slices", "get_slices");                                                        \
  ADD_GROUP("Oscillator", "");                                                                     \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "osc_mix", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),      \
      "set_osc_mix", "get_osc_mix");                                                               \
  ADD_PROPERTY(PropertyInfo(Variant::INT, "osc_mode", PropertyHint::PROPERTY_HINT_ENUM,            \
                            "Crossfade,AM,FM,MA,MF,Ring"),                                         \
               "set_osc_mode", "get_osc_mode");                                                    \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "osc_noise_mix", PropertyHint::PROPERTY_HINT_RANGE,    \
                            "0,1,0.01"),                                                           \
               "set_osc_noise_mix", "get_osc_noise_mix");                                          \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "osc_pitch_shift"), "set_osc_pitch_shift",             \
               "get_osc_pitch_shift");                                                             \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "osc_shape", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),    \
      "set_osc_shape", "get_osc_shape");                                                           \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "osc_skew", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"),    \
      "set_osc_skew", "get_osc_skew");                                                             \
  ADD_GROUP("Bit Crusher", "");                                                                    \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "crush_depth", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),  \
      "set_crush_depth", "get_crush_depth");                                                       \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "crush_rate", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),   \
      "set_crush_rate", "get_crush_rate");                                                         \
  ADD_GROUP("Distortion", "");                                                                     \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distortion_mix", PropertyHint::PROPERTY_HINT_RANGE,   \
                            "0,1,0.01"),                                                           \
               "set_distortion_mix", "get_distortion_mix");                                        \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distortion_drive", PropertyHint::PROPERTY_HINT_RANGE, \
                            "0,1,0.01"),                                                           \
               "set_distortion_drive", "get_distortion_drive");                                    \
  ADD_GROUP("Filter", "");                                                                         \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_cutoff", PropertyHint::PROPERTY_HINT_RANGE,    \
                            "0,1,0.01"),                                                           \
               "set_filter_cutoff", "get_filter_cutoff");                                          \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_resonance", PropertyHint::PROPERTY_HINT_RANGE, \
                            "0,1,0.01"),                                                           \
               "set_filter_resonance", "get_filter_resonance");                                    \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "filter_tone", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"), \
      "set_filter_tone", "get_filter_tone");                                                       \
  ADD_GROUP("Send Effects", "");                                                                   \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "delay_send", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),   \
      "set_delay_send", "get_delay_send");                                                         \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::FLOAT, "reverb_send", PropertyHint::PROPERTY_HINT_RANGE, "0,2,0.01"),  \
      "set_reverb_send", "get_reverb_send");                                                       \
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sidechain_send", PropertyHint::PROPERTY_HINT_RANGE,   \
                            "-1,1,0.01"),                                                          \
               "set_sidechain_send", "get_sidechain_send");                                        \
  ADD_GROUP("Polyphony", "");                                                                      \
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "retrigger"), "set_retrigger", "get_retrigger");        \
  ADD_PROPERTY(                                                                                    \
      PropertyInfo(Variant::INT, "voice_count", PropertyHint::PROPERTY_HINT_RANGE, "1,16"),        \
      "set_voice_count", "get_voice_count");

BarelyInstrumentState::BarelyInstrumentState(Node& instrument) : instrument_(&instrument) {
  ::BarelyEngine* engine = BarelyEngine::get_singleton()->get();
  instrument_id_ = BarelyEngine_CreateInstrument(engine);
}

BarelyInstrumentState::~BarelyInstrumentState() {
  BarelyInstrument_Destroy(BarelyEngine::get_singleton()->get(), instrument_id_);
  instrument_id_ = 0;
}

void BarelyInstrumentState::on_slice_changed() {
  if (!slice_buffers_.empty()) {
    BarelyInstrument_SetSampleData(BarelyEngine::get_singleton()->get(), instrument_id_, nullptr,
                                   0);
    slice_buffers_.clear();
  }

  if (slices_.is_empty()) {
    return;
  }

  std::vector<BarelySlice> sample_data(slices_.size(), BarelySlice{nullptr, 0, 0, 0.0f});
  slice_buffers_.resize(slices_.size());

  for (int i = 0; i < slices_.size(); ++i) {
    const Ref<BarelySliceResource>& slice = slices_[i];
    if (slice.is_null()) {
      continue;
    }

    Ref<AudioStreamWAV> stream = slice->get_stream();
    if (stream.is_null()) {
      continue;
    }

    const auto& data = stream->get_data();
    const uint8_t* bytes = data.ptr();
    const int byte_count = data.size();

    const auto format = stream->get_format();
    const int32_t sample_rate = stream->get_mix_rate();

    auto& samples = slice_buffers_[i];
    if (format == AudioStreamWAV::FORMAT_16_BITS) {
      const int16_t* pcm_bytes = reinterpret_cast<const int16_t*>(bytes);
      const int count = byte_count / sizeof(int16_t);
      samples.resize(count);
      for (int i = 0; i < count; ++i) {
        static constexpr float kMaxSample = 32768.0f;
        samples[i] = pcm_bytes[i] / kMaxSample;
      }
    } else if (format == AudioStreamWAV::FORMAT_8_BITS) {
      samples.resize(byte_count);
      for (int i = 0; i < byte_count; ++i) {
        static constexpr float kMaxSample = 128.0f;
        samples[i] = (bytes[i] - 128) / kMaxSample;
      }
    } else if (format == AudioStreamWAV::FORMAT_QOA) {
      Ref<AudioStreamPlayback> playback = stream->instantiate_playback();
      if (!playback.is_valid()) {
        continue;
      }
      playback->start(0.0);

      const float length = stream->get_length();
      samples.reserve(static_cast<int32_t>(length * static_cast<float>(sample_rate)));

      static constexpr int kQoaFrameCount = 512;
      while (playback->is_playing()) {
        const auto frames = playback->mix_audio(1.0f, kQoaFrameCount);
        if (frames.size() == 0) {
          break;
        }
        for (int i = 0; i < static_cast<int>(frames.size()); ++i) {
          const auto& frame = frames.ptr()[i];
          samples.push_back(0.5f * (frame.x + frame.y));
        }
      }
    } else {
      continue;  // TODO(#181): Support all formats.
    }

    sample_data[i] = {samples.data(), static_cast<int32_t>(samples.size()), sample_rate,
                      slice->get_root_pitch()};
  }

  BarelyInstrument_SetSampleData(BarelyEngine::get_singleton()->get(), instrument_id_,
                                 sample_data.data(), static_cast<int32_t>(sample_data.size()));
}

void BarelyInstrumentState::set_all_notes_off() {
  for (const float pitch : std::exchange(pitches_, {})) {
    BarelyInstrument_SetNoteOff(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
    instrument_->emit_signal("note_off", pitch);
  }
}

void BarelyInstrumentState::set_note_off(float pitch) {
  if (pitches_.erase(pitch) > 0) {
    BarelyInstrument_SetNoteOff(BarelyEngine::get_singleton()->get(), instrument_id_, pitch);
    instrument_->emit_signal("note_off", pitch);
  }
}

void BarelyInstrumentState::set_note_on(float pitch, float gain, float pitch_shift) {
  if (pitches_.insert(pitch).second) {
    ::BarelyEngine* engine = BarelyEngine::get_singleton()->get();
    BarelyInstrument_SetNoteOn(engine, instrument_id_, pitch);
    if (gain != 1.0f) {
      BarelyInstrument_SetNoteControl(engine, instrument_id_, pitch, BarelyNoteControlType_kGain,
                                      gain);
    }
    if (pitch_shift != 0.0f) {
      BarelyInstrument_SetNoteControl(engine, instrument_id_, pitch,
                                      BarelyNoteControlType_kPitchShift, pitch_shift);
    }
    instrument_->emit_signal("note_on", pitch);
  }
}

void BarelyInstrumentState::set_slices(const TypedArray<Ref<BarelySliceResource>>& slices) {
  for (int i = 0; i < slices_.size(); ++i) {
    Ref<BarelySliceResource> slice = slices_[i];
    if (slice.is_valid()) {
      slice->disconnect("slice_changed", Callable(instrument_, "_on_slice_changed"));
    }
  }
  slices_ = slices;
  for (int i = 0; i < slices_.size(); ++i) {
    Ref<BarelySliceResource> slice = slices_[i];
    if (slice.is_valid()) {
      slice->connect("slice_changed", Callable(instrument_, "_on_slice_changed"),
                     Object::CONNECT_DEFERRED);
    }
  }
  on_slice_changed();
}

BarelyInstrument::BarelyInstrument() : state_(*this) {
  BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, Gain, gain, float, 1.0f);
  BARELY_GODOT_INSTRUMENT_CONTROLS(BarelyInstrument, BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL);
  BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, StereoPan, stereo_pan, float, 0.0f);
}

void BarelyInstrument::_bind_methods() {
  BARELY_BIND_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, Gain, gain);
  BARELY_BIND_GODOT_INSTRUMENT_COMMON(BarelyInstrument);
  BARELY_BIND_GODOT_INSTRUMENT_CONTROL(BarelyInstrument, StereoPan, stereo_pan);
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "stereo_pan", PropertyHint::PROPERTY_HINT_RANGE, "-1,1,0.01"),
      "set_stereo_pan", "get_stereo_pan");
  BARELY_BIND_GODOT_INSTRUMENT_GROUPS(BarelyInstrument);
}

BarelyInstrument2D::BarelyInstrument2D() : state_(*this) {
  BARELY_GODOT_INSTRUMENT_CONTROLS(BarelyInstrument2D, BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL);
  auto* engine = BarelyEngine::get_singleton()->get();
  BarelyInstrument_SetControl(engine, state_.get_instrument_id(), BarelyInstrumentControlType_kGain,
                              spatial_gain_);
  BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                              BarelyInstrumentControlType_kStereoPan, spatial_pan_);
}

void BarelyInstrument2D::_process([[maybe_unused]] double delta) {
  float spatial_gain = gain_;
  float spatial_pan = 0.0f;

  const auto* listener = get_viewport()->get_audio_listener_2d();
  if (listener != nullptr) {
    const auto offset = get_global_position() - listener->get_global_position();
    const float distance = offset.length();
    if (max_distance_ > 0.0f) {
      const float normalized_distance = std::clamp(distance / max_distance_, 0.0f, 1.0f);
      spatial_gain *= (normalized_distance >= 1.0f)
                          ? 0.0f
                          : std::pow(1.0f - normalized_distance, std::max(attenuation_, 0.0f));

      const float listener_local_x = listener->to_local(get_global_position()).x;
      spatial_pan = (listener_local_x / max_distance_) * panning_strength_;
    } else if (distance > 0.0f) {
      spatial_gain = 0.0f;
    }
  }

  auto* engine = BarelyEngine::get_singleton()->get();
  if (spatial_gain_ != spatial_gain) {
    spatial_gain_ = spatial_gain;
    BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                                BarelyInstrumentControlType_kGain, spatial_gain_);
  }
  if (spatial_pan_ != spatial_pan) {
    spatial_pan_ = spatial_pan;
    BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                                BarelyInstrumentControlType_kStereoPan, spatial_pan_);
  }
}

void BarelyInstrument2D::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_gain", "gain"), &BarelyInstrument2D::set_gain);
  ClassDB::bind_method(D_METHOD("get_gain"), &BarelyInstrument2D::get_gain);
  BARELY_BIND_GODOT_INSTRUMENT_COMMON(BarelyInstrument2D);
  ClassDB::bind_method(D_METHOD("set_max_distance", "max_distance"),
                       &BarelyInstrument2D::set_max_distance);
  ClassDB::bind_method(D_METHOD("get_max_distance"), &BarelyInstrument2D::get_max_distance);
  ClassDB::bind_method(D_METHOD("set_attenuation", "attenuation"),
                       &BarelyInstrument2D::set_attenuation);
  ClassDB::bind_method(D_METHOD("get_attenuation"), &BarelyInstrument2D::get_attenuation);
  ClassDB::bind_method(D_METHOD("set_panning_strength", "panning_strength"),
                       &BarelyInstrument2D::set_panning_strength);
  ClassDB::bind_method(D_METHOD("get_panning_strength"), &BarelyInstrument2D::get_panning_strength);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,4096,1,or_greater,suffix:px"),
               "set_max_distance", "get_max_distance");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "attenuation", PropertyHint::PROPERTY_HINT_RANGE, "0,1000,0.01"),
      "set_attenuation", "get_attenuation");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "panning_strength", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,10,0.01"),
               "set_panning_strength", "get_panning_strength");
  BARELY_BIND_GODOT_INSTRUMENT_GROUPS(BarelyInstrument2D);
}

BarelyInstrument3D::BarelyInstrument3D() : state_(*this) {
  BARELY_GODOT_INSTRUMENT_CONTROLS(BarelyInstrument3D, BARELY_SET_DEFAULT_GODOT_INSTRUMENT_CONTROL);
  auto* engine = BarelyEngine::get_singleton()->get();
  BarelyInstrument_SetControl(engine, state_.get_instrument_id(), BarelyInstrumentControlType_kGain,
                              spatial_gain_);
  BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                              BarelyInstrumentControlType_kStereoPan, spatial_pan_);
}

void BarelyInstrument3D::_process([[maybe_unused]] double delta) {
  float spatial_gain = gain_;
  float spatial_pan = 0.0f;

  const auto* listener = get_viewport()->get_audio_listener_3d();
  if (listener != nullptr) {
    const auto listener_local_position = listener->to_local(get_global_position());
    const float distance = listener_local_position.length();
    if (max_distance_ > 0.0f && distance > max_distance_) {
      spatial_gain = 0.0f;
    } else if (const float normalized_distance = (unit_size_ > 0.0f) ? distance / unit_size_ : 0.0f;
               normalized_distance > 0.0f) {
      switch (attenuation_model_) {
        case ::godot::AudioStreamPlayer3D::ATTENUATION_INVERSE_DISTANCE:
          spatial_gain *= 1.0f / normalized_distance;
          break;
        case ::godot::AudioStreamPlayer3D::ATTENUATION_INVERSE_SQUARE_DISTANCE:
          spatial_gain *= 1.0f / normalized_distance * normalized_distance;
          break;
        case ::godot::AudioStreamPlayer3D::ATTENUATION_LOGARITHMIC:
          spatial_gain *= std::pow(10.0f, -std::log(normalized_distance));
          break;
        case ::godot::AudioStreamPlayer3D::ATTENUATION_DISABLED:
        default:
          break;
      }
    }
    const float horizontal_distance =
        std::hypot(listener_local_position.x, listener_local_position.z);
    if (horizontal_distance > 0.0f) {
      spatial_pan = (listener_local_position.x / horizontal_distance) * panning_strength_;
    }
  }

  auto* engine = BarelyEngine::get_singleton()->get();
  if (spatial_gain_ != spatial_gain) {
    spatial_gain_ = spatial_gain;
    BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                                BarelyInstrumentControlType_kGain, spatial_gain_);
  }
  if (spatial_pan_ != spatial_pan) {
    spatial_pan_ = spatial_pan;
    BarelyInstrument_SetControl(engine, state_.get_instrument_id(),
                                BarelyInstrumentControlType_kStereoPan, spatial_pan_);
  }
}

void BarelyInstrument3D::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_gain", "gain"), &BarelyInstrument3D::set_gain);
  ClassDB::bind_method(D_METHOD("get_gain"), &BarelyInstrument3D::get_gain);
  BARELY_BIND_GODOT_INSTRUMENT_COMMON(BarelyInstrument3D);
  ClassDB::bind_method(D_METHOD("set_attenuation_model", "attenuation_model"),
                       &BarelyInstrument3D::set_attenuation_model);
  ClassDB::bind_method(D_METHOD("get_attenuation_model"),
                       &BarelyInstrument3D::get_attenuation_model);
  ClassDB::bind_method(D_METHOD("set_unit_size", "unit_size"), &BarelyInstrument3D::set_unit_size);
  ClassDB::bind_method(D_METHOD("get_unit_size"), &BarelyInstrument3D::get_unit_size);
  ClassDB::bind_method(D_METHOD("set_max_distance", "max_distance"),
                       &BarelyInstrument3D::set_max_distance);
  ClassDB::bind_method(D_METHOD("get_max_distance"), &BarelyInstrument3D::get_max_distance);
  ClassDB::bind_method(D_METHOD("set_panning_strength", "panning_strength"),
                       &BarelyInstrument3D::set_panning_strength);
  ClassDB::bind_method(D_METHOD("get_panning_strength"), &BarelyInstrument3D::get_panning_strength);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "attenuation_model", PropertyHint::PROPERTY_HINT_ENUM,
                            "Inverse Distance,Inverse Square Distance,Logarithmic,Disabled"),
               "set_attenuation_model", "get_attenuation_model");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "unit_size", PropertyHint::PROPERTY_HINT_RANGE,
                            "0.01,1000,0.01,or_greater,suffix:m"),
               "set_unit_size", "get_unit_size");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,4096,0.01,or_greater,suffix:m"),
               "set_max_distance", "get_max_distance");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "panning_strength", PropertyHint::PROPERTY_HINT_RANGE,
                            "0,10,0.01"),
               "set_panning_strength", "get_panning_strength");
  BARELY_BIND_GODOT_INSTRUMENT_GROUPS(BarelyInstrument3D);
}

}  // namespace barely::godot
