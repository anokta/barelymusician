#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <barelymusician.h>

#include <cassert>
#include <cstdint>

#include "core/pool.h"
#include "core/rng.h"
#include "engine/engine_state.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "engine/message_queue.h"

namespace barely {

class InstrumentController {
 public:
  explicit InstrumentController(EngineState& engine) noexcept : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept;
  void Release(uint32_t instrument_index) noexcept;

  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept;
  void SetNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                      float value) noexcept {
    assert(type <= BarelyNoteControlType_kCount);
    engine_.ScheduleMessage(
        NoteControlMessage{instrument_index, pitch, type, kNoteControls[type].Sanitize(value)});
  }
  void SetNoteOff(uint32_t instrument_index, float pitch) noexcept {
    engine_.ScheduleMessage(NoteOffMessage{instrument_index, pitch});
  }
  void SetNoteOn(uint32_t instrument_index, float pitch) noexcept {
    engine_.ScheduleMessage(NoteOnMessage{instrument_index, pitch});
  }
  void SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                     int32_t slice_count) noexcept;

 private:
  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
