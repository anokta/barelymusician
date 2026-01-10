#include "engine/instrument_controller.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

#include "core/rng.h"
#include "dsp/control.h"
#include "engine/instrument_state.h"
#include "engine/message.h"

namespace barely {

namespace {

// Returns a control array with overrides.
[[nodiscard]] InstrumentControlArray BuildControlArray(
    const BarelyInstrumentControlOverride* control_overrides,
    int32_t control_override_count) noexcept {
  InstrumentControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                   // kGain
      Control(0.0f),                               // kPitchShift
      Control(false),                              // kRetrigger
      Control(0.0f, -1.0f, 1.0f),                  // kStereoPan
      Control(8, 1, 16),                           // kVoiceCount
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
      Control(16.0f, 1.0f, 16.0f),                 // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                   // kBitCrusherRate
      Control(0.0f, 0.0f, 1.0f),                   // kDistortionAmount
      Control(1.0f, 0.0f, 20.0f),                  // kDistortionDrive
      Control(0, 0, BarelyFilterType_kCount - 1),  // kFilterType
      Control(0.0f, 0.0f),                         // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),              // kFilterQ
      Control(0.0f, 0.0f, 1.0f),                   // kDelaySend
      Control(0.0f, -1.0f, 1.0f),                  // kSidechainSend
      Control(0, 0, BarelyArpMode_kCount - 1),     // kArpMode
      Control(0.5f, 0.001f, 1.0f),                 // kArpGateRatio
      Control(1.0f, 0.0f, 16.0f),                  // kArpRate
  };
  for (int i = 0; i < control_override_count; ++i) {
    control_array[control_overrides[i].type].SetValue(control_overrides[i].value);
  }
  return control_array;
}

// Returns a note control array with overrides.
[[nodiscard]] NoteControlArray BuildNoteControlArray(
    const BarelyNoteControlOverride* note_control_overrides,
    int32_t note_control_override_count) noexcept {
  NoteControlArray note_control_array = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };
  for (int i = 0; i < note_control_override_count; ++i) {
    note_control_array[note_control_overrides[i].type].SetValue(note_control_overrides[i].value);
  }
  return note_control_array;
}

// Returns an array of note control values from a given note control array.
[[nodiscard]] std::array<float, BarelyNoteControlType_kCount> BuildNoteControls(
    const NoteControlArray& note_control_array) noexcept {
  std::array<float, BarelyNoteControlType_kCount> note_controls;
  for (int i = 0; i < BarelyNoteControlType_kCount; ++i) {
    note_controls[i] = note_control_array[i].value;
  }
  return note_controls;
}

}  // namespace

uint32_t InstrumentController::Acquire(const BarelyInstrumentControlOverride* control_overrides,
                                       int32_t control_override_count) noexcept {
  const uint32_t instrument_index = engine_.instrument_pool.Acquire();
  if (instrument_index == 0) {
    return 0;
  }

  InstrumentState& instrument = engine_.instrument_pool.Get(instrument_index);
  instrument = {};
  instrument.controls = BuildControlArray(control_overrides, control_override_count);

  engine_.ScheduleMessage(InstrumentCreateMessage{instrument_index});
  for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
    engine_.ScheduleMessage(InstrumentControlMessage{instrument_index,
                                                     static_cast<BarelyInstrumentControlType>(i),
                                                     instrument.controls[i].value});
  }

  return instrument_index;
}

void InstrumentController::Release(uint32_t instrument_index) noexcept {
  SetAllNotesOff(instrument_index);
  engine_.instrument_pool.Release(instrument_index);
}

void InstrumentController::SetAllNotesOff() noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.GetActiveCount(); ++i) {
    SetAllNotesOff(engine_.instrument_pool.GetActiveIndex(i));
  }
}

