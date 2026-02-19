#include "engine/instrument_controller.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>

#include "core/constants.h"
#include "core/control.h"
#include "core/rng.h"
#include "core/time.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "engine/note_state.h"

namespace barely {

namespace {

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
  if (instrument_index != kInvalidIndex) {
    InstrumentState& instrument = engine_.GetInstrument(instrument_index);
    instrument = {};
    instrument.controls = BuildInstrumentControlArray(control_overrides, control_override_count);

    engine_.ScheduleMessage(InstrumentCreateMessage{instrument_index});
    for (int i = 0; i < BarelyInstrumentControlType_kCount; ++i) {
      engine_.ScheduleMessage(InstrumentControlMessage{instrument_index,
                                                       static_cast<BarelyInstrumentControlType>(i),
                                                       instrument.controls[i].value});
    }
  }
  return instrument_index;
}

void InstrumentController::Release(uint32_t instrument_index) noexcept {
  SetAllNotesOff(instrument_index);
  ReleaseSampleData(engine_.GetInstrument(instrument_index));
  engine_.instrument_pool.Release(instrument_index);
}

void InstrumentController::SetAllNotesOff() noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    SetAllNotesOff(engine_.instrument_pool.GetActive(i));
  }
}

void InstrumentController::SetAllNotesOff(uint32_t instrument_index) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);

  uint32_t note_index = instrument.first_note_index;
  for (uint32_t i = 0; i < instrument.note_count; ++i) {
    const auto& note = engine_.note_pool.Get(note_index);
    if (!instrument.IsArpEnabled() || note_index == instrument.arp.note_index) {
      instrument.note_event_callback(BarelyNoteEventType_kEnd, note.pitch);
      engine_.ScheduleMessage(NoteOffMessage{note_index});
    }
    const uint32_t next_note_index = note.next_note_index;
    engine_.note_pool.Release(note_index);
    note_index = next_note_index;
  }

  instrument.arp = {};
  instrument.first_note_index = kInvalidIndex;
  instrument.note_count = 0;
}

void InstrumentController::SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                                      float value) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);
  if (auto& control = instrument.controls[type]; control.SetValue(value)) {
    switch (type) {
      case BarelyInstrumentControlType_kArpMode:
        if (static_cast<BarelyArpMode>(value) == BarelyArpMode_kNone) {
          if (instrument.arp.note_index != kInvalidIndex) {
            if (instrument.arp.is_note_on) {
              const float pitch = engine_.note_pool.Get(instrument.arp.note_index).pitch;
              instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
              engine_.ScheduleMessage(NoteOffMessage{instrument.arp.note_index});
              if (instrument.arp.should_release_note) {
                ReleaseNote(instrument, instrument.arp.note_index);
              }
            }
            instrument.arp = {};
            uint32_t note_index = instrument.first_note_index;
            for (uint32_t i = 0; i < instrument.note_count; ++i) {
              const auto& note = engine_.note_pool.Get(note_index);
              instrument.note_event_callback(BarelyNoteEventType_kBegin, note.pitch);
              engine_.ScheduleMessage(NoteOnMessage{note_index, instrument_index, note.pitch,
                                                    BuildNoteControls(note.controls)});
              note_index = note.next_note_index;
            }
          }
        } else if (!instrument.arp.is_note_on) {
          uint32_t note_index = instrument.first_note_index;
          for (uint32_t i = 0; i < instrument.note_count; ++i) {
            const auto& note = engine_.note_pool.Get(note_index);
            instrument.note_event_callback(BarelyNoteEventType_kEnd, note.pitch);
            engine_.ScheduleMessage(NoteOffMessage{note_index});
            note_index = note.next_note_index;
          }
          instrument.arp.note_index = instrument.first_note_index;
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
  if (const uint32_t note_index = GetNote(engine_.GetInstrument(instrument_index), pitch);
      note_index != kInvalidIndex) {
    if (auto& note_control = engine_.note_pool.Get(note_index).controls[type];
        note_control.SetValue(value)) {
      engine_.ScheduleMessage(NoteControlMessage{note_index, type, note_control.value});
    }
  }
}

void InstrumentController::SetNoteEventCallback(uint32_t instrument_index,
                                                BarelyNoteEventCallback callback,
                                                void* user_data) noexcept {
  engine_.GetInstrument(instrument_index).note_event_callback = {callback, user_data};
}

void InstrumentController::SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);

  const uint32_t note_index = GetNote(instrument, pitch);
  if (note_index == kInvalidIndex) {
    return;
  }

  if (!instrument.IsArpEnabled()) {
    instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
    engine_.ScheduleMessage(NoteOffMessage{note_index});
    ReleaseNote(instrument, note_index);
  } else if (instrument.note_count == 1) {
    if (instrument.arp.is_note_on) {
      instrument.note_event_callback(BarelyNoteEventType_kEnd, pitch);
      engine_.ScheduleMessage(NoteOffMessage{note_index});
      instrument.arp.is_note_on = false;
    }
    ReleaseNote(instrument, note_index);
  } else if (instrument.arp.note_index != note_index) {
    ReleaseNote(instrument, note_index);
  } else {
    instrument.arp.should_release_note = true;
  }
}

