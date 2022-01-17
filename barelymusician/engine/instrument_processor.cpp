#include "barelymusician/engine/instrument_processor.h"

#include <algorithm>
#include <map>
#include <variant>

#include "barelymusician/common/visitor.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

namespace {

// Dummy create function that does nothing.
void NoopCreateFn(void** /*state*/, int /*sample_rate*/) noexcept {}

// Dummy destroy function that does nothing.
void NoopDestroyFn(void** /*state*/) noexcept {}

// Dummy set data function that does nothing.
void NoopSetDataFn(void** /*state*/, void* /*data*/) noexcept {}

// Dummy set note off function that does nothing.
void NoopSetNoteOffFn(void** /*state*/, float /*pitch*/) noexcept {}

// Dummy set instrument note on function that does nothing.
void NoopSetNoteOnFn(void** /*state*/, float /*pitch*/,
                     float /*intensity*/) noexcept {}

// Dummy set instrument parameter function that does nothing.
void NoopSetParamFn(void** /*state*/, int /*index*/, float /*value*/) noexcept {
}

// Process function that fills the output buffer with zeros.
void ZeroFillProcessFn(void** /*state*/, float* output, int num_channels,
                       int num_frames) noexcept {
  std::fill_n(output, num_channels * num_frames, 0.0f);
}

}  // namespace

InstrumentProcessor::InstrumentProcessor(InstrumentDefinition definition,
                                         std::vector<float> param_values,
                                         int sample_rate) noexcept
    : create_fn_(definition.create_fn ? definition.create_fn : &NoopCreateFn),
      destroy_fn_(definition.destroy_fn ? definition.destroy_fn
                                        : &NoopDestroyFn),
      process_fn_(definition.process_fn ? definition.process_fn
                                        : &ZeroFillProcessFn),
      set_data_fn_(definition.set_data_fn ? definition.set_data_fn
                                          : &NoopSetDataFn),
      set_note_off_fn_(definition.set_note_off_fn ? definition.set_note_off_fn
                                                  : &NoopSetNoteOffFn),
      set_note_on_fn_(definition.set_note_on_fn ? definition.set_note_on_fn
                                                : &NoopSetNoteOnFn),
      set_param_fn_(definition.set_param_fn ? definition.set_param_fn
                                            : &NoopSetParamFn),
      gain_(1.0f),
      sample_rate_(sample_rate) {
  create_fn_(&state_, sample_rate_);
  for (int i = 0; i < static_cast<int>(param_values.size()); ++i) {
    set_param_fn_(&state_, i, param_values[i]);
  }
}

InstrumentProcessor::~InstrumentProcessor() noexcept { destroy_fn_(&state_); }

void InstrumentProcessor::MergeEvents(
    std::multimap<double, InstrumentEvent> events) noexcept {
  events_.merge(events);
}

void InstrumentProcessor::Process(float* output, int num_output_channels,
                                  int num_output_frames,
                                  double timestamp) noexcept {
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
    std::visit(Visitor{[this](const SetDataEvent& set_data_event) {
                         set_data_fn_(&state_, set_data_event.data);
                       },
                       [this](const SetGainEvent& set_gain_event) {
                         gain_ = set_gain_event.gain;
                       },
                       [this](const SetParamEvent& set_param_event) {
                         set_param_fn_(&state_, set_param_event.index,
                                       set_param_event.value);
                       },
                       [this](const StartNoteEvent& start_note_event) {
                         set_note_on_fn_(&state_, start_note_event.pitch,
                                         start_note_event.intensity);
                       },
                       [this](const StopNoteEvent& stop_note_event) {
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

void InstrumentProcessor::Reset(std::vector<float> param_values,
                                int sample_rate) noexcept {
  destroy_fn_(&state_);
  sample_rate_ = sample_rate;
  create_fn_(&state_, sample_rate_);
  for (int i = 0; i < static_cast<int>(param_values.size()); ++i) {
    set_param_fn_(&state_, i, param_values[i]);
  }
}

}  // namespace barelyapi
