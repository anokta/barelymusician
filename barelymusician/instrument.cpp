#include "barelymusician/instrument.h"

#include <cassert>
#include <utility>
#include <variant>

#include "barelymusician/barelymusician.h"
#include "barelymusician/event.h"

namespace barelyapi {

namespace {

/// Event visitor template.
template <class... EventTypes>
struct EventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
EventVisitor(EventTypes...) -> EventVisitor<EventTypes...>;

}  // namespace

Instrument::Instrument(const BarelyInstrumentDefinition& definition,
                       int sample_rate) noexcept
    : create_callback_(definition.create_callback),
      destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      set_parameter_callback_(definition.set_parameter_callback),
      sample_rate_(sample_rate) {
  assert(sample_rate >= 0);
  parameters_.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    parameters_.emplace_back(definition.parameter_definitions[index]);
  }
  if (create_callback_) {
    create_callback_(&state_, sample_rate_);
  }
  if (set_parameter_callback_) {
    for (int index = 0; index < definition.num_parameter_definitions; ++index) {
      set_parameter_callback_(&state_, index, parameters_[index].GetValue(),
                              0.0);
    }
  }
}

Instrument::~Instrument() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

const Parameter* Instrument::GetParameter(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    return &parameters_[index];
  }
  return nullptr;
}

bool Instrument::IsNoteOn(float pitch) const noexcept {
  return pitches_.contains(pitch);
}

void Instrument::Process(float* output, int num_output_channels,
                         int num_output_frames, double timestamp) noexcept {
  assert(output);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  // Process *all* events before the end timestamp.
  const double end_timestamp = timestamp + GetSeconds(num_output_frames);
  for (const auto* event = events_.GetNext(end_timestamp); event;
       event = events_.GetNext(end_timestamp)) {
    const int message_frame = GetSamples(event->first - timestamp);
    if (frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_, &output[num_output_channels * frame],
                          num_output_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        EventVisitor{
            [this](const SetDataEvent& set_data_event) noexcept {
              if (set_data_callback_) {
                set_data_callback_(&state_, set_data_event.definition.data);
              }
            },
            [this](const SetParameterEvent& set_parameter_event) noexcept {
              if (set_parameter_callback_) {
                set_parameter_callback_(&state_, set_parameter_event.index,
                                        set_parameter_event.value, 0.0);
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
        event->second);
  }
  // Process the rest of the buffer.
  if (frame < num_output_frames && process_callback_) {
    process_callback_(&state_, &output[num_output_channels * frame],
                      num_output_channels, num_output_frames - frame);
  }
}

void Instrument::ProcessEvent(const Event& event, double timestamp) noexcept {
  std::visit(
      EventVisitor{[&](const SetDataEvent& set_data_event) noexcept {
                     SetData(set_data_event.definition, timestamp);
                   },
                   [&](const SetParameterEvent& set_parameter_event) noexcept {
                     SetParameter(set_parameter_event.index,
                                  set_parameter_event.value, timestamp);
                   },
                   [&](const StartNoteEvent& start_note_event) noexcept {
                     StartNote(start_note_event.pitch,
                               start_note_event.intensity, timestamp);
                   },
                   [&](const StopNoteEvent& stop_note_event) noexcept {
                     StopNote(stop_note_event.pitch, timestamp);
                   }},
      event);
}

void Instrument::ResetAllParameters(double timestamp) noexcept {
  for (int index = 0; index < static_cast<int>(parameters_.size()); ++index) {
    if (parameters_[index].ResetValue()) {
      events_.Add(timestamp,
                  SetParameterEvent{index, parameters_[index].GetValue()});
    }
  }
}

bool Instrument::ResetParameter(int index, double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].ResetValue()) {
      events_.Add(timestamp,
                  SetParameterEvent{index, parameters_[index].GetValue()});
    }
    return true;
  }
  return false;
}

void Instrument::SetData(BarelyDataDefinition definition,
                         double timestamp) noexcept {
  void* new_data = nullptr;
  if (definition.move_callback) {
    definition.move_callback(definition.data, &new_data);
  }
  definition.data = new_data;
  events_.Add(timestamp, SetDataEvent{definition});
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

bool Instrument::SetParameter(int index, double value,
                              double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].SetValue(value)) {
      events_.Add(timestamp,
                  SetParameterEvent{index, parameters_[index].GetValue()});
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
    events_.Add(timestamp, StartNoteEvent{pitch, intensity});
  }
}

void Instrument::StopAllNotes(double timestamp) noexcept {
  for (const float pitch : std::exchange(pitches_, {})) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    events_.Add(timestamp, StopNoteEvent{pitch});
  }
}

void Instrument::StopNote(float pitch, double timestamp) noexcept {
  if (pitches_.erase(pitch) > 0) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    events_.Add(timestamp, StopNoteEvent{pitch});
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
