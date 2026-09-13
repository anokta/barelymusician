#ifndef BARELYMUSICIAN_GODOT_RESOURCES_H_
#define BARELYMUSICIAN_GODOT_RESOURCES_H_

#include <barelymusician.h>

#include <cstdint>

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/packed_float64_array.hpp"

namespace barely::godot {

class BarelyQuantizationResource : public ::godot::Resource {
 public:
  void set_subdivision(int32_t subdivision);
  void set_amount(double amount);

  double get_position(double position) const;
  int32_t get_subdivision() const { return quantization_.subdivision; }
  double get_amount() const { return quantization_.amount; }

 private:
  GDCLASS(BarelyQuantizationResource, ::godot::Resource);
  static void _bind_methods();

  BarelyQuantization quantization_ = {1, 1.0};
};

class BarelyScaleResource : public ::godot::Resource {
 public:
  void set_pitches(const ::godot::PackedFloat64Array& pitches);
  void set_root_pitch(double root_pitch);
  void set_mode(int32_t mode);

  double get_pitch(int32_t degree) const;
  ::godot::PackedFloat64Array get_pitches() const { return pitches_; }
  int32_t get_pitch_count() const { return static_cast<int32_t>(pitches_.size()); }
  double get_root_pitch() const { return scale_.root_pitch; }
  int32_t get_mode() const { return scale_.mode; }

 private:
  GDCLASS(BarelyScaleResource, ::godot::Resource);
  static void _bind_methods();

  ::godot::PackedFloat64Array pitches_ = {0.0};
  BarelyScale scale_ = {0.0, pitches_.ptr(), static_cast<int32_t>(pitches_.size()), 0};
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_RESOURCES_H_
