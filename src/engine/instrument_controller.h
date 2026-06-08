#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <barelymusician.h>

#include <atomic>
#include <cassert>
#include <cstdint>

#include "core/constants.h"
#include "core/control.h"
#include "core/pool.h"
#include "core/rng.h"
#include "engine/engine_state.h"
#include "engine/message.h"
#include "engine/message_queue.h"

namespace barely {

class InstrumentController {
 public:
  explicit InstrumentController(EngineState& engine) noexcept : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept {
    const uint32_t instrument_index = engine_.instrument_pool.Acquire();
    if (instrument_index != kInvalidIndex) {
      auto& instrument = engine_.GetInstrument(instrument_index);
      instrument = {};
      engine_.ScheduleMessage(InstrumentCreateMessage{instrument_index});
    }
    return instrument_index;
  }

  void Release(uint32_t instrument_index) noexcept {
    engine_.queued_sample_data_counts[instrument_index].fetch_add(1, std::memory_order_acq_rel);
    while (engine_.process_fence.load(std::memory_order_acquire));  // busy wait until next process.
    auto& instrument = engine_.GetInstrument(instrument_index);
    engine_.slice_pool.Release(instrument.first_slice_index);
    engine_.ScheduleMessage(InstrumentDestroyMessage{instrument_index});
    engine_.instrument_pool.Release(instrument_index);
  }

  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept {
    assert(type <= BarelyInstrumentControlType_kCount);
    engine_.ScheduleMessage(
        InstrumentControlMessage{instrument_index, type, kInstrumentControls[type].Clamp(value)});
  }

  void SetNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                      float value) noexcept {
    assert(type <= BarelyNoteControlType_kCount);
    engine_.ScheduleMessage(
        NoteControlMessage{instrument_index, pitch, type, kNoteControls[type].Clamp(value)});
  }

  void SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
    engine_.ScheduleMessage(NoteOffMessage{instrument_index, pitch});
  }

  void SetNoteOn(uint32_t instrument_index, float pitch) noexcept {
    engine_.ScheduleMessage(NoteOnMessage{instrument_index, pitch});
  }

  void SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                     int32_t slice_count) noexcept {
    engine_.queued_sample_data_counts[instrument_index].fetch_add(1, std::memory_order_acq_rel);
    while (engine_.process_fence.load(std::memory_order_acquire));  // busy wait until next process.
    auto& instrument = engine_.GetInstrument(instrument_index);
    engine_.slice_pool.Release(instrument.first_slice_index);
    instrument.first_slice_index =
        engine_.slice_pool.Acquire(slices, static_cast<uint32_t>(slice_count));
    engine_.ScheduleMessage(SampleDataMessage{instrument_index, instrument.first_slice_index});
  }

 private:
  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
