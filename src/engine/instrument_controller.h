#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <barelymusician.h>

#include <cstdint>

#include "core/pool.h"
#include "core/rng.h"
#include "engine/instrument_state.h"
#include "engine/message.h"
#include "engine/message_queue.h"

namespace barely {

class InstrumentController {
 public:
  InstrumentController(MessageQueue& message_queue, const int64_t& update_frame) noexcept
      : message_queue_(message_queue), update_frame_(update_frame) {}

  BarelyRef Acquire(const BarelyInstrumentControlOverride* control_overrides,
                    int32_t control_override_count) noexcept;
  void Release(uint32_t instrument_index) noexcept;

  void SetAllNotesOff() noexcept;
  void SetAllNotesOff(uint32_t instrument_index) noexcept;
  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept;
  void SetNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                      float value) noexcept;
  void SetNoteEventCallback(uint32_t instrument_index, BarelyNoteEventCallback callback,
                            void* user_data) noexcept;
  void SetNoteOff(uint32_t instrument_index, float pitch) noexcept;
  void SetNoteOn(uint32_t instrument_index, float pitch,
                 const BarelyNoteControlOverride* note_control_overrides,
                 int32_t note_control_override_count) noexcept;

  [[nodiscard]] float GetControl(uint32_t instrument_index,
                                 BarelyInstrumentControlType type) const noexcept;
  [[nodiscard]] const float* GetNoteControl(uint32_t instrument_index, float pitch,
                                            BarelyNoteControlType type) const noexcept;
  [[nodiscard]] bool IsNoteOn(uint32_t instrument_index, float pitch) const noexcept;

  [[nodiscard]] bool IsActive(BarelyRef instrument) const noexcept {
    return instrument_pool_.IsActive(instrument.index, instrument.generation);
  }

  // TODO(#126): clean this up?
  void ProcessArp(MainRng& main_rng) noexcept;
  void Update(double duration) noexcept;
  [[nodiscard]] double GetNextDuration() const noexcept;

 private:
  // Instrument pool.
  Pool<InstrumentState, BARELYMUSICIAN_MAX_INSTRUMENT_COUNT> instrument_pool_ = {};

  MessageQueue& message_queue_;
  const int64_t& update_frame_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
