#ifndef BARELYMUSICIAN_GODOT_INSTRUMENT_H_
#define BARELYMUSICIAN_GODOT_INSTRUMENT_H_

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"

namespace barely::godot {

class BarelyInstrument : public ::godot::Node {
 public:
  ~BarelyInstrument();

  void set_note_off(float pitch);
  void set_note_on(float pitch);

  void _ready() override;
  void _process(double delta) override;

 private:
  GDCLASS(BarelyInstrument, ::godot::Node);
  static void _bind_methods();

  uint32_t instrument_id_ = 0;
};

}  // namespace barely::godot

#endif  // BARELYMUSICIAN_GODOT_INSTRUMENT_H_
