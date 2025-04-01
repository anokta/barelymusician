#include "api/instrument.h"

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <span>
#include <utility>
#include <variant>

#include "api/engine.h"
#include "common/find_or_null.h"
#include "dsp/control.h"
#include "dsp/instrument_processor.h"
#include "dsp/message.h"

namespace {

using ::barely::Control;
using ::barely::ControlArray;
using ::barely::ControlMessage;
using ::barely::ControlType;
using ::barely::MessageVisitor;
using ::barely::NoteControlArray;
using ::barely::NoteControlMessage;
using ::barely::NoteControlType;
using ::barely::NoteOffMessage;
using ::barely::NoteOnMessage;
using ::barely::ReferenceFrequencyMessage;
using ::barely::SampleDataMessage;

// Returns a control array with overrides.
ControlArray BuildControlArray(std::span<const BarelyControlOverride> control_overrides) noexcept {
  ControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                   // kGain
      Control(0.0f),                               // kPitchShift
      Control(false),                              // kRetrigger
      Control(8, 1, 20),                           // kVoiceCount
      Control(0.0f, 0.0f, 60.0f),                  // kAttack
      Control(0.0f, 0.0f, 60.0f),                  // kDecay
      Control(1.0f, 0.0f, 1.0f),                   // kSustain
      Control(0.0f, 0.0f, 60.0f),                  // kRelease
      Control(0.0f, 0.0f, 1.0f),                   // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),     // kOscMode
      Control(0.0f, 0.0f, 1.0f),                   // kOscNoiseMix
      Control(0.0f),                               // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                   // kOscShape
      Control(0.0f, -0.5f, 0.5f),                  // kOscSkew
      Control(0, 0, BarelySliceMode_kCount - 1),   // kSliceMode
      Control(0, 0, BarelyFilterType_kCount - 1),  // kFilterType
      Control(0.0f, 0.0f),                         // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),              // kFilterQ
      Control(16.0f, 1.0f, 16.0f),                 // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                   // kBitCrusherRate
  };
  for (const auto& [type, value] : control_overrides) {
    control_array[type].SetValue(value);
  }
  return control_array;
}

// Returns a note control array with overrides.
NoteControlArray BuildNoteControlArray(
    std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  NoteControlArray note_control_array = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };
  for (const auto& [type, value] : note_control_overrides) {
    note_control_array[type].SetValue(value);
  }
  return note_control_array;
}

// Returns an array of note control values from a given note control array.
std::array<float, BarelyNoteControlType_kCount> BuildNoteControls(
    const NoteControlArray& note_control_array) noexcept {
  std::array<float, BarelyNoteControlType_kCount> note_controls;
  for (int i = 0; i < BarelyNoteControlType_kCount; ++i) {
    note_controls[i] = note_control_array[i].value;
  }
  return note_controls;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyInstrument::BarelyInstrument(
    BarelyEngine& engine, std::span<const BarelyControlOverride> control_overrides) noexcept
    : engine_(engine),
      controls_(BuildControlArray(control_overrides)),
      update_sample_(engine_.SecondsToSamples(engine_.GetTimestamp())),
      processor_(control_overrides, engine_.audio_rng(), engine_.GetSampleRate(),
                 engine_.GetReferenceFrequency()) {
  engine_.CreateInstrument(this);
}

BarelyInstrument::~BarelyInstrument() noexcept {
  SetAllNotesOff();
  engine_.DestroyInstrument(this);
}

float BarelyInstrument::GetControl(BarelyControlType type) const noexcept {
  return controls_[type].value;
}

const float* BarelyInstrument::GetNoteControl(float pitch,
                                              BarelyNoteControlType type) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return &(*note_controls)[type].value;
  }
  return nullptr;
}

bool BarelyInstrument::IsNoteOn(float pitch) const noexcept {
  return note_controls_.contains(pitch);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool BarelyInstrument::Process(std::span<float> output_samples, double timestamp) noexcept {
  if (output_samples.empty()) {
    return false;
  }
  const int output_sample_count = static_cast<int>(output_samples.size());
  const int64_t process_sample = engine_.SecondsToSamples(timestamp);

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
              processor_.SetNoteOn(note_on_message.pitch, note_on_message.controls);
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
void BarelyInstrument::SetAllNotesOff() noexcept {
  for (const auto& [pitch, _] : std::exchange(note_controls_, {})) {
    note_off_callback_(pitch);
    message_queue_.Add(update_sample_, NoteOffMessage{pitch});
  }
}

void BarelyInstrument::SetControl(BarelyControlType type, float value) noexcept {
  if (auto& control = controls_[type]; control.SetValue(value)) {
    message_queue_.Add(update_sample_,
                       ControlMessage{static_cast<ControlType>(type), control.value});
  }
}

void BarelyInstrument::SetNoteControl(float pitch, BarelyNoteControlType type,
                                      float value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[type]; note_control.SetValue(value)) {
      message_queue_.Add(
          update_sample_,
          NoteControlMessage{pitch, static_cast<NoteControlType>(type), note_control.value});
    }
  }
}

void BarelyInstrument::SetNoteOff(float pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_callback_(pitch);
    message_queue_.Add(update_sample_, NoteOffMessage{pitch});
  }
}

void BarelyInstrument::SetNoteOffCallback(NoteCallback callback) noexcept {
  note_off_callback_ = callback;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetNoteOn(
    float pitch, std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  if (const auto [it, success] =
          note_controls_.try_emplace(pitch, BuildNoteControlArray(note_control_overrides));
      success) {
    note_on_callback_(pitch);
    message_queue_.Add(update_sample_, NoteOnMessage{pitch, BuildNoteControls(it->second)});
  }
}

void BarelyInstrument::SetNoteOnCallback(NoteCallback callback) noexcept {
  note_on_callback_ = callback;
}

void BarelyInstrument::SetReferenceFrequency(float reference_frequency) noexcept {
  message_queue_.Add(update_sample_, ReferenceFrequencyMessage{reference_frequency});
}

void BarelyInstrument::SetSampleData(std::span<const BarelySlice> slices) noexcept {
  message_queue_.Add(update_sample_, SampleDataMessage{slices});
}

void BarelyInstrument::Update(int64_t update_sample) noexcept {
  assert(update_sample >= update_sample_);
  update_sample_ = update_sample;
}
