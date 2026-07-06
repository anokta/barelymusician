#ifndef BARELYMUSICIAN_GODOT_RESOURCES_H_
#define BARELYMUSICIAN_GODOT_RESOURCES_H_

#include <barelymusician.h>

#include <cstdint>

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/packed_float32_array.hpp"

namespace barely::godot {

class BarelyQuantizationResource : public ::godot::Resource {
 public:
  void set_subdivision(int32_t subdivision);
  void set_amount(float amount);

  double get_position(double position) const;
  int32_t get_subdivision() const { return quantization_.subdivision; }
  float get_amount() const { return quantization_.amount; }

 private:
  GDCLASS(BarelyQuantizationResource, ::godot::Resource);
  static void _bind_methods();

  BarelyQuantization quantization_ = {1, 1.0f};
};

class BarelyScaleResource : public ::godot::Resource {
 public:
  void set_pitches(const ::godot::PackedFloat32Array& pitches);
  void set_root_pitch(float root_pitch);
  void set_mode(int32_t mode);

  float get_pitch(int32_t degree) const;
  ::godot::PackedFloat32Array get_pitches() const { return pitches_; }
  int32_t get_pitch_count() const { return static_cast<int32_t>(pitches_.size()); }
  float get_root_pitch() const { return scale_.root_pitch; }
  int32_t get_mode() const { return scale_.mode; }

 private:
  GDCLASS(BarelyScaleResource, ::godot::Resource);
  static void _bind_methods();

  ::godot::PackedFloat32Array pitches_ = {0.0f};
  BarelyScale scale_ = {pitches_.ptr(), static_cast<int32_t>(pitches_.size()), 0.0f, 0};
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_RESOURCES_H_
