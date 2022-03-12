#include "barelymusician/instrument/instrument.h"

#include <cassert>
#include <utility>
#include <variant>

#include "barelymusician/api/instrument.h"
#include "barelymusician/instrument/event.h"

namespace barelyapi {

namespace {

// Event visitor template.
template <class... EventTypes>
struct EventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
EventVisitor(EventTypes...) -> EventVisitor<EventTypes...>;

// Returns corresponding frames for given `seconds`.
int FramesFromSeconds(int frame_rate, double seconds) noexcept {
  return frame_rate > 0
             ? static_cast<int>(seconds * static_cast<double>(frame_rate))
             : 0;
}

// Returns corresponding seconds for given `frames`.
double SecondsFromFrames(int frame_rate, int frames) noexcept {
  return frame_rate > 0
             ? static_cast<double>(frames) / static_cast<double>(frame_rate)
             : 0.0;
}

}  // namespace

Instrument::Instrument(const BarelyInstrumentDefinition& definition,
                       int frame_rate) noexcept
    : processor_{definition.destroy_callback, definition.process_callback,
                 definition.set_data_callback, definition.set_note_off_callback,
                 definition.set_note_on_callback} {
  assert(frame_rate >= 0);
  controller_.parameters.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    controller_.parameters.emplace_back(
        definition.parameter_definitions[index]);
  }
  if (definition.create_callback) {
    definition.create_callback(&processor_.state, processor_.frame_rate);
  }
  if (processor_.set_parameter_callback) {
    for (int index = 0; index < definition.num_parameter_definitions; ++index) {
      processor_.set_parameter_callback(
          &processor_.state, index, controller_.parameters[index].GetValue(),
          0.0);
    }
  }
}

Instrument::~Instrument() noexcept {
  if (processor_.destroy_callback) {
    processor_.destroy_callback(&processor_.state);
  }
}

const Parameter* Instrument::GetParameter(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    return &controller_.parameters[index];
  }
  return nullptr;
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  return controller_.pitches.contains(pitch);
}

void Instrument::Process(double* output, int num_output_channels,
                         int num_output_frames, double timestamp) noexcept {
  assert(output);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  // Process *all* events before the end timestamp.
  const double end_timestamp =
      timestamp + SecondsFromFrames(processor_.frame_rate, num_output_frames);
  for (const auto* event = event_queue_.GetNext(end_timestamp); event;
       event = event_queue_.GetNext(end_timestamp)) {
    const int message_frame =
        FramesFromSeconds(processor_.frame_rate, event->first - timestamp);
    if (frame < message_frame) {
      if (processor_.process_callback) {
        processor_.process_callback(&processor_.state,
                                    &output[num_output_channels * frame],
                                    num_output_channels, message_frame - frame);
      }
      frame = message_frame;
    }
    std::visit(
        EventVisitor{
            [this](const SetDataEvent& set_data_event) noexcept {
              if (processor_.set_data_callback) {
                processor_.set_data_callback(&processor_.state,
                                             set_data_event.definition.data);
              }
            },
            [this](const SetParameterEvent& set_parameter_event) noexcept {
              if (processor_.set_parameter_callback) {
                processor_.set_parameter_callback(
                    &processor_.state, set_parameter_event.index,
                    set_parameter_event.value,
                    set_parameter_event.slope /
                        static_cast<double>(processor_.frame_rate));
              }
            },
            [this](const StartNoteEvent& start_note_event) noexcept {
              if (processor_.set_note_on_callback) {
                processor_.set_note_on_callback(&processor_.state,
                                                start_note_event.pitch,
                                                start_note_event.intensity);
              }
            },
            [this](const StopNoteEvent& stop_note_event) noexcept {
              if (processor_.set_note_off_callback) {
                processor_.set_note_off_callback(&processor_.state,
                                                 stop_note_event.pitch);
              }
            }},
        event->second);
  }
  // Process the rest of the buffer.
  if (frame < num_output_frames && processor_.process_callback) {
    processor_.process_callback(&processor_.state,
                                &output[num_output_channels * frame],
                                num_output_channels, num_output_frames - frame);
  }
}

void Instrument::ResetAllParameters(double timestamp) noexcept {
  for (int index = 0; index < static_cast<int>(controller_.parameters.size());
       ++index) {
    if (controller_.parameters[index].ResetValue()) {
      event_queue_.Add(
          timestamp, SetParameterEvent{
                         index, controller_.parameters[index].GetValue(), 0.0});
    }
  }
}

bool Instrument::ResetParameter(int index, double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    if (controller_.parameters[index].ResetValue()) {
      event_queue_.Add(
          timestamp, SetParameterEvent{
                         index, controller_.parameters[index].GetValue(), 0.0});
    }
    return true;
  }
  return false;
}

void Instrument::SetData(barely::DataDefinition definition,
                         double timestamp) noexcept {
  void* new_data = nullptr;
  if (definition.move_callback) {
    definition.move_callback(definition.data, &new_data);
  }
  definition.data = new_data;
  event_queue_.Add(timestamp, SetDataEvent{definition});
}

void Instrument::SetNoteOffCallback(
    barely::Instrument::NoteOffCallback note_off_callback) noexcept {
  controller_.note_off_callback = std::move(note_off_callback);
}

void Instrument::SetNoteOnCallback(
    barely::Instrument::NoteOnCallback note_on_callback) noexcept {
  controller_.note_on_callback = std::move(note_on_callback);
}

bool Instrument::SetParameter(int index, double value, double slope,
                              double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    if (controller_.parameters[index].SetValue(value)) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, controller_.parameters[index].GetValue(),
                            slope});
    }
    return true;
  }
  return false;
}

void Instrument::StartNote(double pitch, double intensity,
                           double timestamp) noexcept {
  if (controller_.pitches.insert(pitch).second) {
    if (controller_.note_on_callback) {
      controller_.note_on_callback(pitch, intensity, timestamp);
    }
    event_queue_.Add(timestamp, StartNoteEvent{pitch, intensity});
  }
}

void Instrument::StopAllNotes(double timestamp) noexcept {
  for (const double pitch : std::exchange(controller_.pitches, {})) {
    if (controller_.note_off_callback) {
      controller_.note_off_callback(pitch, timestamp);
    }
    event_queue_.Add(timestamp, StopNoteEvent{pitch});
  }
}

void Instrument::StopNote(double pitch, double timestamp) noexcept {
  if (controller_.pitches.erase(pitch) > 0) {
    if (controller_.note_off_callback) {
      controller_.note_off_callback(pitch, timestamp);
    }
    event_queue_.Add(timestamp, StopNoteEvent{pitch});
  }
}

}  // namespace barelyapi