void InstrumentController::SetAllNotesOff(uint32_t instrument_index) noexcept {
  auto& instrument = engine_.instrument_pool.Get(instrument_index);
  instrument.note_controls.clear();
  if (instrument.is_arp_pitch_on) {
    instrument.pitches.clear();
    instrument.is_arp_pitch_on = false;
    instrument.arp_phase = 0.0;
    instrument.arp_pitch_index = -1;
    instrument.note_event_callback(BarelyNoteEventType_kEnd, instrument.arp_pitch);
    engine_.ScheduleMessage(NoteOffMessage{instrument_index, instrument.arp_pitch});
  } else {
    for (const float pitch : std::exchange(instrument.pitches, {})) {
      instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
      engine_.ScheduleMessage(NoteOffMessage{instrument_index, pitch});
    }
  }
}

void InstrumentController::SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                                      float value) noexcept {
  auto& instrument = engine_.instrument_pool.Get(instrument_index);
  if (auto& control = instrument.controls[type]; control.SetValue(value)) {
    switch (type) {
      case BarelyInstrumentControlType_kArpMode:
        if (static_cast<BarelyArpMode>(value) == BarelyArpMode_kNone) {
          if (instrument.arp_pitch_index != -1) {
            instrument.is_arp_pitch_on = false;
            instrument.arp_phase = 0.0;
            instrument.arp_pitch_index = -1;
            instrument.note_event_callback(BarelyNoteEventType_kEnd, instrument.arp_pitch);
            engine_.ScheduleMessage(NoteOffMessage{instrument_index, instrument.arp_pitch});
            for (const auto& [pitch, note_controls] : instrument.note_controls) {
              instrument.note_event_callback(BarelyNoteEventType_kBegin, pitch);
              engine_.ScheduleMessage(
                  NoteOnMessage{instrument_index, pitch, BuildNoteControls(note_controls)});
            }
          }
        } else if (!instrument.pitches.empty() && !instrument.is_arp_pitch_on) {
          for (const float pitch : instrument.pitches) {
            instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
            engine_.ScheduleMessage(NoteOffMessage{instrument_index, pitch});
          }
        }
        break;
      case BarelyInstrumentControlType_kArpGateRatio:
        [[fallthrough]];
      case BarelyInstrumentControlType_kArpRate:
        break;
      default:
        engine_.ScheduleMessage(InstrumentControlMessage{instrument_index, type, value});
        break;
    }
  }
}

void InstrumentController::SetNoteControl(uint32_t instrument_index, float pitch,
                                          BarelyNoteControlType type, float value) noexcept {
  auto& instrument = engine_.instrument_pool.Get(instrument_index);
  if (const auto it = instrument.note_controls.find(pitch); it != instrument.note_controls.end()) {
    if (auto& note_control = it->second[type]; note_control.SetValue(value)) {
      engine_.ScheduleMessage(
          NoteControlMessage{instrument_index, pitch, type, note_control.value});
    }
  }
}

void InstrumentController::SetNoteEventCallback(uint32_t instrument_index,
                                                BarelyNoteEventCallback callback,
                                                void* user_data) noexcept {
  engine_.instrument_pool.Get(instrument_index).note_event_callback = {callback, user_data};
}

void InstrumentController::SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
  auto& instrument = engine_.instrument_pool.Get(instrument_index);
  if (instrument.note_controls.erase(pitch) > 0) {
    instrument.pitches.erase(
        std::find(instrument.pitches.begin(), instrument.pitches.end(), pitch));
    if (instrument.pitches.empty() && instrument.IsArpEnabled()) {
      instrument.is_arp_pitch_on = false;
      instrument.arp_phase = 0.0;
      instrument.arp_pitch_index = -1;
      instrument.note_event_callback(BarelyNoteEventType_kEnd, instrument.arp_pitch);
      engine_.ScheduleMessage(NoteOffMessage{instrument_index, instrument.arp_pitch});
    } else if (!instrument.IsArpEnabled()) {
      instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
      engine_.ScheduleMessage(NoteOffMessage{instrument_index, pitch});
    }
  }
}

