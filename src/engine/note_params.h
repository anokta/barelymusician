#ifndef BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_

namespace barely {

struct NoteParams {
  // Gain in linear amplitude.
  float gain = 1.0f;

  // Oscillator increment per sample.
  float osc_increment = 0.0f;

  // Slice increment per sample.
  float slice_increment = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_
