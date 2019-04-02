#include "barelymusician/instrument/instrument_player.h"

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const MessageId kPlayNoteId = 0;
const MessageId kStopNoteId = 1;
const MessageId kUpdateFloatParamId = 2;

// Play note message data.
struct PlayNoteData {
  float index;
  float intensity;
};

// Stop note message data.
struct StopNoteData {
  float index;
};

// Update float parameter message data.
struct UpdateFloatParamData {
  ParamId id;
  float value;
};

// Returns new |Message| with the given message |id|, |data| and |timestamp|.
template <typename DataType>
Message BuildMessage(MessageId id, const DataType& data, int timestamp) {
  Message message;
  message.id = id;
  WriteMessageData<DataType>(data, message.data);
  message.timestamp = timestamp;
  return message;
}

}  // namespace

InstrumentPlayer::InstrumentPlayer(Instrument* instrument)
    : instrument_(instrument) {
  DCHECK(instrument_);
}

void InstrumentPlayer::PlayNote(int timestamp, float index, float intensity) {
  message_queue_.Push(
      BuildMessage<PlayNoteData>(kPlayNoteId, {index, intensity}, timestamp));
}

void InstrumentPlayer::StopNote(int timestamp, float index) {
  message_queue_.Push(
      BuildMessage<StopNoteData>(kStopNoteId, {index}, timestamp));
}

void InstrumentPlayer::UpdateFloatParam(int timestamp, ParamId id,
                                        float value) {
  message_queue_.Push(BuildMessage<UpdateFloatParamData>(
      kUpdateFloatParamId, {id, value}, timestamp));
}

void InstrumentPlayer::Process(int timestamp, int num_samples, float* output) {
  DCHECK(output);

  int i = 0;
  // Process samples within message events range.
  const auto messages = message_queue_.Pop(timestamp, num_samples);
  for (const auto& message : messages) {
    while (i + timestamp < message.timestamp) {
      // TODO(#21): Note that, this won't handle any messages with a timestamp
      // prior to the input |timestamp| *properly*, they will be triggered
      // immediately at the beginning of the buffer.
      output[i++] = instrument_->Next();
    }
    // Process the message.
    switch (message.id) {
      case kPlayNoteId: {
        const auto play_note = ReadMessageData<PlayNoteData>(message.data);
        instrument_->NoteOn(play_note.index, play_note.intensity);
      } break;
      case kStopNoteId: {
        const auto stop_note = ReadMessageData<StopNoteData>(message.data);
        instrument_->NoteOff(stop_note.index);
      } break;
      case kUpdateFloatParamId: {
        const auto update_float_param =
            ReadMessageData<UpdateFloatParamData>(message.data);
        instrument_->SetFloatParam(update_float_param.id,
                                   update_float_param.value);
      } break;
      default:
        DLOG(ERROR) << "Unknown message ID: " << message.id;
        break;
    }
  }
  // Process remaining samples.
  while (i < num_samples) {
    output[i++] = instrument_->Next();
  }
}

void InstrumentPlayer::Reset() {
  message_queue_.Reset();
  instrument_->Reset();
}

}  // namespace barelyapi
