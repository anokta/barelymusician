#ifndef BARELYMUSICIAN_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Generic instrument interface.
class GenericInstrument {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~GenericInstrument() = default;

  /// Processes the next output samples.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param channel_count Number of output channels.
  /// @param frame_count Number of output frames.
  virtual void Process(Real* output_samples, Integer channel_count,
                       Integer frame_count) noexcept = 0;

  /// Sets control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetControl(Integer index, Real value,
                          Real slope_per_frame) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, Integer size) noexcept = 0;

  /// Sets note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetNoteControl(Real pitch, Integer index, Real value,
                              Real slope_per_frame) noexcept = 0;

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOff(Real pitch) noexcept = 0;

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOn(Real pitch) noexcept = 0;
};

/// Returns instrument definition for `GenericInstrumentType`.
template <typename GenericInstrumentType>
InstrumentDefinition GetInstrumentDefinition(
    const std::vector<ControlDefinition>& control_definitions,
    const std::vector<ControlDefinition>& note_control_definitions) noexcept {
  return InstrumentDefinition(
      [](void** state, Integer frame_rate) noexcept {
        // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
        *state = static_cast<void*>(new GenericInstrumentType(frame_rate));
      },
      [](void** state) noexcept {
        delete static_cast<GenericInstrumentType*>(*state);
        *state = nullptr;
      },
      [](void** state, Real* output_samples, Integer output_channel_count,
         Integer output_frame_count) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->Process(output_samples, output_channel_count,
                            output_frame_count);
      },
      [](void** state, Integer index, Real value, Real slope_per_frame) {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetControl(index, value, slope_per_frame);
      },
      [](void** state, const void* data, Integer size) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetData(data, size);
      },
      [](void** state, Real pitch, Integer index, Real value,
         Real slope_per_frame) {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteControl(pitch, index, value, slope_per_frame);
      },
      [](void** state, Real pitch) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOff(pitch);
      },
      [](void** state, Real pitch) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOn(pitch);
      },
      control_definitions, note_control_definitions);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_GENERIC_INSTRUMENT_H_
