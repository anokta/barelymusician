#ifndef UNITY_NATIVE_UNITY_INSTRUMENT_H_
#define UNITY_NATIVE_UNITY_INSTRUMENT_H_

#include <functional>

#include "barelymusician/engine/instrument.h"

namespace barelyapi {
namespace unity {

// Unity instrument.
class UnityInstrument : public Instrument {
 public:
  // Note off function signature.
  using NoteOffFn = std::function<void(float index)>;

  // Note on function signature.
  using NoteOnFn = std::function<void(float index, float intensity)>;

  // Process function signature.
  using ProcessFn =
      std::function<void(float* output, int size, int num_channels)>;

  // Constructs new |UnityInstrument|.
  //
  // @param note_off_fn Note off function.
  // @param note_on_fn Note on function.
  // @param process_fn Process function.
  UnityInstrument(NoteOffFn&& note_off_fn, NoteOnFn&& note_on_fn,
                  ProcessFn&& process_fn);

  // Implements |Instrument|.
  void Control(int, float) override {}
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

 private:
  // Note off function.
  NoteOffFn note_off_fn_;

  // Note on function.
  NoteOnFn note_on_fn_;

  // Process function.
  ProcessFn process_fn_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // UNITY_NATIVE_UNITY_INSTRUMENT_H_
