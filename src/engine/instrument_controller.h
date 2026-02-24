#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_

#include <barelymusician.h>

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
  InstrumentController(EngineState& engine) noexcept : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire(const BarelyInstrumentControlOverride* control_overrides,
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
  void SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                     int32_t slice_count) noexcept;

  [[nodiscard]] float GetControl(uint32_t instrument_index,
                                 BarelyInstrumentControlType type) const noexcept;
  [[nodiscard]] const float* GetNoteControl(uint32_t instrument_index, float pitch,
                                            BarelyNoteControlType type) const noexcept;
  [[nodiscard]] bool IsNoteOn(uint32_t instrument_index, float pitch) const noexcept;

  void ProcessArp() noexcept;
  void Update(double duration) noexcept;
  [[nodiscard]] double GetNextDuration() const noexcept;

 private:
  [[nodiscard]] uint32_t GetNote(const InstrumentState& instrument, float pitch) const noexcept;

  void ReleaseNote(InstrumentState& instrument, uint32_t note_index) noexcept;
  void ReleaseSampleData(InstrumentState& instrument) noexcept;
  void UpdateArpNote(InstrumentState& instrument) noexcept;

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
