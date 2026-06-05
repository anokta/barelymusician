#include "engine/instrument_controller.h"

#include <barelymusician.h>

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>

#include "core/constants.h"
#include "core/control.h"
#include "engine/instrument_state.h"
#include "engine/message.h"

namespace barely {

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
  SetAllNotesOff(instrument_index);
  SetSampleData(instrument_index, nullptr, 0);
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
  while (note_index != kInvalidIndex) {
    const auto& note = engine_.note_pool.Get(note_index);
    instrument.note_event_callback(BarelyEventType_kEnd, note.pitch);
    engine_.ScheduleMessage(NoteOffMessage{note_index});
    const uint32_t next_note_index = note.next_note_index;
    engine_.note_pool.Release(note_index);
    note_index = next_note_index;
  }
  instrument.first_note_index = kInvalidIndex;
}

void InstrumentController::SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                                      float value) noexcept {
  if (auto& control = engine_.GetInstrument(instrument_index).controls[type];
      control.SetValue(value)) {
    engine_.ScheduleMessage(InstrumentControlMessage{instrument_index, type, control.value});
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
  instrument.note_event_callback(BarelyEventType_kEnd, pitch);
  engine_.ScheduleMessage(NoteOffMessage{note_index});
  ReleaseNote(instrument, note_index);
}

void InstrumentController::SetNoteOn(uint32_t instrument_index, float pitch) noexcept {
  auto& instrument = engine_.GetInstrument(instrument_index);

  uint32_t note_index = instrument.first_note_index;
  while (note_index != kInvalidIndex) {
    auto& note = engine_.note_pool.Get(note_index);
    if (note.pitch == pitch) {  // note already exists.
      return;
    }
    if (note.next_note_index == kInvalidIndex || pitch < note.pitch) {
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

  if (note_index == kInvalidIndex) {
    instrument.first_note_index = new_note_index;
  } else {
    auto& note = engine_.note_pool.Get(note_index);
    if (pitch < note.pitch) {
      if (note_index == instrument.first_note_index) {
        instrument.first_note_index = new_note_index;
      }
      new_note.next_note_index = note_index;
      new_note.prev_note_index = note.prev_note_index;
      if (note.prev_note_index != kInvalidIndex) {
        engine_.note_pool.Get(new_note.prev_note_index).next_note_index = new_note_index;
      }
      note.prev_note_index = new_note_index;
    } else {
      new_note.prev_note_index = note_index;
      new_note.next_note_index = note.next_note_index;
      if (note.next_note_index != kInvalidIndex) {
        engine_.note_pool.Get(note.next_note_index).prev_note_index = new_note_index;
      }
      note.next_note_index = new_note_index;
    }
  }

  instrument.note_event_callback(BarelyEventType_kBegin, pitch);
  engine_.ScheduleMessage(NoteOnMessage{new_note_index, instrument_index, pitch});
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

bool InstrumentController::IsNoteOn(uint32_t instrument_index, float pitch) const noexcept {
  return GetNote(engine_.GetInstrument(instrument_index), pitch) != kInvalidIndex;
}

uint32_t InstrumentController::GetNote(const InstrumentState& instrument,
                                       float pitch) const noexcept {
  uint32_t note_index = instrument.first_note_index;
  while (note_index != kInvalidIndex) {
    const auto& note = engine_.note_pool.Get(note_index);
    if (note.pitch == pitch) {
      return note_index;
    }
    note_index = note.next_note_index;
  }
  return note_index;
}

void InstrumentController::ReleaseNote(InstrumentState& instrument, uint32_t note_index) noexcept {
  auto& note = engine_.note_pool.Get(note_index);
  if (note.prev_note_index != kInvalidIndex) {
    engine_.note_pool.Get(note.prev_note_index).next_note_index = note.next_note_index;
  } else {
    instrument.first_note_index = note.next_note_index;
  }
  if (note.next_note_index != kInvalidIndex) {
    engine_.note_pool.Get(note.next_note_index).prev_note_index = note.prev_note_index;
  }
  engine_.note_pool.Release(note_index);
}

}  // namespace barely
