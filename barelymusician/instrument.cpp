#include "barelymusician/instrument.h"

#include <cassert>
#include <map>
#include <utility>
#include <variant>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instrument_event.h"
#include "barelymusician/visitor.h"

namespace barelyapi {

namespace {

// Maximum number of tasks to be executed per each `Process` call.
constexpr int kNumMaxTasks = 1000;

}  // namespace

Instrument::Instrument(const BarelyInstrumentDefinition& definition,
                       int sample_rate) noexcept
    : runner_(kNumMaxTasks),
      create_callback_(definition.create_callback),
      destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      set_parameter_callback_(definition.set_parameter_callback),
      sample_rate_(sample_rate) {
  assert(sample_rate_ >= 0);
  parameters_.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    parameters_.emplace_back(definition.parameter_definitions[index]);
  }
  if (create_callback_) {
    create_callback_(&state_, sample_rate_);
  }
  if (set_parameter_callback_) {
    for (int index = 0; index < definition.num_parameter_definitions; ++index) {
      set_parameter_callback_(&state_, index, parameters_[index].GetValue());
    }
  }
}

Instrument::~Instrument() {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

float Instrument::GetGain() const noexcept { return gain_; }

const Parameter* Instrument::GetParameter(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    return &parameters_[index];
  }
  return nullptr;
}

bool Instrument::IsMuted() const noexcept { return is_muted_; }

bool Instrument::IsNoteOn(float pitch) const noexcept {
  return pitches_.contains(pitch);
}

void Instrument::Process(float* output, int num_output_channels,
                         int num_output_frames, double timestamp) noexcept {
  runner_.Run();
  assert(output);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  // Process *all* events before the end timestamp.
  const auto begin = events_.begin();
  const auto end = std::lower_bound(
      begin, events_.end(), timestamp + GetSeconds(num_output_frames),
      [](const std::pair<double, InstrumentEvent>& event, double timestamp) {
        return event.first < timestamp;
      });
  for (auto it = begin; it != end; ++it) {
    const int message_frame = GetSamples(it->first - timestamp);
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
                  processor_gain_ = set_gain_event.gain;
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
  if (processor_gain_ != 1.0f) {
    for (int i = 0; i < num_output_channels * num_output_frames; ++i) {
      output[i] *= processor_gain_;
    }
  }
}

void Instrument::ProcessEvent(const InstrumentEvent& event,
                              double timestamp) noexcept {
  std::visit(
      Visitor{[&](const SetDataEvent& set_data_event) noexcept {
                SetData(set_data_event.data, timestamp);
              },
              [&](const SetGainEvent& set_gain_event) noexcept {
                SetGain(set_gain_event.gain, timestamp);
              },
              [&](const SetParameterEvent& set_parameter_event) noexcept {
                SetParameter(set_parameter_event.index,
                             set_parameter_event.value, timestamp);
              },
              [&](const StartNoteEvent& start_note_event) noexcept {
                StartNote(start_note_event.pitch, start_note_event.intensity,
                          timestamp);
              },
              [&](const StopNoteEvent& stop_note_event) noexcept {
                StopNote(stop_note_event.pitch, timestamp);
              }},
      event);
}

void Instrument::ResetAllParameters(double timestamp) noexcept {
  for (int index = 0; index < static_cast<int>(parameters_.size()); ++index) {
    if (parameters_[index].ResetValue()) {
      runner_.Add(
          [this, timestamp, index, value = parameters_[index].GetValue()]() {
            events_.emplace_back(timestamp, SetParameterEvent{index, value});
          });
    }
  }
}

bool Instrument::ResetParameter(int index, double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].ResetValue()) {
      runner_.Add(
          [this, timestamp, index, value = parameters_[index].GetValue()]() {
            events_.emplace_back(timestamp, SetParameterEvent{index, value});
          });
    }
    return true;
  }
  return false;
}

void Instrument::SetData(void* data, double timestamp) noexcept {
  runner_.Add([this, timestamp, data]() {
    events_.emplace_back(timestamp, SetDataEvent{data});
  });
}

void Instrument::SetGain(float gain, double timestamp) noexcept {
  if (gain_ != gain) {
    gain_ = gain;
    if (!is_muted_) {
      runner_.Add([this, timestamp, gain = gain_]() {
        events_.emplace_back(timestamp, SetGainEvent{gain});
      });
    }
  }
}

void Instrument::SetMuted(bool is_muted, double timestamp) noexcept {
  if (is_muted_ != is_muted) {
    is_muted_ = is_muted;
    runner_.Add([this, timestamp, gain = is_muted_ ? 0.0f : gain_]() {
      events_.emplace_back(timestamp, SetGainEvent{gain});
    });
  }
}

void Instrument::SetNoteOffCallback(
    BarelyInstrument_NoteOffCallback note_off_callback,
    void* user_data) noexcept {
  if (note_off_callback) {
    note_off_callback_ = [note_off_callback, user_data](float pitch,
                                                        double timestamp) {
      if (note_off_callback) {
        note_off_callback(pitch, timestamp, user_data);
      }
    };
  } else {
    note_off_callback_ = nullptr;
  }
}

void Instrument::SetNoteOnCallback(
    BarelyInstrument_NoteOnCallback note_on_callback,
    void* user_data) noexcept {
  if (note_on_callback) {
    note_on_callback_ = [note_on_callback, user_data](
                            float pitch, float intensity, double timestamp) {
      note_on_callback(pitch, intensity, timestamp, user_data);
    };
  } else {
    note_on_callback_ = nullptr;
  }
}

bool Instrument::SetParameter(int index, float value,
                              double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].SetValue(value)) {
      runner_.Add(
          [this, timestamp, index, value = parameters_[index].GetValue()]() {
            events_.emplace_back(timestamp, SetParameterEvent{index, value});
          });
    }
    return true;
  }
  return false;
}

void Instrument::StartNote(float pitch, float intensity,
                           double timestamp) noexcept {
  if (pitches_.insert(pitch).second) {
    if (note_on_callback_) {
      note_on_callback_(pitch, intensity, timestamp);
    }
    runner_.Add([this, timestamp, pitch, intensity]() {
      events_.emplace_back(timestamp, StartNoteEvent{pitch, intensity});
    });
  }
}

void Instrument::StopAllNotes(double timestamp) noexcept {
  for (const float pitch : std::exchange(pitches_, {})) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    runner_.Add([this, timestamp, pitch]() {
      events_.emplace_back(timestamp, StopNoteEvent{pitch});
    });
  }
}

void Instrument::StopNote(float pitch, double timestamp) noexcept {
  if (pitches_.erase(pitch) > 0) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    runner_.Add([this, timestamp, pitch]() {
      events_.emplace_back(timestamp, StopNoteEvent{pitch});
    });
  }
}

int Instrument::GetSamples(double seconds) const noexcept {
  return sample_rate_ > 0
             ? static_cast<int>(seconds * static_cast<double>(sample_rate_))
             : 0;
}

double Instrument::GetSeconds(int samples) const noexcept {
  return sample_rate_ > 0
             ? static_cast<double>(samples) / static_cast<double>(sample_rate_)
             : 0.0;
}

}  // namespace barelyapi