void InstrumentController::SetNoteOn(uint32_t instrument_index, float pitch,
                                     const BarelyNoteControlOverride* note_control_overrides,
                                     int32_t note_control_override_count) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);

  uint32_t note_index = instrument.first_note_index;
  for (uint32_t i = 0; i < instrument.note_count; ++i) {
    auto& note = engine_.note_pool.Get(note_index);
    if (note.pitch == pitch) {  // note already exists.
      return;
    }
    if (note.next_note_index == instrument.first_note_index || pitch < note.pitch) {
      break;
    }
    note_index = note.next_note_index;
  }

  const uint32_t new_note_index = engine_.note_pool.Acquire();
  if (new_note_index == kInvalidIndex) {
    assert(!"Maximum note count exceeded!");
    return;
  }

  auto& new_note = engine_.note_pool.Get(new_note_index);
  new_note = {};
  new_note.controls = BuildNoteControlArray(note_control_overrides, note_control_override_count);
  new_note.pitch = pitch;

  ++instrument.note_count;

  if (note_index == kInvalidIndex) {
    instrument.first_note_index = new_note_index;
    // Circle the note back to itself for easier iteration.
    new_note.next_note_index = new_note_index;
    new_note.prev_note_index = new_note_index;
  } else {
    auto& note = engine_.note_pool.Get(note_index);
    if (pitch < note.pitch) {
      if (note_index == instrument.first_note_index) {
        instrument.first_note_index = new_note_index;
      }
      new_note.next_note_index = note_index;
      new_note.prev_note_index = note.prev_note_index;
      engine_.note_pool.Get(new_note.prev_note_index).next_note_index = new_note_index;
      note.prev_note_index = new_note_index;
    } else {
      new_note.prev_note_index = note_index;
      new_note.next_note_index = note.next_note_index;
      engine_.note_pool.Get(new_note.next_note_index).prev_note_index = new_note_index;
      note.next_note_index = new_note_index;
    }
  }

  if (!instrument.IsArpEnabled()) {
    instrument.note_event_callback(BarelyNoteEventType_kBegin, pitch);
    engine_.ScheduleMessage(NoteOnMessage{new_note_index, instrument_index, pitch,
                                          BuildNoteControls(new_note.controls)});
  } else if (instrument.note_count == 1) {
    instrument.arp.note_index = instrument.first_note_index;
  }
}

void InstrumentController::SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                                         int32_t slice_count) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);
  ReleaseSampleData(instrument);
  instrument.first_slice_index =
      engine_.slice_pool.Acquire(slices, static_cast<uint32_t>(slice_count));
  engine_.ScheduleMessage(SampleDataMessage{instrument_index, instrument.first_slice_index});
}

float InstrumentController::GetControl(uint32_t instrument_index,
                                       BarelyInstrumentControlType type) const noexcept {
  return engine_.GetInstrument(instrument_index).controls[type].value;
}

