#include "barelymusician/composition/performer.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Triggers the given |instrument| to play the given |note|.
//
// @param note Note to play.
// @param instrument Instrument to play.
void PlayNote(const Note& note, Instrument* instrument) {
  if (note.is_on) {
    instrument->NoteOn(note.index, note.intensity);
  } else {
    instrument->NoteOff(note.index);
  }
}

}  // namespace

Performer::Performer(Instrument* instrument) : instrument_(instrument) {
  DCHECK(instrument_);
}

void Performer::Perform(bool is_on, float index, float intensity,
                        int timestamp) {
  note_queue_.Push({is_on, index, intensity, timestamp});
}

void Performer::Process(int timestamp, int num_samples, float* output) {
  int i = 0;
  // Process samples within note events range.
  const auto notes = note_queue_.Pop(timestamp, num_samples);
  for (const auto& note : notes) {
    while (i + timestamp < note.timestamp) {
      // TODO(#21): Note that, this won't handle any returned notes with a
      // timestamp prior to the input |timestamp| *properly*, they will be
      // triggered immediately at the beginning of the buffer.
      output[i++] = instrument_->Next();
    }
    PlayNote(note, instrument_);
  }
  // Process remaining samples.
  while (i < num_samples) {
    output[i++] = instrument_->Next();
  }
}

void Performer::Reset() {
  note_queue_.Reset();
  instrument_->Reset();
}

}  // namespace barelyapi
