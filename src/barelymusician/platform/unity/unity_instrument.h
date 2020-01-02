#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_

#include <functional>

#include "barelymusician/instrument/instrument.h"

namespace barelyapi {
namespace unity {

class UnityInstrument : public Instrument {
 public:
  // Instrument function signatures.
  using AllNotesOffFn = std::function<void()>;
  using NoteOffFn = std::function<void(float index)>;
  using NoteOnFn = std::function<void(float index, float intensity)>;
  using ProcessFn =
      std::function<void(float* output, int num_channels, int num_frames)>;

  // Constructs new |UnityInstrument|.
  //
  // @param all_notes_off_fn All notes off function.
  // @param note_off_fn Note off function.
  // @param note_on_fn Note on function.
  // @param process_fn Process function.
  UnityInstrument(AllNotesOffFn&& all_notes_off_fn, NoteOffFn&& note_off_fn,
                  NoteOnFn&& note_on_fn, ProcessFn&& process_fn);

  // Implements |Instrument|.
  void AllNotesOff() override;
  void NoteOff(float index) override;
  void NoteOn(float index, float intensity) override;
  void Process(float* output, int num_channels, int num_frames) override;

 private:
  // Instrument callbacks.
  AllNotesOffFn all_notes_off_fn_;
  NoteOffFn note_off_fn_;
  NoteOnFn note_on_fn_;
  ProcessFn process_fn_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_INSTRUMENT_H_
