#include "barelymusician/engine/instrument_processor.h"

#include <algorithm>
#include <variant>

#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(int sample_rate,
                                         InstrumentDefinition definition)
    : sample_rate_(sample_rate),
      definition_(std::move(definition)),
      state_(nullptr) {
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate_);
  }
}

InstrumentProcessor::~InstrumentProcessor() {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
}

InstrumentProcessor::InstrumentProcessor(InstrumentProcessor&& other) noexcept
    : sample_rate_(std::move(other.sample_rate_)),
      definition_(std::exchange(other.definition_, {})),
      state_(std::exchange(other.state_, nullptr)),
      messages_(std::move(other.messages_)) {}

InstrumentProcessor& InstrumentProcessor::operator=(
    InstrumentProcessor&& other) noexcept {
  if (this != &other) {
    sample_rate_ = std::move(other.sample_rate_);
    std::swap(definition_, other.definition_);
    std::swap(state_, other.state_);
    messages_ = std::move(other.messages_);
  }
  return *this;
}

void InstrumentProcessor::Create() {
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate_);
  }
}

void InstrumentProcessor::Destroy() {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
}

void InstrumentProcessor::Process(double timestamp, float* output,
                                  int num_channels, int num_frames) {
  int frame = 0;
  // Process *all* messages before |end_timestamp|.
  const double end_timestamp =
      timestamp + SecondsFromSamples(sample_rate_, num_frames);
  const auto begin = messages_.cbegin();
  const auto end =
      std::lower_bound(begin, messages_.cend(), end_timestamp,
                       [](const auto& message, double message_timestamp) {
                         return message.first < message_timestamp;
                       });
  for (auto it = begin; it != end; ++it) {
    const int message_frame =
        SamplesFromSeconds(sample_rate_, it->first - timestamp);
    if (frame < message_frame) {
      if (definition_.process_fn) {
        definition_.process_fn(&state_, &output[num_channels * frame],
                               num_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(MessageDataVisitor{[this](const NoteOffData& note_off_data) {
                                    SetNoteOff(note_off_data.index);
                                  },
                                  [this](const NoteOnData& note_on_data) {
                                    SetNoteOn(note_on_data.index,
                                              note_on_data.intensity);
                                  }},
               it->second);
  }
  messages_.erase(begin, end);
  // Process the rest of the buffer.
  if (frame < num_frames && definition_.process_fn) {
    definition_.process_fn(&state_, &output[num_channels * frame], num_channels,
                           num_frames - frame);
  }
}

void InstrumentProcessor::ScheduleNoteOff(double timestamp, float note_index) {
  messages_.emplace_back(timestamp, NoteOffData{note_index});
}

void InstrumentProcessor::ScheduleNoteOn(double timestamp, float note_index,
                                         float note_intensity) {
  messages_.emplace_back(timestamp, NoteOnData{note_index, note_intensity});
}

void InstrumentProcessor::SetCustomData(void* custom_data) {
  if (definition_.set_custom_data_fn) {
    definition_.set_custom_data_fn(&state_, custom_data);
  }
}

void InstrumentProcessor::SetNoteOff(float note_index) {
  if (definition_.set_note_off_fn) {
    definition_.set_note_off_fn(&state_, note_index);
  }
}

void InstrumentProcessor::SetNoteOn(float note_index, float note_intensity) {
  if (definition_.set_note_on_fn) {
    definition_.set_note_on_fn(&state_, note_index, note_intensity);
  }
}

void InstrumentProcessor::SetParam(int param_id, float param_value) {
  if (definition_.set_param_fn) {
    definition_.set_param_fn(&state_, param_id, param_value);
  }
}

}  // namespace barelyapi
