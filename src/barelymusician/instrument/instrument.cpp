#include "barelymusician/instrument/instrument.h"

#include <variant>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Instrument message processor.
struct MessageProcessor {
  // Processes |NoteOffData|.
  void operator()(const NoteOffData& note_off_data) {
    instrument->NoteOff(note_off_data.index);
  }

  // Processes |NoteOnData|.
  void operator()(const NoteOnData& note_on_data) {
    instrument->NoteOn(note_on_data.index, note_on_data.intensity);
  }

  // Instrument to process.
  Instrument* instrument;
};

}  // namespace

void Instrument::NoteOffScheduled(float index, int timestamp) {
  message_buffer_.Push({NoteOffData{index}, timestamp});
}

void Instrument::NoteOnScheduled(float index, float intensity, int timestamp) {
  message_buffer_.Push({NoteOnData{index, intensity}, timestamp});
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
    std::visit(MessageProcessor{this}, it->data);
  }
  message_buffer_.Clear(messages);
  // Process the rest of the buffer.
  if (frame < num_frames) {
    Process(&output[num_channels * frame], num_channels, num_frames - frame);
  }
}

}  // namespace barelyapi
