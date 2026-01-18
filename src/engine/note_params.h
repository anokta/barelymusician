#ifndef BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_
#define BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_

namespace barely {

struct NoteParams {
  float gain = 1.0f;
  float osc_increment = 0.0f;
  float slice_increment = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_NOTE_PARAMS_H_
