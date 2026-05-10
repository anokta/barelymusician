#include "engine/instrument_controller.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

#include "core/constants.h"
#include "core/control.h"
#include "core/rng.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "engine/note_state.h"

namespace barely {

namespace {

void InsertScheduledEvent(EngineState& engine, uint32_t& first_event_index,
                          uint32_t new_event_index) noexcept {
  auto& new_event = engine.GetEvent(new_event_index);
  uint32_t prev_event_index = kInvalidIndex;
  uint32_t event_index = first_event_index;

  while (event_index != kInvalidIndex &&
         engine.GetEvent(event_index).position <= new_event.position) {
    prev_event_index = event_index;
    event_index = engine.GetEvent(event_index).next_event_index;
  }

  new_event.prev_event_index = prev_event_index;
  new_event.next_event_index = event_index;

  if (prev_event_index == kInvalidIndex) {
    first_event_index = new_event_index;
  } else {
    engine.GetEvent(prev_event_index).next_event_index = new_event_index;
  }

  if (event_index != kInvalidIndex) {
    engine.GetEvent(event_index).prev_event_index = new_event_index;
  }
}

}  // namespace

uint32_t InstrumentController::Acquire() noexcept {
  const uint32_t instrument_index = engine_.instrument_pool.Acquire();
  if (instrument_index != kInvalidIndex) {
    InstrumentState& instrument = engine_.GetInstrument(instrument_index);
    instrument = {};
    engine_.ScheduleMessage(InstrumentCreateMessage{instrument_index});
  }
  return instrument_index;
}

void InstrumentController::Release(uint32_t instrument_index) noexcept {
  CancelAllScheduledEvents(instrument_index);
  SetAllNotesOff(instrument_index);
  SetSampleData(instrument_index, nullptr, 0);
  engine_.instrument_pool.Release(instrument_index);
}

void InstrumentController::CancelAllScheduledEvents(uint32_t instrument_index) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);
  uint32_t event_index = instrument.first_event_index;
  while (event_index != kInvalidIndex) {
    auto& event = engine_.GetEvent(event_index);
    const uint32_t next_event_index = event.next_event_index;
    engine_.event_pool.Release(event_index);
    event_index = next_event_index;
  }
  instrument.first_event_index = kInvalidIndex;

  event_index = instrument.first_note_off_event_index;
  while (event_index != kInvalidIndex) {
    auto& event = engine_.GetEvent(event_index);
    const uint32_t next_event_index = event.next_event_index;
    engine_.event_pool.Release(event_index);
    event_index = next_event_index;
  }
  instrument.first_note_off_event_index = kInvalidIndex;
}

void InstrumentController::ScheduleControl(uint32_t instrument_index,
                                           BarelyInstrumentControlType type, float value,
                                           double position, double duration) noexcept {
  const uint32_t control_event_index = engine_.event_pool.Acquire();
  if (control_event_index == kInvalidIndex) {
    assert(!"Maximum event count exceeded!");
    return;
  }

  auto& instrument = engine_.GetInstrument(instrument_index);

  auto& control_event = engine_.GetEvent(control_event_index);
  control_event = {};
  control_event.control.type = type;
  control_event.control.value = value;
  control_event.position = position;
  control_event.type = EventType::kControlEvent;
  // TODO(schedule): Support duration
  duration;

  InsertScheduledEvent(engine_, instrument.first_event_index, control_event_index);
}

