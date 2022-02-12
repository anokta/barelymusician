#include "barelymusician/engine/instrument_processor.h"

#include <cassert>
#include <map>
#include <variant>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/visitor.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

InstrumentProcessor::InstrumentProcessor(
    const BarelyInstrumentDefinition& definition, int sample_rate) noexcept
    : create_callback_(definition.create_callback),
      destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      set_parameter_callback_(definition.set_parameter_callback),
      gain_(1.0f),
      sample_rate_(sample_rate) {
  assert(sample_rate_ >= 0);
  if (create_callback_) {
    create_callback_(&state_, sample_rate_);
  }
  if (set_parameter_callback_) {
    for (int index = 0; index < definition.num_parameter_definitions; ++index) {
      set_parameter_callback_(
          &state_, index,
          definition.parameter_definitions[index].default_value);
    }
  }
}

InstrumentProcessor::~InstrumentProcessor() noexcept {
  destroy_callback_(&state_);
}

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
      if (process_callback_) {
        process_callback_(&state_, &output[num_output_channels * frame],
                          num_output_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        Visitor{[this](const SetDataEvent& set_data_event) noexcept {
                  if (set_data_callback_) {
                    set_data_callback_(&state_, set_data_event.data);
                  }
                },
                [this](const SetGainEvent& set_gain_event) noexcept {
                  gain_ = set_gain_event.gain;
                },
                [this](const SetParameterEvent& set_parameter_event) noexcept {
                  if (set_parameter_callback_) {
                    set_parameter_callback_(&state_, set_parameter_event.index,
                                            set_parameter_event.value);
                  }
                },
                [this](const StartNoteEvent& start_note_event) noexcept {
                  if (set_note_on_callback_) {
                    set_note_on_callback_(&state_, start_note_event.pitch,
                                          start_note_event.intensity);
                  }
                },
                [this](const StopNoteEvent& stop_note_event) noexcept {
                  if (set_note_off_callback_) {
                    set_note_off_callback_(&state_, stop_note_event.pitch);
                  }
                }},
        it->second);
  }
  events_.erase(begin, end);
  // Process the rest of the buffer.
  if (frame < num_output_frames && process_callback_) {
    process_callback_(&state_, &output[num_output_channels * frame],
                      num_output_channels, num_output_frames - frame);
  }
  // TODO(#88): Revisit gain processing.
  for (int i = 0; i < num_output_channels * num_output_frames; ++i) {
    output[i] *= gain_;
  }
}

}  // namespace barelyapi
