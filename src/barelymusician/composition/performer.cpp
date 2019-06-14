#include "barelymusician/composition/performer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

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
  int frame = 0;
  while (frame < num_frames) {
    if (message_queue_.Pop(num_frames, &temp_message_)) {
      const int timestamp = temp_message_.timestamp;
      if (frame < timestamp) {
        instrument_->Process(&output[num_channels * frame], num_channels,
                             timestamp - frame);
        frame = timestamp;
      }
      ProcessMessage(temp_message_);
    } else {
      instrument_->Process(&output[num_channels * frame], num_channels,
                           num_frames - frame);
      break;
    }
  }
  message_queue_.Update(num_frames);
}

void Performer::RegisterNoteOffCallback(NoteOffCallback&& note_off_callback) {
  note_off_event_.Register(std::move(note_off_callback));
}

void Performer::RegisterNoteOnCallback(NoteOnCallback&& note_on_callback) {
  note_on_event_.Register(std::move(note_on_callback));
}

void Performer::StartNote(float index, float intensity, int offset_samples) {
  message_queue_.Push(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, offset_samples));
}

void Performer::StopNote(float index, int offset_samples) {
  message_queue_.Push(
      BuildMessage<NoteOffData>(kNoteOffId, {index}, offset_samples));
}

void Performer::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOffId: {
      const auto note_off = ReadMessageData<NoteOffData>(message.data);
      instrument_->NoteOff(note_off.index);
      note_off_event_.Trigger(note_off.index);
    } break;
    case kNoteOnId: {
      const auto note_on = ReadMessageData<NoteOnData>(message.data);
      instrument_->NoteOn(note_on.index, note_on.intensity);
      note_on_event_.Trigger(note_on.index, note_on.intensity);
    } break;
    default:
      DLOG(ERROR) << "Unknown message ID: " << message.id;
      break;
  }
}

}  // namespace barelyapi
