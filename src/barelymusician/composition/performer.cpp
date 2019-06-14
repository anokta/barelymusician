#include "barelymusician/composition/performer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/instrument/instrument_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const int kNoteOffId = 0;
const int kNoteOnId = 1;

// |NoteOff| message data.
struct NoteOffData {
  float index;
};

// |NoteOn| message data.
struct NoteOnData {
  float index;
  float intensity;
};

}  // namespace

Performer::Performer(std::unique_ptr<Instrument> instrument)
    : instrument_(std::move(instrument)) {
  DCHECK(instrument_);
}

void Performer::ClearAllNotes() {
  message_queue_.Clear();
  instrument_->AllNotesOff();
}

void Performer::Process(float* output, int num_channels, int num_frames) {
  barelyapi::Process(instrument_.get(), &message_queue_, output, num_channels,
                     num_frames);
  message_queue_.Update(num_frames);
}

void Performer::RegisterNoteOffCallback(NoteOffCallback&& note_off_callback) {
  note_off_event_.Register(std::move(note_off_callback));
}

void Performer::RegisterNoteOnCallback(NoteOnCallback&& note_on_callback) {
  note_on_event_.Register(std::move(note_on_callback));
}

void Performer::StartNote(float index, float intensity, int offset_samples) {
  PushNoteOnMessage(index, intensity, offset_samples, &message_queue_);
}

void Performer::StopNote(float index, int offset_samples) {
  PushNoteOffMessage(index, offset_samples, &message_queue_);
}

}  // namespace barelyapi
