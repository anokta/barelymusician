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
  explicit InstrumentController(EngineState& engine) noexcept : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept;
  void Release(uint32_t instrument_index) noexcept;

  void CancelAllScheduledEvents(uint32_t instrument_index) noexcept;
  void ScheduleControl(uint32_t instrument_index, BarelyInstrumentControlType type, float value,
                       double position, double duration) noexcept;
  void ScheduleNote(uint32_t instrument_index, float pitch, double position,
                    double duration) noexcept;
  void ScheduleNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                           float value, double position, double duration) noexcept;

  void SetAllNotesOff() noexcept;
  void SetAllNotesOff(uint32_t instrument_index) noexcept;
  void SetControl(uint32_t instrument_index, BarelyInstrumentControlType type,
                  float value) noexcept;
  void SetNoteControl(uint32_t instrument_index, float pitch, BarelyNoteControlType type,
                      float value) noexcept;
  void SetNoteOff(uint32_t instrument_index, float pitch) noexcept;
  void SetNoteOffCallback(uint32_t instrument_index, BarelyNoteCallback callback,
                          void* user_data) noexcept;
  void SetNoteOn(uint32_t instrument_index, float pitch) noexcept;
  void SetNoteOnCallback(uint32_t instrument_index, BarelyNoteCallback callback,
                         void* user_data) noexcept;
  void SetSampleData(uint32_t instrument_index, const BarelySlice* slices,
                     int32_t slice_count) noexcept;

  [[nodiscard]] float GetControl(uint32_t instrument_index,
                                 BarelyInstrumentControlType type) const noexcept;
  [[nodiscard]] const float* GetNoteControl(uint32_t instrument_index, float pitch,
                                            BarelyNoteControlType type) const noexcept;
  [[nodiscard]] bool IsNoteOn(uint32_t instrument_index, float pitch) const noexcept;

  void ProcessAllEventsAtPosition(double position) noexcept;
  void Update(double duration) noexcept;
  [[nodiscard]] double GetNextDuration(double position) const noexcept;

 private:
  [[nodiscard]] uint32_t GetNote(const InstrumentState& instrument, float pitch) const noexcept;

  void ReleaseNote(InstrumentState& instrument, uint32_t note_index) noexcept;
  void UpdateArpNote(InstrumentState& instrument) noexcept;

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_CONTROLLER_H_
