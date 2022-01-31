#include "barelymusician/engine/instrument_processor.h"

#include <cassert>
#include <map>
#include <variant>

#include "barelymusician/common/visitor.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(InstrumentDefinition definition,
                                         int sample_rate) noexcept
    : create_fn_(definition.create_fn),
      destroy_fn_(definition.destroy_fn),
      process_fn_(definition.process_fn),
      set_data_fn_(definition.set_data_fn),
      set_note_off_fn_(definition.set_note_off_fn),
      set_note_on_fn_(definition.set_note_on_fn),
      set_param_fn_(definition.set_param_fn),
      gain_(1.0f),
      sample_rate_(sample_rate) {
  assert(create_fn_);
  assert(destroy_fn_);
  assert(process_fn_);
  assert(set_data_fn_);
  assert(set_note_off_fn_);
  assert(set_note_on_fn_);
  assert(set_param_fn_);
  assert(sample_rate_ >= 0);
  create_fn_(&state_, sample_rate_);
  const auto& param_definitions = definition.param_definitions;
  for (int i = 0; i < static_cast<int>(param_definitions.size()); ++i) {
    set_param_fn_(&state_, i, param_definitions[i].default_value);
  }
}

InstrumentProcessor::~InstrumentProcessor() noexcept { destroy_fn_(&state_); }

void InstrumentProcessor::AddEvents(
    std::multimap<double, InstrumentEvent> events) noexcept {
  events_.merge(std::move(events));
}

void InstrumentProcessor::Process(float* output, int num_output_channels,
                                  int num_output_frames,
                                  double timestamp) noexcept {
  assert(output);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  // Process *all* events before the end timestamp.
  const auto begin = events_.begin();
  const auto end = events_.lower_bound(
      timestamp + SecondsFromSamples(sample_rate_, num_output_frames));
  for (auto it = begin; it != end; ++it) {
    const int message_frame =
        SamplesFromSeconds(sample_rate_, it->first - timestamp);
    if (frame < message_frame) {
      process_fn_(&state_, &output[num_output_channels * frame],
                  num_output_channels, message_frame - frame);
      frame = message_frame;
    }
    std::visit(Visitor{[this](const SetDataEvent& set_data_event) noexcept {
                         set_data_fn_(&state_, set_data_event.data);
                       },
                       [this](const SetGainEvent& set_gain_event) noexcept {
                         gain_ = set_gain_event.gain;
                       },
                       [this](const SetParamEvent& set_param_event) noexcept {
                         set_param_fn_(&state_, set_param_event.index,
                                       set_param_event.value);
                       },
                       [this](const StartNoteEvent& start_note_event) noexcept {
                         set_note_on_fn_(&state_, start_note_event.pitch,
                                         start_note_event.intensity);
                       },
                       [this](const StopNoteEvent& stop_note_event) noexcept {
                         set_note_off_fn_(&state_, stop_note_event.pitch);
                       }},
               it->second);
  }
  events_.erase(begin, end);
  // Process the rest of the buffer.
  if (frame < num_output_frames) {
    process_fn_(&state_, &output[num_output_channels * frame],
                num_output_channels, num_output_frames - frame);
  }
  // TODO(#88): Revisit gain processing.
  for (int i = 0; i < num_output_channels * num_output_frames; ++i) {
    output[i] *= gain_;
  }
}

}  // namespace barelyapi