void InstrumentController::ScheduleNote(uint32_t instrument_index, float pitch, double position,
                                        double duration) noexcept {
  const uint32_t note_on_event_index = engine_.event_pool.Acquire();
  if (note_on_event_index == kInvalidIndex) {
    assert(!"Maximum event count exceeded!");
    return;
  }

  const uint32_t note_off_event_index = engine_.event_pool.Acquire();
  if (note_off_event_index == kInvalidIndex) {
    engine_.event_pool.Release(note_on_event_index);
    assert(!"Maximum event count exceeded!");
    return;
  }

  auto& instrument = engine_.GetInstrument(instrument_index);

  auto& note_on_event = engine_.GetEvent(note_on_event_index);
  note_on_event = {};
  note_on_event.pitch = pitch;
  note_on_event.position = position;
  note_on_event.type = EventType::kNoteOnEvent;

  auto& note_off_event = engine_.GetEvent(note_off_event_index);
  note_off_event = {};
  note_off_event.pitch = pitch;
  note_off_event.position = position + duration;
  note_off_event.type = EventType::kNoteOffEvent;

  InsertScheduledEvent(engine_, instrument.first_event_index, note_on_event_index);
  InsertScheduledEvent(engine_, instrument.first_note_off_event_index, note_off_event_index);
}

void InstrumentController::ScheduleNoteControl(uint32_t instrument_index, float pitch,
                                               BarelyNoteControlType type, float value,
                                               double position, double duration) noexcept {
  const uint32_t note_control_event_index = engine_.event_pool.Acquire();
  if (note_control_event_index == kInvalidIndex) {
    assert(!"Maximum event count exceeded!");
    return;
  }

  auto& instrument = engine_.GetInstrument(instrument_index);

  auto& note_control_event = engine_.GetEvent(note_control_event_index);
  note_control_event = {};
  note_control_event.note_control.pitch = pitch;
  note_control_event.note_control.type = type;
  note_control_event.note_control.value = value;
  note_control_event.position = position;
  note_control_event.type = EventType::kNoteControlEvent;
  // TODO(schedule): Support duration
  duration;

  InsertScheduledEvent(engine_, instrument.first_event_index, note_control_event_index);
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
    if (!instrument.IsArpEnabled() ||
        (instrument.arp.is_note_on && note_index == instrument.arp.note_index)) {
      instrument.note_off_callback(note.pitch);
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
        if (static_cast<BarelyArpMode>(control.value) == BarelyArpMode_kNone) {
          if (instrument.arp.note_index != kInvalidIndex) {
            if (instrument.arp.is_note_on) {
              const float pitch = engine_.note_pool.Get(instrument.arp.note_index).pitch;
              instrument.note_off_callback(pitch);
              engine_.ScheduleMessage(NoteOffMessage{instrument.arp.note_index});
              if (instrument.arp.should_release_note) {
                ReleaseNote(instrument, instrument.arp.note_index);
              }
            }
            instrument.arp = {};
            uint32_t note_index = instrument.first_note_index;
            for (uint32_t i = 0; i < instrument.note_count; ++i) {
              const auto& note = engine_.note_pool.Get(note_index);
              instrument.note_on_callback(note.pitch);
              engine_.ScheduleMessage(NoteOnMessage{note_index, instrument_index, note.pitch});
              note_index = note.next_note_index;
            }
          }
        } else if (!instrument.arp.is_note_on) {
          uint32_t note_index = instrument.first_note_index;
          for (uint32_t i = 0; i < instrument.note_count; ++i) {
            const auto& note = engine_.note_pool.Get(note_index);
            instrument.note_off_callback(note.pitch);
            engine_.ScheduleMessage(NoteOffMessage{note_index});
            note_index = note.next_note_index;
          }
          instrument.arp.note_index = instrument.first_note_index;
        }
        break;
      case BarelyInstrumentControlType_kArpGate:
        [[fallthrough]];
      case BarelyInstrumentControlType_kArpRate:
        break;
      default:
        engine_.ScheduleMessage(InstrumentControlMessage{instrument_index, type, control.value});
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

void InstrumentController::SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);

  const uint32_t note_index = GetNote(instrument, pitch);
  if (note_index == kInvalidIndex) {
    return;
  }

  if (!instrument.IsArpEnabled()) {
    instrument.note_off_callback(pitch);
    engine_.ScheduleMessage(NoteOffMessage{note_index});
    ReleaseNote(instrument, note_index);
  } else if (instrument.note_count == 1 ||
             (instrument.note_count == 2 && instrument.arp.should_release_note)) {
    SetAllNotesOff(instrument_index);
  } else if (instrument.arp.note_index != note_index) {
    ReleaseNote(instrument, note_index);
  } else if (!instrument.arp.is_note_on) {
    UpdateArpNote(instrument);
    ReleaseNote(instrument, note_index);
  } else {
    instrument.arp.should_release_note = true;
  }
}

