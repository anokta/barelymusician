#include "engine/instrument.h"

#include <cassert>
#include <cstdint>
#include <span>
#include <utility>
#include <variant>

#include "barelymusician.h"
#include "common/find_or_null.h"
#include "dsp/decibels.h"
#include "dsp/instrument_processor.h"
#include "dsp/one_pole_filter.h"
#include "dsp/oscillator.h"
#include "dsp/sample_data.h"
#include "engine/message.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(int sample_rate, float reference_frequency, int64_t update_sample) noexcept
    : sample_rate_(sample_rate),
      update_sample_(update_sample),
      processor_(sample_rate, reference_frequency) {
  assert(sample_rate > 0);
}

Instrument::~Instrument() noexcept { SetAllNotesOff(); }

float Instrument::GetControl(ControlType type) const noexcept {
  return controls_[static_cast<int>(type)].value;
}

const float* Instrument::GetNoteControl(float pitch, NoteControlType type) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return &(*note_controls)[static_cast<int>(type)].value;
  }
  return nullptr;
}

int Instrument::GetSampleRate() const noexcept { return sample_rate_; }

bool Instrument::IsNoteOn(float pitch) const noexcept { return note_controls_.contains(pitch); }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Instrument::Process(std::span<float> output_samples, int64_t process_sample) noexcept {
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
  for (const auto& [pitch, _] : std::exchange(note_controls_, {})) {
    note_off_callback_(pitch);
    message_queue_.Add(update_sample_, NoteOffMessage{pitch});
  }
}

void Instrument::SetControl(ControlType type, float value) noexcept {
  if (auto& control = controls_[static_cast<int>(type)]; control.SetValue(value)) {
    message_queue_.Add(update_sample_, BuildControlMessage(type, control.value));
  }
}

void Instrument::SetNoteControl(float pitch, NoteControlType type, float value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[static_cast<int>(type)];
        note_control.SetValue(value)) {
      message_queue_.Add(update_sample_, NoteControlMessage{pitch, type, note_control.value});
    }
  }
}

void Instrument::SetNoteOff(float pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_callback_(pitch);
    message_queue_.Add(update_sample_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffCallback(NoteOffCallback note_off_callback) noexcept {
  note_off_callback_ = note_off_callback;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(float pitch, float intensity) noexcept {
  if (const auto [it, success] = note_controls_.try_emplace(pitch,
                                                            NoteControlArray{
                                                                Control(0.0f),  // kPitchShift
                                                            });
      success) {
    note_on_callback_(pitch, intensity);
    message_queue_.Add(update_sample_, NoteOnMessage{pitch, intensity});
  }
}

void Instrument::SetNoteOnCallback(NoteOnCallback note_on_callback) noexcept {
  note_on_callback_ = note_on_callback;
}

void Instrument::SetReferenceFrequency(float reference_frequency) noexcept {
  message_queue_.Add(update_sample_, ReferenceFrequencyMessage{reference_frequency});
}

void Instrument::SetSampleData(SampleData sample_data) noexcept {
  message_queue_.Add(update_sample_, SampleDataMessage{std::move(sample_data)});
}

void Instrument::Update(int64_t update_sample) noexcept {
  assert(update_sample >= update_sample_);
  update_sample_ = update_sample;
}

ControlMessage Instrument::BuildControlMessage(ControlType type, float value) const noexcept {
  switch (type) {
    case ControlType::kGain:
      return ControlMessage{type, AmplitudeFromDecibels(value)};
    case ControlType::kPulseWidth:
      return ControlMessage{type, NormalizePulseWidth(value)};
    case ControlType::kFilterFrequency:
      return ControlMessage{type, GetFilterCoefficient(sample_rate_, value)};
    default:
      break;
  }
  return ControlMessage{type, value};
}

}  // namespace barely::internal
