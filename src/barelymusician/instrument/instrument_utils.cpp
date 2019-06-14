#include "barelymusician/instrument/instrument_utils.h"

#include <algorithm>
#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
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

}  // namespace

Message BuildNoteOffMessage(float index, int timestamp) {
  return BuildMessage<NoteOffData>(kNoteOffId, {index}, timestamp);
}

Message BuildNoteOnMessage(float index, float intensity, int timestamp) {
  return BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, timestamp);
}

float FrequencyFromNoteIndex(float index) {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ ((i - iA4) / 12).
  return kFrequencyA4 * std::pow(2.0f, (index - kNoteIndexA4) / kNumSemitones);
}

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

}  // namespace barelyapi