const float* InstrumentController::GetNoteControl(uint32_t instrument_index, float pitch,
                                                  BarelyNoteControlType type) const noexcept {
  const auto& instrument = engine_.GetInstrument(instrument_index);
  if (const uint32_t note_index = GetNote(instrument, pitch); note_index != kInvalidIndex) {
    return &engine_.note_pool.Get(note_index).controls[type].value;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(uint32_t instrument_index, float pitch) const noexcept {
  const auto& instrument = engine_.GetInstrument(instrument_index);
  return instrument.IsArpEnabled()
             ? (instrument.arp.is_note_on &&
                engine_.note_pool.Get(instrument.arp.note_index).pitch == pitch)
             : (GetNote(instrument, pitch) != kInvalidIndex);
}

void InstrumentController::ProcessArp() noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    const uint32_t instrument_index = engine_.instrument_pool.GetActive(i);
    auto& instrument = engine_.GetInstrument(instrument_index);
    if (instrument.first_note_index == kInvalidIndex || !instrument.IsArpEnabled()) {
      continue;
    }
    if (!instrument.arp.is_note_on && instrument.arp.phase == 0.0) {
      const auto& note = engine_.note_pool.Get(instrument.arp.note_index);
      instrument.arp.is_note_on = true;
      instrument.note_event_callback(BarelyNoteEventType_kBegin, note.pitch);
      engine_.ScheduleMessage(NoteOnMessage{instrument.arp.note_index, instrument_index, note.pitch,
                                            BuildNoteControls(note.controls)});
    } else if (instrument.arp.is_note_on &&
               instrument.arp.phase ==
                   static_cast<double>(
                       instrument.controls[BarelyInstrumentControlType_kArpGateRatio].value)) {
      const auto& note = engine_.note_pool.Get(instrument.arp.note_index);
      instrument.note_event_callback(BarelyNoteEventType_kEnd, note.pitch);
      engine_.ScheduleMessage(NoteOffMessage{instrument.arp.note_index});
      if (instrument.arp.should_release_note) {
        ReleaseNote(instrument, instrument.arp.note_index);
        instrument.arp.should_release_note = false;
      }
      instrument.arp.is_note_on = false;
      switch (static_cast<BarelyArpMode>(
          instrument.controls[BarelyInstrumentControlType_kArpMode].value)) {
        case BarelyArpMode_kUp:
          instrument.arp.note_index = note.next_note_index;
          break;
        case BarelyArpMode_kDown:
          instrument.arp.note_index = note.prev_note_index;
          break;
        case BarelyArpMode_kRandom: {
          uint32_t note_index = instrument.arp.note_index;
          const uint32_t random_count = engine_.main_rng.Generate(0, instrument.note_count);
          for (uint32_t _ = 0; _ < random_count; ++_) {
            note_index = engine_.note_pool.Get(note_index).next_note_index;
          }
          instrument.arp.note_index = note_index;
        } break;
        default:
          assert(!"Invalid arpeggiator mode");
          return;
      }
    }
  }
}

void InstrumentController::Update(double duration) noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    engine_.GetInstrument(engine_.instrument_pool.GetActive(i)).Update(duration);
  }
}

double InstrumentController::GetNextDuration() const noexcept {
  double next_duration = std::numeric_limits<double>::max();
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    next_duration =
        std::min(engine_.GetInstrument(engine_.instrument_pool.GetActive(i)).GetNextArpDuration(),
                 next_duration);
  }
  return next_duration;
}

uint32_t InstrumentController::GetNote(const InstrumentState& instrument,
                                       float pitch) const noexcept {
  uint32_t note_index = instrument.first_note_index;
  for (uint32_t i = 0; i < instrument.note_count; ++i) {
    const auto& note = engine_.note_pool.Get(note_index);
    if (note.pitch == pitch) {
      return (!instrument.arp.should_release_note || instrument.arp.note_index != note_index)
                 ? note_index
                 : kInvalidIndex;
    }
    note_index = note.next_note_index;
  }
  return kInvalidIndex;
}

void InstrumentController::ReleaseNote(InstrumentState& instrument, uint32_t note_index) noexcept {
  auto& note = engine_.note_pool.Get(note_index);

  if (instrument.note_count == 1) {
    instrument.first_note_index = kInvalidIndex;
    instrument.arp.phase = 0.0;
  } else {
    if (note_index == instrument.first_note_index) {
      instrument.first_note_index = note.next_note_index;
    }
    engine_.note_pool.Get(note.prev_note_index).next_note_index = note.next_note_index;
    engine_.note_pool.Get(note.next_note_index).prev_note_index = note.prev_note_index;
  }

  engine_.note_pool.Release(note_index);
  --instrument.note_count;
}

void InstrumentController::ReleaseSampleData(InstrumentState& instrument) noexcept {
  if (instrument.first_slice_index != kInvalidIndex) {
    engine_.slice_pool.ReleaseAt(instrument.first_slice_index,
                                 SecondsToFrames(engine_.sample_rate, engine_.timestamp));
  }
}

}  // namespace barely
