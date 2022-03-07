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

// Returns corresponding samples for given `seconds`.
int SamplesFromSeconds(int sample_rate, double seconds) noexcept {
  return sample_rate > 0
             ? static_cast<int>(seconds * static_cast<double>(sample_rate))
             : 0;
}

// Returns corresponding seconds for given `samples`.
double SecondsFromSamples(int sample_rate, int samples) noexcept {
  return sample_rate > 0
             ? static_cast<double>(samples) / static_cast<double>(sample_rate)
             : 0.0;
}

}  // namespace

Instrument::Instrument(const BarelyInstrumentDefinition& definition,
                       int sample_rate) noexcept
    : processor_{
          definition.destroy_callback,     definition.process_callback,
          definition.set_data_callback,    definition.set_note_off_callback,
          definition.set_note_on_callback, GainProcessor(sample_rate)} {
  assert(sample_rate >= 0);
  controller_.parameters.reserve(definition.num_parameter_definitions);
  processor_.parameters.reserve(definition.num_parameter_definitions);
  for (int index = 0; index < definition.num_parameter_definitions; ++index) {
    controller_.parameters.emplace_back(
        definition.parameter_definitions[index]);
    processor_.parameters.push_back(
        {controller_.parameters[index].GetValue(), 0.0});
  }
  processor_.state = {.sample_rate = sample_rate,
                      .parameters = processor_.parameters.data(),
                      .num_parameters = definition.num_parameter_definitions,
                      .user_state = nullptr};
  if (definition.create_callback) {
    definition.create_callback(&processor_.state);
  }
}

Instrument::~Instrument() noexcept {
  if (processor_.destroy_callback) {
    processor_.destroy_callback(&processor_.state);
  }
}

double Instrument::GetGain() const noexcept { return controller_.gain; }

const Parameter* Instrument::GetParameter(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    return &controller_.parameters[index];
  }
  return nullptr;
}

bool Instrument::IsMuted() const noexcept { return controller_.is_muted; }

bool Instrument::IsNoteOn(float pitch) const noexcept {
  return controller_.pitches.contains(pitch);
}

void Instrument::Process(float* output, int num_output_channels,
                         int num_output_frames, double timestamp) noexcept {
  assert(output);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  int frame = 0;
  int gain_frame = 0;
  // Process *all* events before the end timestamp.
  const double end_timestamp =
      timestamp +
      SecondsFromSamples(processor_.state.sample_rate, num_output_frames);
  for (const auto* event = event_queue_.GetNext(end_timestamp); event;
       event = event_queue_.GetNext(end_timestamp)) {
    const int message_frame = SamplesFromSeconds(processor_.state.sample_rate,
                                                 event->first - timestamp);
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
            [&](const SetGainEvent& set_gain_event) noexcept {
              if (gain_frame < frame) {
                processor_.gain_processor.Process(
                    &output[num_output_channels * gain_frame],
                    num_output_channels, frame - gain_frame);
                gain_frame = frame;
              }
              processor_.gain_processor.SetGain(set_gain_event.gain);
            },
            [this](const SetParameterEvent& set_parameter_event) noexcept {
              processor_.parameters[set_parameter_event.index] = {
                  set_parameter_event.value, 0.0};
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
  if (gain_frame < num_output_frames) {
    processor_.gain_processor.Process(&output[num_output_channels * gain_frame],
                                      num_output_channels, num_output_frames);
  }
}

void Instrument::ResetAllParameters(double timestamp) noexcept {
  for (int index = 0; index < static_cast<int>(controller_.parameters.size());
       ++index) {
    if (controller_.parameters[index].ResetValue()) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, controller_.parameters[index].GetValue()});
    }
  }
}

bool Instrument::ResetParameter(int index, double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    if (controller_.parameters[index].ResetValue()) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, controller_.parameters[index].GetValue()});
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

void Instrument::SetGain(double gain, double timestamp) noexcept {
  if (controller_.gain != gain) {
    controller_.gain = gain;
    if (!controller_.is_muted) {
      event_queue_.Add(timestamp, SetGainEvent{gain});
    }
  }
}

void Instrument::SetMuted(bool is_muted, double timestamp) noexcept {
  if (controller_.is_muted != is_muted) {
    controller_.is_muted = is_muted;
    event_queue_.Add(
        timestamp, SetGainEvent{controller_.is_muted ? 0.0 : controller_.gain});
  }
}

void Instrument::SetNoteOffCallback(
    BarelyInstrument_NoteOffCallback note_off_callback,
    void* user_data) noexcept {
  if (note_off_callback) {
    controller_.note_off_callback = [note_off_callback, user_data](
                                        float pitch, double timestamp) {
      if (note_off_callback) {
        note_off_callback(pitch, timestamp, user_data);
      }
    };
  } else {
    controller_.note_off_callback = nullptr;
  }
}

void Instrument::SetNoteOnCallback(
    BarelyInstrument_NoteOnCallback note_on_callback,
    void* user_data) noexcept {
  if (note_on_callback) {
    controller_.note_on_callback = [note_on_callback, user_data](
                                       float pitch, float intensity,
                                       double timestamp) {
      note_on_callback(pitch, intensity, timestamp, user_data);
    };
  } else {
    controller_.note_on_callback = nullptr;
  }
}

bool Instrument::SetParameter(int index, double value,
                              double timestamp) noexcept {
  if (index >= 0 && index < static_cast<int>(controller_.parameters.size())) {
    if (controller_.parameters[index].SetValue(value)) {
      event_queue_.Add(
          timestamp,
          SetParameterEvent{index, controller_.parameters[index].GetValue()});
    }
    return true;
  }
  return false;
}

void Instrument::StartNote(float pitch, float intensity,
                           double timestamp) noexcept {
  if (controller_.pitches.insert(pitch).second) {
    if (controller_.note_on_callback) {
      controller_.note_on_callback(pitch, intensity, timestamp);
    }
    event_queue_.Add(timestamp, StartNoteEvent{pitch, intensity});
  }
}

void Instrument::StopAllNotes(double timestamp) noexcept {
  for (const float pitch : std::exchange(controller_.pitches, {})) {
    if (controller_.note_off_callback) {
      controller_.note_off_callback(pitch, timestamp);
    }
    event_queue_.Add(timestamp, StopNoteEvent{pitch});
  }
}

void Instrument::StopNote(float pitch, double timestamp) noexcept {
  if (controller_.pitches.erase(pitch) > 0) {
    if (controller_.note_off_callback) {
      controller_.note_off_callback(pitch, timestamp);
    }
    event_queue_.Add(timestamp, StopNoteEvent{pitch});
  }
}

}  // namespace barelyapi
