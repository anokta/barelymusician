#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_

#include <functional>

#include "barelymusician/instrument/instrument.h"

namespace barelyapi {
namespace unity {

class UnityInstrument : public Instrument {
 public:
  // Instrument function signatures.
  using ClearFn = std::function<void()>;
  using NoteOffFn = std::function<void(float)>;
  using NoteOnFn = std::function<void(float, float)>;
  using ProcessFn = std::function<void(float*, int, int)>;

  // Constructs new |UnityInstrument|.
  //
  // @param clear_fn Clear function.
  // @param note_off_fn Note off function.
  // @param note_on_fn Note on function.
  // @param process_fn Process function.
  UnityInstrument(ClearFn&& clear_fn, NoteOffFn&& note_off_fn,
                  NoteOnFn&& note_on_fn, ProcessFn&& process_fn);

  // Implements |Instrument|.
  void Clear() override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

 private:
  // Instrument callbacks.
  ClearFn clear_fn_;
  NoteOffFn note_off_fn_;
  NoteOnFn note_on_fn_;
  ProcessFn process_fn_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
