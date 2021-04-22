#include "barelymusician/engine/instrument_processor.h"

#include <utility>
#include <variant>

#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(int sample_rate,
                                         InstrumentDefinition definition)
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
  // Process *all* events before the end timestamp.
  const auto begin = events_.cbegin();
  const auto end = events_.lower_bound(
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
    std::visit(InstrumentEventVisitor{
                   [this](const SetCustomData& custom_data) {
                     if (definition_.set_custom_data_fn) {
                       definition_.set_custom_data_fn(&state_,
                                                      custom_data.data);
                     }
                   },
                   [this](const SetNoteOff& note_off) {
                     if (definition_.set_note_off_fn) {
                       definition_.set_note_off_fn(&state_, note_off.pitch);
                     }
                   },
                   [this](const SetNoteOn& note_on) {
                     if (definition_.set_note_on_fn) {
                       definition_.set_note_on_fn(&state_, note_on.pitch,
                                                  note_on.intensity);
                     }
                   },
                   [this](const SetParam& param) {
                     if (definition_.set_param_fn) {
                       definition_.set_param_fn(&state_, param.id, param.value);
                     }
                   }},
               it->second);
  }
  events_.erase(begin, end);
  // Process the rest of the buffer.
  if (frame < num_frames && definition_.process_fn) {
    definition_.process_fn(&state_, &output[num_channels * frame], num_channels,
                           num_frames - frame);
  }
}

void InstrumentProcessor::Reset(int sample_rate) {
  if (definition_.destroy_fn) {
    definition_.destroy_fn(&state_);
  }
  sample_rate_ = sample_rate;
  if (definition_.create_fn) {
    definition_.create_fn(&state_, sample_rate_);
  }
}

void InstrumentProcessor::Schedule(InstrumentProcessorEvents events) {
  events_.merge(events);
}

}  // namespace barelyapi
