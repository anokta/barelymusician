#include "barelymusician/platform/unity/unity_instrument.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace unity {

UnityInstrument::UnityInstrument(ClearCallback&& clear_callback,
                                 NoteOffCallback&& note_off_callback,
                                 NoteOnCallback&& note_on_callback,
                                 ProcessCallback&& process_callback)
    : clear_callback_(std::move(clear_callback)),
      note_off_callback_(std::move(note_off_callback)),
      note_on_callback_(std::move(note_on_callback)),
      process_callback_(std::move(process_callback)) {
  DCHECK(clear_callback_);
  DCHECK(note_off_callback_);
  DCHECK(note_on_callback_);
  DCHECK(process_callback_);
}

void UnityInstrument::Clear() { clear_callback_(); }

void UnityInstrument::NoteOff(float index) { note_off_callback_(index); };

void UnityInstrument::NoteOn(float index, float intensity) {
  note_on_callback_(index, intensity);
}

void UnityInstrument::Process(float* output, int num_channels, int num_frames) {
  DCHECK(output);
  process_callback_(output, num_channels * num_frames, num_channels);
}

}  // namespace unity
}  // namespace barelyapi
