#include "barelymusician/engine/instrument.h"

#include <cassert>
#include <utility>
#include <variant>

#include "barelymusician/engine/event.h"

namespace barelyapi {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const Definition& definition, int frame_rate) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      set_parameter_callback_(definition.set_parameter_callback),
      frame_rate_(frame_rate) {
  assert(frame_rate >= 0);
  parameters_.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    parameters_.emplace_back(definition.parameter_definitions[index]);
  }
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
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
  assert(index >= 0);
  if (index < static_cast<int>(parameters_.size())) {
    return &parameters_[index];
  }
  return nullptr;
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  return pitches_.contains(pitch);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::Process(double* output, int num_output_channels,
                         int num_output_frames, double timestamp) noexcept {
  assert(output || num_output_channels == 0 || num_output_frames == 0);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  // Process *all* events before the end timestamp.
  const double end_timestamp = timestamp + GetSeconds(num_output_frames);
  for (auto* event = event_queue_.GetNext(end_timestamp); event;
       event = event_queue_.GetNext(end_timestamp)) {
    const int message_frame = GetFrames(event->first - timestamp);
    if (frame < message_frame) {
      if (process_callback_) {
        process_callback_(&state_, &output[num_output_channels * frame],
                          num_output_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        EventVisitor{[this](SetDataEvent& set_data_event) noexcept {
                       if (set_data_callback_) {
                         data_.swap(set_data_event.data);
                         set_data_callback_(&state_, data_.data(),
                                            static_cast<int>(data_.size()));
                       }
                     },
                     [this](SetNoteOffEvent& set_note_off_event) noexcept {
                       if (set_note_off_callback_) {
                         set_note_off_callback_(&state_,
                                                set_note_off_event.pitch);
                       }
                     },
                     [this](SetNoteOnEvent& set_note_on_event) noexcept {
                       if (set_note_on_callback_) {
                         set_note_on_callback_(&state_, set_note_on_event.pitch,
                                               set_note_on_event.intensity);
                       }
                     },
                     [this](SetParameterEvent& set_parameter_event) noexcept {
                       if (set_parameter_callback_) {
                         set_parameter_callback_(
                             &state_, set_parameter_event.index,
                             set_parameter_event.value,
                             GetSlopePerFrame(set_parameter_event.slope));
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

void Instrument::ResetAllParameters(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  for (int index = 0; index < static_cast<int>(parameters_.size()); ++index) {
    if (parameters_[index].ResetValue()) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, parameters_[index].GetValue(), 0.0});
    }
  }
}

bool Instrument::ResetParameter(int index, double timestamp) noexcept {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].ResetValue()) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, parameters_[index].GetValue(), 0.0});
    }
    return true;
  }
  return false;
}

void Instrument::SetData(std::vector<std::byte> data,
                         double timestamp) noexcept {
  assert(timestamp >= 0.0);
  event_queue_.Add(timestamp, SetDataEvent{std::move(data)});
}

void Instrument::SetNoteOffCallback(NoteOffCallback callback) noexcept {
  note_off_callback_ = std::move(callback);
}

void Instrument::SetNoteOnCallback(NoteOnCallback callback) noexcept {
  note_on_callback_ = std::move(callback);
}

bool Instrument::SetParameter(int index, double value, double slope,
                              double timestamp) noexcept {
  assert(index >= 0);
  assert(timestamp >= 0.0);
  if (index < static_cast<int>(parameters_.size())) {
    if (parameters_[index].SetValue(value)) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, parameters_[index].GetValue(), slope});
    }
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::StartNote(double pitch, double intensity,
                           double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (pitches_.insert(pitch).second) {
    if (note_on_callback_) {
      note_on_callback_(pitch, intensity, timestamp);
    }
    event_queue_.Add(timestamp, SetNoteOnEvent{pitch, intensity});
  }
}

void Instrument::StopAllNotes(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  for (const double pitch : std::exchange(pitches_, {})) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    event_queue_.Add(timestamp, SetNoteOffEvent{pitch});
  }
}

void Instrument::StopNote(double pitch, double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (pitches_.erase(pitch) > 0) {
    if (note_off_callback_) {
      note_off_callback_(pitch, timestamp);
    }
    event_queue_.Add(timestamp, SetNoteOffEvent{pitch});
  }
}

int Instrument::GetFrames(double seconds) const noexcept {
  return frame_rate_ > 0
             ? static_cast<int>(seconds * static_cast<double>(frame_rate_))
             : 0;
}

double Instrument::GetSeconds(int frames) const noexcept {
  return frame_rate_ > 0
             ? static_cast<double>(frames) / static_cast<double>(frame_rate_)
             : 0.0;
}

double Instrument::GetSlopePerFrame(double slope) const noexcept {
  return frame_rate_ > 0 ? slope / static_cast<double>(frame_rate_) : 0.0;
}

}  // namespace barelyapi
