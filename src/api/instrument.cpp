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
#include "common/time.h"
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
using ::barely::SampleDataMessage;

// Returns a control array with overrides.
ControlArray BuildControlArray(std::span<const BarelyControlOverride> control_overrides) noexcept {
  ControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                   // kGain
      Control(0.0f),                               // kPitchShift
      Control(false),                              // kRetrigger
      Control(0.0f, -1.0f, 1.0f),                  // kStereoPan
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
      update_frame_(barely::SecondsToFrames(engine_.GetSampleRate(), engine_.GetTimestamp())),
      processor_(control_overrides, engine_.audio_rng(), engine_.GetSampleRate(),
                 engine_.GetReferenceFrequency()) {
  engine_.AddInstrument(this);
}

BarelyInstrument::~BarelyInstrument() noexcept {
  SetAllNotesOff();
  engine_.RemoveInstrument(this);
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
void BarelyInstrument::Process(float* output_samples, int output_frame_count,
                               int64_t process_frame) noexcept {
  assert(output_samples != nullptr);
  assert(output_frame_count > 0);

  int current_frame = 0;
  // Process *all* messages before the end sample.
  const int64_t end_frame = process_frame + output_frame_count;
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        current_frame < message_frame) {
      processor_.Process(&output_samples[current_frame * barely::kStereoChannelCount],
                         message_frame - current_frame);
      current_frame = message_frame;
    }
    std::visit(MessageVisitor{[this](ControlMessage& control_message) noexcept {
                                processor_.SetControl(control_message.type, control_message.value);
                              },
                              [this](NoteControlMessage& note_control_message) noexcept {
                                processor_.SetNoteControl(note_control_message.pitch,
                                                          note_control_message.type,
                                                          note_control_message.value);
                              },
                              [this](NoteOffMessage& note_off_message) noexcept {
                                processor_.SetNoteOff(note_off_message.pitch);
                              },
                              [this](NoteOnMessage& note_on_message) noexcept {
                                processor_.SetNoteOn(note_on_message.pitch,
                                                     note_on_message.controls);
                              },
                              [this](SampleDataMessage& sample_data_message) noexcept {
                                processor_.SetSampleData(sample_data_message.sample_data);
                              }},
               message->second);
  }
  // Process the rest of the samples.
  if (current_frame < output_frame_count) {
    processor_.Process(&output_samples[current_frame * barely::kStereoChannelCount],
                       output_frame_count - current_frame);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetAllNotesOff() noexcept {
  for (const auto& [pitch, _] : std::exchange(note_controls_, {})) {
    note_event_callback_(BarelyNoteEventType_kOff, pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void BarelyInstrument::SetControl(BarelyControlType type, float value) noexcept {
  if (auto& control = controls_[type]; control.SetValue(value)) {
    control_event_callback_(type, control.value);
    message_queue_.Add(update_frame_,
                       ControlMessage{static_cast<ControlType>(type), control.value});
  }
}

void BarelyInstrument::SetNoteControl(float pitch, BarelyNoteControlType type,
                                      float value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[type]; note_control.SetValue(value)) {
      note_control_event_callback_(pitch, type, note_control.value);
      message_queue_.Add(
          update_frame_,
          NoteControlMessage{pitch, static_cast<NoteControlType>(type), note_control.value});
    }
  }
}

void BarelyInstrument::SetNoteOff(float pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_event_callback_(BarelyNoteEventType_kOff, pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetNoteOn(
    float pitch, std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  if (const auto [it, success] =
          note_controls_.try_emplace(pitch, BuildNoteControlArray(note_control_overrides));
      success) {
    note_event_callback_(BarelyNoteEventType_kOn, pitch);
    message_queue_.Add(update_frame_, NoteOnMessage{pitch, BuildNoteControls(it->second)});
  }
}

void BarelyInstrument::SetSampleData(std::span<const BarelySlice> slices) noexcept {
  message_queue_.Add(update_frame_, SampleDataMessage{slices});
}

void BarelyInstrument::Update(int64_t update_frame) noexcept {
  assert(update_frame >= update_frame_);
  update_frame_ = update_frame;
}
