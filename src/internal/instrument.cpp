#include "internal/instrument.h"

#include <cassert>
#include <cstdint>
#include <span>
#include <utility>
#include <variant>

#include "barelymusician.h"
#include "common/find_or_null.h"
#include "internal/event.h"
#include "internal/instrument_processor.h"
#include "internal/message.h"
#include "internal/sample_data.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(int sample_rate, double reference_frequency, int64_t update_sample) noexcept
    : sample_rate_(sample_rate),
      update_sample_(update_sample),
      processor_(sample_rate, reference_frequency) {
  assert(sample_rate > 0);
}

Instrument::~Instrument() noexcept { SetAllNotesOff(); }

double Instrument::GetControl(ControlType type) const noexcept {
  return controls_[static_cast<int>(type)].value;
}

const double* Instrument::GetNoteControl(double pitch, NoteControlType type) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return &(*note_controls)[static_cast<int>(type)].value;
  }
  return nullptr;
}

int Instrument::GetSampleRate() const noexcept { return sample_rate_; }

bool Instrument::IsNoteOn(double pitch) const noexcept { return note_controls_.contains(pitch); }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Instrument::Process(std::span<double> output_samples, int64_t process_sample) noexcept {
  if (output_samples.empty()) {
    return false;
  }
  const int output_sample_count = static_cast<int>(output_samples.size());

  int current_sample = 0;
  // Process *all* messages before the end sample.
  const int64_t end_sample = process_sample + output_sample_count;
  for (auto* message = message_queue_.GetNext(end_sample); message;
       message = message_queue_.GetNext(end_sample)) {
    if (const int message_sample = static_cast<int>(message->first - process_sample);
        current_sample < message_sample) {
      processor_.Process(&output_samples[current_sample], message_sample - current_sample);
      current_sample = message_sample;
    }
    std::visit(
        MessageVisitor{
            [this](ControlMessage& control_message) noexcept {
              processor_.SetControl(control_message.type, control_message.value);
            },
            [this](NoteControlMessage& note_control_message) noexcept {
              processor_.SetNoteControl(note_control_message.pitch, note_control_message.type,
                                        note_control_message.value);
            },
            [this](NoteOffMessage& note_off_message) noexcept {
              processor_.SetNoteOff(note_off_message.pitch);
            },
            [this](NoteOnMessage& note_on_message) noexcept {
              processor_.SetNoteOn(note_on_message.pitch, note_on_message.intensity);
            },
            [this](ReferenceFrequencyMessage& reference_frequency_message) noexcept {
              processor_.SetReferenceFrequency(reference_frequency_message.reference_frequency);
            },
            [this](SampleDataMessage& sample_data_message) noexcept {
              processor_.SetSampleData(sample_data_message.sample_data);
            }},
        message->second);
  }
  // Process the rest of the samples.
  if (current_sample < output_sample_count) {
    processor_.Process(&output_samples[current_sample], output_sample_count - current_sample);
  }
  return true;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetAllNotesOff() noexcept {
  for (const auto& [note, _] : std::exchange(note_controls_, {})) {
    note_off_event_.Process(note);
    message_queue_.Add(update_sample_, NoteOffMessage{note});
  }
}

void Instrument::SetControl(ControlType type, double value) noexcept {
  if (auto& control = controls_[static_cast<int>(type)]; control.SetValue(value)) {
    message_queue_.Add(update_sample_, ControlMessage{type, control.value});
  }
}

void Instrument::SetNoteControl(double pitch, NoteControlType type, double value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[static_cast<int>(type)];
        note_control.SetValue(value)) {
      message_queue_.Add(update_sample_, NoteControlMessage{pitch, type, note_control.value});
    }
  }
}

void Instrument::SetNoteOff(double pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_event_.Process(pitch);
    message_queue_.Add(update_sample_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEvent(const NoteOffEvent* note_off_event) noexcept {
  note_off_event_ = (note_off_event != nullptr) ? *note_off_event : Event<NoteOffEvent, double>{};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (const auto [it, success] = note_controls_.try_emplace(pitch,
                                                            NoteControlArray{
                                                                Control(0.0),  // kPitchShift
                                                            });
      success) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(update_sample_, NoteOnMessage{pitch, intensity});
  }
}

void Instrument::SetNoteOnEvent(const NoteOnEvent* note_on_event) noexcept {
  note_on_event_ =
      (note_on_event != nullptr) ? *note_on_event : Event<NoteOnEvent, double, double>{};
}

void Instrument::SetReferenceFrequency(double reference_frequency) noexcept {
  message_queue_.Add(update_sample_, ReferenceFrequencyMessage{reference_frequency});
}

void Instrument::SetSampleData(SampleData sample_data) noexcept {
  message_queue_.Add(update_sample_, SampleDataMessage{std::move(sample_data)});
}

void Instrument::Update(int64_t update_sample) noexcept {
  assert(update_sample >= update_sample_);
  update_sample_ = update_sample;
}

}  // namespace barely::internal
