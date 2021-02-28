#include "barelymusician/engine/instrument_processor.h"

#include <utility>
#include <variant>

namespace barelyapi {

namespace {

// Returns number of samples for the given number of |seconds|.
int SamplesFromSeconds(int sample_rate, double seconds) {
  return static_cast<int>(seconds * static_cast<double>(sample_rate));
}

// Returns number of seconds for the given number of |samples|.
double SecondsFromSamples(int sample_rate, int samples) {
  return sample_rate > 0
             ? static_cast<double>(samples) / static_cast<double>(sample_rate)
             : 0.0;
}

}  // namespace

InstrumentProcessor::InstrumentProcessor(InstrumentDefinition definition,
                                         int sample_rate)
    : sample_rate_(sample_rate), definition_(std::move(definition)) {
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate);
  }
}

InstrumentProcessor::~InstrumentProcessor() {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
}

void InstrumentProcessor::Process(double timestamp, float* output,
                                  int num_channels, int num_frames) {
  int frame = 0;
  // Process *all* events before |end_timestamp|.
  const auto begin = data_.cbegin();
  const auto end = data_.lower_bound(
      timestamp + SecondsFromSamples(sample_rate_, num_frames));
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

void InstrumentProcessor::Reset(int sample_rate) {
  sample_rate_ = sample_rate;
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate_);
  }
}

void InstrumentProcessor::SetData(double timestamp, InstrumentData data) {
  data_.emplace(timestamp, std::move(data));
}

}  // namespace barelyapi
