#include "barelymusician/instrument/instrument.h"

#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs.
constexpr int kNoteOffId = 1;
constexpr int kNoteOnId = 2;

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

void Instrument::NoteOffScheduled(float index, int timestamp) {
  message_buffer_.Push(
      BuildMessage<NoteOffData>(kNoteOffId, {index}, timestamp));
}

void Instrument::NoteOnScheduled(float index, float intensity, int timestamp) {
  message_buffer_.Push(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, timestamp));
}

void Instrument::ProcessScheduled(float* output, int num_channels,
                                  int num_frames, int timestamp) {
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  DCHECK_GE(timestamp, 0);
  int frame = 0;
  // Process messages.
  const auto messages = message_buffer_.GetIterator(timestamp, num_frames);
  for (auto it = messages.cbegin; it != messages.cend; ++it) {
    const int message_frame = it->timestamp - messages.timestamp;
    if (frame < message_frame) {
      Process(&output[num_channels * frame], num_channels,
              message_frame - frame);
      frame = message_frame;
    }
    ProcessMessage(*it);
  }
  message_buffer_.Clear(messages);
  // Process the rest of the buffer.
  if (frame < num_frames) {
    Process(&output[num_channels * frame], num_channels, num_frames - frame);
  }
}

void Instrument::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOffId: {
      const NoteOffData note_off = ReadMessageData<NoteOffData>(message.data);
      NoteOff(note_off.index);
    } break;
    case kNoteOnId: {
      const NoteOnData note_on = ReadMessageData<NoteOnData>(message.data);
      NoteOn(note_on.index, note_on.intensity);
    } break;
    default:
      DLOG(ERROR) << "Invalid message ID: " << message.id;
      break;
  }
}

}  // namespace barelyapi
