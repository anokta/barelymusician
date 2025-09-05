#include "api/instrument.h"

#include <barelymusician.h>

#include <array>
#include <cassert>
#include <cmath>
#include <span>
#include <utility>

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
using ::barely::NoteControlArray;
using ::barely::NoteControlMessage;
using ::barely::NoteControlType;
using ::barely::NoteOffMessage;
using ::barely::NoteOnMessage;
using ::barely::SampleDataMessage;

// Returns a control array with overrides.
ControlArray BuildControlArray(std::span<const BarelyControlOverride> control_overrides) noexcept {
  ControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                        // kGain
      Control(0.0f),                                    // kPitchShift
      Control(false),                                   // kRetrigger
      Control(0.0f, -1.0f, 1.0f),                       // kStereoPan
      Control(8, 1, 20),                                // kVoiceCount
      Control(0.0f, 0.0f, 60.0f),                       // kAttack
      Control(0.0f, 0.0f, 60.0f),                       // kDecay
      Control(1.0f, 0.0f, 1.0f),                        // kSustain
      Control(0.0f, 0.0f, 60.0f),                       // kRelease
      Control(0.0f, 0.0f, 1.0f),                        // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),          // kOscMode
      Control(0.0f, 0.0f, 1.0f),                        // kOscNoiseMix
      Control(0.0f),                                    // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                        // kOscShape
      Control(0.0f, -0.5f, 0.5f),                       // kOscSkew
      Control(0, 0, BarelySliceMode_kCount - 1),        // kSliceMode
      Control(0, 0, BarelyFilterType_kCount - 1),       // kFilterType
      Control(0.0f, 0.0f),                              // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),                   // kFilterQ
      Control(16.0f, 1.0f, 16.0f),                      // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                        // kBitCrusherRate
      Control(0.0f, 0.0f, 1.0f),                        // kDelaySend
      Control(0, 0, BarelyArpeggiatorMode_kCount - 1),  // kArpeggiatorMode
      Control(0.5f, 0.0f, 1.0f),                        // kArpeggiatorGateRatio
      Control(1.0f, 0.0f, 8.0f),                        // kArpeggiatorRate

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
      arpeggiator_(engine, *this),
      controls_(BuildControlArray(control_overrides)),
      processor_(control_overrides, engine_.audio_rng(), engine_.GetSampleRate(),
                 engine_.GetReferenceFrequency()) {
  engine_.AddInstrument(this);
  arpeggiator_.SetMode(
      static_cast<BarelyArpeggiatorMode>(controls_[BarelyControlType_kArpeggiatorMode].value));
  arpeggiator_.SetGateRatio(controls_[BarelyControlType_kArpeggiatorGateRatio].value);
  arpeggiator_.SetRate(controls_[BarelyControlType_kArpeggiatorGateRatio].value,
                       controls_[BarelyControlType_kArpeggiatorRate].value);
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
void BarelyInstrument::SetAllNotesOff() noexcept {
  if (static_cast<BarelyArpeggiatorMode>(controls_[BarelyControlType_kArpeggiatorMode].value) !=
      BarelyArpeggiatorMode_kNone) {
    arpeggiator_.SetAllNotesOff();
  } else {
    StopAllNotes();
  }
}

void BarelyInstrument::SetControl(BarelyControlType type, float value) noexcept {
  if (auto& control = controls_[type]; control.SetValue(value)) {
    ProcessControl(static_cast<ControlType>(type), control.value);
  }
}

void BarelyInstrument::SetNoteControl(float pitch, BarelyNoteControlType type,
                                      float value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[type]; note_control.SetValue(value)) {
      engine_.ScheduleMessage(
          NoteControlMessage{this, pitch, static_cast<NoteControlType>(type), note_control.value});
    }
  }
}

void BarelyInstrument::SetNoteOff(float pitch) noexcept {
  if (static_cast<BarelyArpeggiatorMode>(controls_[BarelyControlType_kArpeggiatorMode].value) !=
      BarelyArpeggiatorMode_kNone) {
    arpeggiator_.SetNoteOff(pitch);
  } else {
    StopNote(pitch);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetNoteOn(
    float pitch, std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  if (static_cast<BarelyArpeggiatorMode>(controls_[BarelyControlType_kArpeggiatorMode].value) !=
      BarelyArpeggiatorMode_kNone) {
    arpeggiator_.SetNoteOn(pitch);
  } else {
    StartNote(pitch, note_control_overrides);
  }
}

void BarelyInstrument::SetSampleData(std::span<const BarelySlice> slices) noexcept {
  engine_.ScheduleMessage(SampleDataMessage{this, slices});
}

void BarelyInstrument::StartNote(
    float pitch, std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  if (const auto [it, success] =
          note_controls_.try_emplace(pitch, BuildNoteControlArray(note_control_overrides));
      success) {
    note_event_callback_(BarelyNoteEventType_kBegin, pitch);
    engine_.ScheduleMessage(NoteOnMessage{this, pitch, BuildNoteControls(it->second)});
  }
}

void BarelyInstrument::StopAllNotes() noexcept {
  for (const auto& [pitch, _] : std::exchange(note_controls_, {})) {
    note_event_callback_(BarelyNoteEventType_kEnd, pitch);
    engine_.ScheduleMessage(NoteOffMessage{this, pitch});
  }
}

void BarelyInstrument::StopNote(float pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_event_callback_(BarelyNoteEventType_kEnd, pitch);
    engine_.ScheduleMessage(NoteOffMessage{this, pitch});
  }
}

void BarelyInstrument::ProcessControl(ControlType type, float value) noexcept {
  switch (type) {
    case ControlType::kArpeggiatorMode:
      arpeggiator_.SetMode(static_cast<BarelyArpeggiatorMode>(value));
      break;
    case ControlType::kArpeggiatorGateRatio:
      arpeggiator_.SetGateRatio(value);
      break;
    case ControlType::kArpeggiatorRate:
      arpeggiator_.SetRate(controls_[BarelyControlType_kArpeggiatorGateRatio].value, value);
      break;
    default:
      engine_.ScheduleMessage(ControlMessage{this, type, value});
      break;
  }
}
