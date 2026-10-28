#include "godot/resources.h"

#include <algorithm>

#include "godot_cpp/classes/audio_stream_wav.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"

namespace barely::godot {

using ::godot::AudioStreamWAV;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::MethodInfo;
using ::godot::PackedFloat32Array;
using ::godot::PropertyHint;
using ::godot::PropertyInfo;
using ::godot::Ref;
using ::godot::Variant;

void BarelyQuantizationResource::set_subdivision(int32_t subdivision) {
  quantization_.subdivision = std::max(subdivision, 1);
  emit_changed();
}

void BarelyQuantizationResource::set_amount(float amount) {
  quantization_.amount = std::clamp(amount, 0.0f, 1.0f);
  emit_changed();
}

double BarelyQuantizationResource::get_position(double position) const {
  return BarelyQuantization_GetPosition(&quantization_, position);
}

void BarelyQuantizationResource::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_subdivision", "subdivision"),
                       &BarelyQuantizationResource::set_subdivision);
  ClassDB::bind_method(D_METHOD("get_subdivision"), &BarelyQuantizationResource::get_subdivision);

  ClassDB::bind_method(D_METHOD("set_amount", "amount"), &BarelyQuantizationResource::set_amount);
  ClassDB::bind_method(D_METHOD("get_amount"), &BarelyQuantizationResource::get_amount);
  ClassDB::bind_method(D_METHOD("get_position", "position"),
                       &BarelyQuantizationResource::get_position);

  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "subdivision", PropertyHint::PROPERTY_HINT_RANGE, "1,256"),
      "set_subdivision", "get_subdivision");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "amount", PropertyHint::PROPERTY_HINT_RANGE, "0,1,0.01"),
      "set_amount", "get_amount");
}

void BarelyScaleResource::set_pitches(const PackedFloat32Array& pitches) {
  pitches_ = pitches;
  scale_.pitches = pitches_.ptr();
  scale_.pitch_count = static_cast<int32_t>(pitches_.size());
  emit_changed();
}

void BarelyScaleResource::set_root_pitch(float root_pitch) {
  scale_.root_pitch = root_pitch;
  emit_changed();
}

void BarelyScaleResource::set_mode(int32_t mode) {
  scale_.mode = mode;
  emit_changed();
}

float BarelyScaleResource::get_pitch(int32_t degree) const {
  return BarelyScale_GetPitch(&scale_, degree);
}

void BarelyScaleResource::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_pitches", "pitches"), &BarelyScaleResource::set_pitches);
  ClassDB::bind_method(D_METHOD("get_pitches"), &BarelyScaleResource::get_pitches);

  ClassDB::bind_method(D_METHOD("set_root_pitch", "root_pitch"),
                       &BarelyScaleResource::set_root_pitch);
  ClassDB::bind_method(D_METHOD("get_root_pitch"), &BarelyScaleResource::get_root_pitch);

  ClassDB::bind_method(D_METHOD("set_mode", "mode"), &BarelyScaleResource::set_mode);
  ClassDB::bind_method(D_METHOD("get_mode"), &BarelyScaleResource::get_mode);

  ClassDB::bind_method(D_METHOD("get_pitch_count"), &BarelyScaleResource::get_pitch_count);
  ClassDB::bind_method(D_METHOD("get_pitch", "degree"), &BarelyScaleResource::get_pitch);

  ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "pitches"), "set_pitches",
               "get_pitches");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "root_pitch", PropertyHint::PROPERTY_HINT_RANGE, "-4,4,0.01"),
      "set_root_pitch", "get_root_pitch");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "mode"), "set_mode", "get_mode");
}

void BarelySliceResource::set_root_pitch(float root_pitch) {
  if (root_pitch_ != root_pitch) {
    root_pitch_ = root_pitch;
    emit_signal("slice_changed");
  }
}

void BarelySliceResource::set_stream(const Ref<AudioStreamWAV>& stream) {
  if (stream_ != stream) {
    stream_ = stream;
    emit_signal("slice_changed");
  }
}

void BarelySliceResource::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_stream", "stream"), &BarelySliceResource::set_stream);
  ClassDB::bind_method(D_METHOD("get_stream"), &BarelySliceResource::get_stream);

  ClassDB::bind_method(D_METHOD("set_root_pitch", "root_pitch"),
                       &BarelySliceResource::set_root_pitch);
  ClassDB::bind_method(D_METHOD("get_root_pitch"), &BarelySliceResource::get_root_pitch);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stream", PropertyHint::PROPERTY_HINT_RESOURCE_TYPE,
                            "AudioStreamWAV"),
               "set_stream", "get_stream");
  ADD_PROPERTY(
      PropertyInfo(Variant::FLOAT, "root_pitch", PropertyHint::PROPERTY_HINT_RANGE, "-4,4,0.01"),
      "set_root_pitch", "get_root_pitch");

  ADD_SIGNAL(MethodInfo("slice_changed"));
}

}  // namespace barely::godot