void InstrumentController::SetNoteOffCallback(uint32_t instrument_index,
                                              BarelyNoteCallback callback,
                                              void* user_data) noexcept {
  engine_.GetInstrument(instrument_index).note_off_callback = {callback, user_data};
}

void InstrumentController::SetNoteOn(uint32_t instrument_index, float pitch) noexcept {
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
    instrument.note_on_callback(pitch);
    engine_.ScheduleMessage(NoteOnMessage{new_note_index, instrument_index, pitch});
  } else if (instrument.note_count == 1) {
    instrument.arp.note_index = instrument.first_note_index;
  }
}

void InstrumentController::SetNoteOnCallback(uint32_t instrument_index, BarelyNoteCallback callback,
                                             void* user_data) noexcept {
  engine_.GetInstrument(instrument_index).note_on_callback = {callback, user_data};
}

void InstrumentController::SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                                         int32_t slice_count) noexcept {
  engine_.queued_sample_data_counts[instrument_index].fetch_add(1, std::memory_order_acq_rel);
  while (engine_.process_fence.load(std::memory_order_acquire));  // busy wait during next process.
  auto& instrument = engine_.GetInstrument(instrument_index);
  engine_.slice_pool.Release(instrument.first_slice_index);
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

void InstrumentController::ProcessAllEventsAtPosition(double position) noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    const uint32_t instrument_index = engine_.instrument_pool.GetActive(i);
    auto& instrument = engine_.GetInstrument(instrument_index);

    // note off
    uint32_t event_index = instrument.first_note_off_event_index;
    while (event_index != kInvalidIndex) {
      auto& event = engine_.GetEvent(event_index);
      if (event.position > position) {
        break;
      }
      SetNoteOff(instrument_index, event.pitch);
      const uint32_t next_event_index = event.next_event_index;
      engine_.event_pool.Release(event_index);
      event_index = next_event_index;
    }
    instrument.first_note_off_event_index = event_index;
    if (instrument.first_note_off_event_index != kInvalidIndex) {
      engine_.GetEvent(instrument.first_note_off_event_index).prev_event_index = kInvalidIndex;
    }

    // events
    event_index = instrument.first_event_index;
    while (event_index != kInvalidIndex) {
      auto& event = engine_.GetEvent(event_index);
      if (event.position > position) {
        break;
      }
      switch (event.type) {
        case EventType::kControlEvent:
          SetControl(instrument_index, event.control.type, event.control.value);
          break;
        case EventType::kNoteControlEvent:
          SetNoteControl(instrument_index, event.note_control.pitch, event.note_control.type,
                         event.note_control.value);
          break;
        case EventType::kNoteOnEvent:
          SetNoteOn(instrument_index, event.pitch);
          break;
        default:
          assert(!"Invalid event type");
          break;
      }
      const uint32_t next_event_index = event.next_event_index;
      engine_.event_pool.Release(event_index);
      event_index = next_event_index;
    }
    instrument.first_event_index = event_index;
    if (instrument.first_event_index != kInvalidIndex) {
      engine_.GetEvent(instrument.first_event_index).prev_event_index = kInvalidIndex;
    }

    // arp
    if (instrument.first_note_index == kInvalidIndex || !instrument.IsArpEnabled() ||
        instrument.controls[BarelyInstrumentControlType_kArpRate].value <= 0.0f) {
      continue;
    }
    if (!instrument.arp.is_note_on && instrument.arp.phase == 0.0f &&
        !instrument.arp.should_update && engine_.note_pool.IsActive(instrument.arp.note_index)) {
      const float pitch = engine_.note_pool.Get(instrument.arp.note_index).pitch;
      instrument.arp.is_note_on = true;
      instrument.note_on_callback(pitch);
      engine_.ScheduleMessage(NoteOnMessage{instrument.arp.note_index, instrument_index, pitch});
      if (instrument.controls[BarelyInstrumentControlType_kArpGate].value <= 0.0f) {
        instrument.arp.should_update = true;
        instrument.arp.is_note_on = false;
        instrument.note_off_callback(pitch);
        engine_.ScheduleMessage(NoteOffMessage{instrument.arp.note_index});
        UpdateArpNote(instrument);
      }
    }
  }
}