void InstrumentController::SetNoteOn(uint32_t instrument_index, float pitch,
                                     const BarelyNoteControlOverride* note_control_overrides,
                                     int32_t note_control_override_count) noexcept {
  auto& instrument = engine_.instrument_pool.Get(instrument_index);
  if (const auto [it, success] = instrument.note_controls.try_emplace(
          pitch, BuildNoteControlArray(note_control_overrides, note_control_override_count));
      success) {
    instrument.pitches.insert(
        std::lower_bound(instrument.pitches.begin(), instrument.pitches.end(), pitch), pitch);
    if (instrument.pitches.size() == 1 && instrument.IsArpEnabled()) {
    } else if (!instrument.IsArpEnabled()) {
      instrument.note_event_callback(BarelyNoteEventType_kBegin, pitch);
      engine_.ScheduleMessage(
          NoteOnMessage{instrument_index, pitch, BuildNoteControls(it->second)});
    }
  }
}

float InstrumentController::GetControl(uint32_t instrument_index,
                                       BarelyInstrumentControlType type) const noexcept {
  return engine_.instrument_pool.Get(instrument_index).controls[type].value;
}

const float* InstrumentController::GetNoteControl(uint32_t instrument_index, float pitch,
                                                  BarelyNoteControlType type) const noexcept {
  const auto& instrument = engine_.instrument_pool.Get(instrument_index);
  if (const auto it = instrument.note_controls.find(pitch); it != instrument.note_controls.end()) {
    return &it->second[type].value;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(uint32_t instrument_index, float pitch) const noexcept {
  return engine_.instrument_pool.Get(instrument_index).IsNoteOn(pitch);
}

void InstrumentController::ProcessArp(MainRng& main_rng) noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.GetActiveCount(); ++i) {
    auto& instrument = engine_.instrument_pool.GetActive(i);
    if (!instrument.IsArpEnabled() || instrument.pitches.empty()) {
      return;
    }
    if (!instrument.is_arp_pitch_on && instrument.arp_phase == 0.0) {
      const int size = static_cast<int>(instrument.pitches.size());
      switch (static_cast<BarelyArpMode>(
          instrument.controls[BarelyInstrumentControlType_kArpMode].value)) {
        case BarelyArpMode_kUp:
          instrument.arp_pitch_index = (instrument.arp_pitch_index + 1) % size;
          break;
        case BarelyArpMode_kDown:
          instrument.arp_pitch_index = (instrument.arp_pitch_index == -1)
                                           ? size - 1
                                           : (instrument.arp_pitch_index + size - 1) % size;
          break;
        case BarelyArpMode_kRandom:
          instrument.arp_pitch_index = main_rng.Generate(0, size);
          break;
        default:
          assert(!"Invalid arpeggiator mode");
          return;
      }
      assert(instrument.arp_pitch_index >= 0 && instrument.arp_pitch_index < size);
      instrument.arp_pitch = instrument.pitches[instrument.arp_pitch_index];

      instrument.is_arp_pitch_on = true;
      instrument.note_event_callback(BarelyNoteEventType_kBegin, instrument.arp_pitch);
      engine_.ScheduleMessage(
          NoteOnMessage{engine_.instrument_pool.GetActiveIndex(i), instrument.arp_pitch,
                        BuildNoteControls(instrument.note_controls.at(instrument.arp_pitch))});
    } else if (instrument.is_arp_pitch_on &&
               instrument.arp_phase ==
                   static_cast<double>(
                       instrument.controls[BarelyInstrumentControlType_kArpGateRatio].value)) {
      instrument.is_arp_pitch_on = false;
      instrument.note_event_callback(BarelyNoteEventType_kEnd, instrument.arp_pitch);
      engine_.ScheduleMessage(
          NoteOffMessage{engine_.instrument_pool.GetActiveIndex(i), instrument.arp_pitch});
    }
  }
}

void InstrumentController::Update(double duration) noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.GetActiveCount(); ++i) {
    engine_.instrument_pool.GetActive(i).Update(duration);
  }
}

double InstrumentController::GetNextDuration() const noexcept {
  double next_duration = std::numeric_limits<double>::max();
  for (uint32_t i = 0; i < engine_.instrument_pool.GetActiveCount(); ++i) {
    next_duration =
        std::min(engine_.instrument_pool.GetActive(i).GetNextArpDuration(), next_duration);
  }
  return next_duration;
}

}  // namespace barely
