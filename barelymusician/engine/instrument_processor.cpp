#include "barelymusician/engine/instrument_processor.h"

#include <utility>
#include <variant>

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(InstrumentDefinition definition)
    : definition_(std::move(definition)), state_(nullptr) {
  if (definition_.create_fn) {
    definition_.create_fn(&state_);
  }
}

InstrumentProcessor::~InstrumentProcessor() {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
}

InstrumentProcessor::InstrumentProcessor(InstrumentProcessor&& other) noexcept
    : definition_(std::exchange(other.definition_, {})),
      state_(std::exchange(other.state_, nullptr)),
      data_(std::move(other.data_)) {}

InstrumentProcessor& InstrumentProcessor::operator=(
    InstrumentProcessor&& other) noexcept {
  if (this != &other) {
    std::swap(definition_, other.definition_);
    std::swap(state_, other.state_);
    data_ = std::move(other.data_);
  }
  return *this;
}

void InstrumentProcessor::Process(int64 timestamp, float* output,
                                  int num_channels, int num_frames) {
  int frame = 0;
  // Process *all* events before |end_timestamp|.
  const auto begin = data_.cbegin();
  const auto end =
      data_.lower_bound(timestamp + static_cast<int64>(num_frames));
  for (auto it = begin; it != end; ++it) {
    const int message_frame = static_cast<int>(it->first - timestamp);
    if (frame < message_frame) {
      if (definition_.process_fn) {
        definition_.process_fn(&state_, &output[num_channels * frame],
                               num_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(InstrumentDataVisitor{
                   [this](const CustomData& custom_data) {
                     if (definition_.set_custom_data_fn) {
                       definition_.set_custom_data_fn(&state_,
                                                      custom_data.data);
                     }
                   },
                   [this](const NoteOff& note_off) {
                     if (definition_.set_note_off_fn) {
                       definition_.set_note_off_fn(&state_, note_off.pitch);
                     }
                   },
                   [this](const NoteOn& note_on) {
                     if (definition_.set_note_on_fn) {
                       definition_.set_note_on_fn(&state_, note_on.pitch,
                                                  note_on.intensity);
                     }
                   },
                   [this](const Param& param) {
                     if (definition_.set_param_fn) {
                       definition_.set_param_fn(&state_, param.id, param.value);
                     }
                   }},
               it->second);
  }
  data_.erase(begin, end);
  // Process the rest of the buffer.
  if (frame < num_frames && definition_.process_fn) {
    definition_.process_fn(&state_, &output[num_channels * frame], num_channels,
                           num_frames - frame);
  }
}

void InstrumentProcessor::SetData(int64 timestamp, InstrumentData data) {
  data_.emplace(timestamp, std::move(data));
}

}  // namespace barelyapi