void InstrumentController::Update(double duration) noexcept {
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    const uint32_t instrument_index = engine_.instrument_pool.GetActive(i);
    auto& instrument = engine_.GetInstrument(instrument_index);
    if (instrument.first_note_index == kInvalidIndex || !instrument.IsArpEnabled() ||
        instrument.controls[BarelyInstrumentControlType_kArpRate].value <= 0.0f) {
      continue;
    }
    assert(duration <= instrument.GetNextArpDuration());
    const uint32_t note_index = instrument.arp.note_index;
    const double gate =
        static_cast<double>(instrument.controls[BarelyInstrumentControlType_kArpGate].value);
    const double rate =
        static_cast<double>(instrument.controls[BarelyInstrumentControlType_kArpRate].value);
    instrument.arp.phase = std::fmod(instrument.arp.phase + duration * rate, 1.0);
    instrument.arp.should_update = false;
    if (instrument.arp.is_note_on &&
        (instrument.arp.phase >= gate || (instrument.arp.phase == 0.0 && gate >= 1.0)) &&
        engine_.note_pool.IsActive(note_index)) {
      instrument.note_off_callback(engine_.note_pool.Get(note_index).pitch);
      engine_.ScheduleMessage(NoteOffMessage{note_index});
      UpdateArpNote(instrument);
      if (instrument.arp.should_release_note) {
        ReleaseNote(instrument, note_index);
        instrument.arp.should_release_note = false;
      }
      instrument.arp.is_note_on = false;
    }
  }
}

double InstrumentController::GetNextDuration(double position) const noexcept {
  double next_duration = std::numeric_limits<double>::max();
  for (uint32_t i = 0; i < engine_.instrument_pool.ActiveCount(); ++i) {
    const auto& instrument = engine_.GetInstrument(engine_.instrument_pool.GetActive(i));
    if (engine_.note_pool.IsActive(instrument.arp.note_index)) {
      next_duration = std::min(instrument.GetNextArpDuration(), next_duration);
    }
    if (instrument.first_event_index != kInvalidIndex) {
      next_duration = std::min(engine_.GetEvent(instrument.first_event_index).position - position,
                               next_duration);
    }
    if (instrument.first_note_off_event_index != kInvalidIndex) {
      next_duration =
          std::min(engine_.GetEvent(instrument.first_note_off_event_index).position - position,
                   next_duration);
    }
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
    instrument.arp.should_update = false;
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

void InstrumentController::UpdateArpNote(InstrumentState& instrument) noexcept {
  switch (
      static_cast<BarelyArpMode>(instrument.controls[BarelyInstrumentControlType_kArpMode].value)) {
    case BarelyArpMode_kUp:
      instrument.arp.note_index = engine_.note_pool.Get(instrument.arp.note_index).next_note_index;
      break;
    case BarelyArpMode_kDown:
      instrument.arp.note_index = engine_.note_pool.Get(instrument.arp.note_index).prev_note_index;
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

}  // namespace barely
