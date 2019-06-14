#include "barelymusician/instrument/instrument_utils.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/message/message.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Middle A note (A4) frequency.
const float kFrequencyA4 = 440.0f;

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

// Processes the given instrument |message|.
//
// @param message Message to process.
// @param instrument Instrument to process the message for.
void ProcessMessage(const Message& message, Instrument* instrument) {
  DCHECK(instrument);
  switch (message.id) {
    case kNoteOffId: {
      const NoteOffData note_off = ReadMessageData<NoteOffData>(message.data);
      instrument->NoteOff(note_off.index);
    } break;
    case kNoteOnId: {
      const NoteOnData note_on = ReadMessageData<NoteOnData>(message.data);
      instrument->NoteOn(note_on.index, note_on.intensity);
    } break;
    default:
      DLOG(ERROR) << "Invalid message ID: " << message.id;
      break;
  }
}

}  // namespace

float FrequencyFromNoteIndex(float index) {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ ((i - iA4) / 12).
  return kFrequencyA4 * std::pow(2.0f, (index - kNoteIndexA4) / kNumSemitones);
}

void Process(Instrument* instrument, MessageQueue* message_queue, float* output,
             int num_channels, int num_frames) {
  DCHECK(instrument);
  DCHECK(message_queue);
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  int frame = 0;
  // Process messages.
  Message message;
  while (message_queue->Pop(num_frames, &message)) {
    const int timestamp = message.timestamp;
    if (frame < timestamp) {
      instrument->Process(&output[num_channels * frame], num_channels,
                          timestamp - frame);
      frame = timestamp;
    }
    ProcessMessage(message, instrument);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument->Process(&output[num_channels * frame], num_channels,
                        num_frames - frame);
  }
}
void PushNoteOffMessage(float index, int timestamp,
                        MessageQueue* message_queue) {
  message_queue->Push(
      BuildMessage<NoteOffData>(kNoteOffId, {index}, timestamp));
}

void PushNoteOnMessage(float index, float intensity, int timestamp,
                       MessageQueue* message_queue) {
  message_queue->Push(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, timestamp));
}

}  // namespace barelyapi
