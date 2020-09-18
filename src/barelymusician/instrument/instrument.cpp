#include "barelymusician/instrument/instrument.h"

#include "barelymusician/engine/message.h"

namespace barelyapi {

void Instrument::NoteOffScheduled(double timestamp, float index) {
  messages_.Push(timestamp, NoteOffData{index});
}

void Instrument::NoteOnScheduled(double timestamp, float index,
                                 float intensity) {
  messages_.Push(timestamp, NoteOnData{index, intensity});
}

void Instrument::ProcessScheduled(double begin_timestamp, double end_timestamp,
                                  float* output, int num_channels,
                                  int num_frames) {
  int frame = 0;
  // Process mmessages.
  if (begin_timestamp < end_timestamp) {
    const auto messages = messages_.GetIterator(begin_timestamp, end_timestamp);
    const double frame_rate =
        static_cast<double>(num_frames) / (end_timestamp - begin_timestamp);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      int message_frame =
          static_cast<int>(frame_rate * (it->position - begin_timestamp));
      if (frame < message_frame) {
        Process(&output[num_channels * frame], num_channels,
                message_frame - frame);
        frame = message_frame;
      }
      std::visit(MessageVisitor{[this](const NoteOffData& note_off_data) {
                                  NoteOff(note_off_data.index);
                                },
                                [this](const NoteOnData& note_on_data) {
                                  NoteOn(note_on_data.index,
                                         note_on_data.intensity);
                                }},
                 it->data);
    }
    messages_.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    Process(&output[num_channels * frame], num_channels, num_frames - frame);
  }
}

}  // namespace barelyapi
