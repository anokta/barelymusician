#include "barelymusician/platform/unity/unity_instrument.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace unity {

UnityInstrument::UnityInstrument(NoteOffFn&& note_off_fn, NoteOnFn&& note_on_fn,
                                 ProcessFn&& process_fn)
    : note_off_fn_(std::move(note_off_fn)),
      note_on_fn_(std::move(note_on_fn)),
      process_fn_(std::move(process_fn)) {
  DCHECK(note_off_fn_);
  DCHECK(note_on_fn_);
  DCHECK(process_fn_);
}

void UnityInstrument::NoteOff(float index) { note_off_fn_(index); };

void UnityInstrument::NoteOn(float index, float intensity) {
  note_on_fn_(index, intensity);
}

void UnityInstrument::Process(float* output, int num_channels, int num_frames) {
  process_fn_(output, num_channels * num_frames, num_channels);
}

}  // namespace unity
}  // namespace barelyapi
