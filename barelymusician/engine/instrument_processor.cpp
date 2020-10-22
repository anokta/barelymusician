#include "barelymusician/engine/instrument_processor.h"

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(
    std::unique_ptr<Instrument> instrument,
    const std::vector<std::pair<int, float>>& params)
    : instrument_(std::move(instrument)) {
  DCHECK(instrument_);
  for (const auto& [id, default_value] : params) {
    instrument_->Control(id, default_value);
  }
}
void InstrumentProcessor::AllNotesOff() {
  messages_.Clear();
  instrument_->AllNotesOff();
}

void InstrumentProcessor::Message(double timestamp, Message::Data data) {
  messages_.Push(timestamp, std::move(data));
}

void InstrumentProcessor::NoteOff(double timestamp, float index) {
  messages_.Push(timestamp, NoteOffData{index});
}

void InstrumentProcessor::NoteOn(double timestamp, float index,
                                 float intensity) {
  messages_.Push(timestamp, NoteOnData{index, intensity});
}

void InstrumentProcessor::Process(double begin_timestamp, double end_timestamp,
                                  float* output, int num_channels,
                                  int num_frames) {
  int frame = 0;
  // Process messages.
  if (begin_timestamp < end_timestamp) {
    // Include *all* messages before |end_timestamp|.
    const auto messages = messages_.GetIterator(end_timestamp);
    const double frame_rate =
        static_cast<double>(num_frames) / (end_timestamp - begin_timestamp);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      int message_frame =
          static_cast<int>(frame_rate * (it->timestamp - begin_timestamp));
      if (frame < message_frame) {
        instrument_->Process(&output[num_channels * frame], num_channels,
                             message_frame - frame);
        frame = message_frame;
      }
      std::visit(MessageVisitor{[this](const ControlData& data) {
                                  instrument_->Control(data.id, data.value);
                                },
                                [this](const NoteOffData& data) {
                                  instrument_->NoteOff(data.index);
                                },
                                [this](const NoteOnData& data) {
                                  instrument_->NoteOn(data.index,
                                                      data.intensity);
                                }},
                 it->data);
    }
    messages_.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument_->Process(&output[num_channels * frame], num_channels,
                         num_frames - frame);
  }
}

void InstrumentProcessor::SetParam(double timestamp, int id, float value) {
  messages_.Push(timestamp, ControlData{id, value});
}

}  // namespace barelyapi
