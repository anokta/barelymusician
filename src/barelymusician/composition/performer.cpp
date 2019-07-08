#include "barelymusician/composition/performer.h"

#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs.
const int kNoteOffId = 1;
const int kNoteOnId = 2;

// Note off message data.
struct NoteOffData {
  float index;
};

// Note on message data.
struct NoteOnData {
  float index;
  float intensity;
};

}  // namespace

Performer::Performer(std::unique_ptr<Instrument> instrument)
    : instrument_(std::move(instrument)) {
  DCHECK(instrument_);
}

Instrument* Performer::GetInstrument() const { return instrument_.get(); }

void Performer::Process(float* output, int num_channels, int num_frames,
                        int timestamp) {
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  int frame = 0;
  // Process messages.
  const auto messages = message_buffer_.GetIterator(timestamp, num_frames);
  for (auto it = messages.begin; it != messages.end; ++it) {
    const int message_frame = it->timestamp - messages.timestamp;
    if (frame < message_frame) {
      instrument_->Process(&output[num_channels * frame], num_channels,
                           message_frame - frame);
      frame = message_frame;
    }
    ProcessMessage(*it);
  }
  message_buffer_.Clear(messages);
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument_->Process(&output[num_channels * frame], num_channels,
                         num_frames - frame);
  }
}

void Performer::StartNote(float index, float intensity, int timestamp) {
  message_buffer_.Push(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, timestamp));
}

void Performer::StopNote(float index, int timestamp) {
  message_buffer_.Push(
      BuildMessage<NoteOffData>(kNoteOffId, {index}, timestamp));
}

void Performer::ProcessMessage(const Message& message) const {
  switch (message.id) {
    case kNoteOffId: {
      const NoteOffData note_off = ReadMessageData<NoteOffData>(message.data);
      instrument_->NoteOff(note_off.index);
    } break;
    case kNoteOnId: {
      const NoteOnData note_on = ReadMessageData<NoteOnData>(message.data);
      instrument_->NoteOn(note_on.index, note_on.intensity);
    } break;
    default:
      DLOG(ERROR) << "Invalid message ID: " << message.id;
      break;
  }
}

}  // namespace barelyapi
