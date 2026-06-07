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
  SetSampleData(instrument_index, nullptr, 0);
  engine_.ScheduleMessage(InstrumentDestroyMessage{instrument_index});
  engine_.instrument_pool.Release(instrument_index);
}

void InstrumentController::SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                                      float value) noexcept {
  if (auto& control = engine_.GetInstrument(instrument_index).controls[type];
      control.SetValue(value)) {
    engine_.ScheduleMessage(InstrumentControlMessage{instrument_index, type, control.value});
  }
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

}  // namespace barely
