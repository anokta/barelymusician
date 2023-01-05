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

  /// Processes the next output buffer.
  ///
  /// @param output_samples Output samples.
  /// @param channel_count Number of output channels.
  /// @param frame_count Number of output frames.
  virtual void Process(double* output_samples, int channel_count,
                       int frame_count) noexcept = 0;

  /// Sets control value.
  ///
  /// @param id Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetControl(int index, double value,
                          double slope_per_frame) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOff(double pitch) noexcept = 0;

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOn(double pitch) noexcept = 0;
};

/// Returns instrument definition for `GenericInstrumentType`.
template <typename GenericInstrumentType>
InstrumentDefinition GetInstrumentDefinition(
    const std::vector<ControlDefinition>& control_definitions) noexcept {
  return InstrumentDefinition(
      [](void** state, int frame_rate) noexcept {
        // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
        *state = static_cast<void*>(new GenericInstrumentType(frame_rate));
      },
      [](void** state) noexcept {
        delete static_cast<GenericInstrumentType*>(*state);
        *state = nullptr;
      },
      [](void** state, double* output_samples, int output_channel_count,
         int output_frame_count) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->Process(output_samples, output_channel_count,
                            output_frame_count);
      },
      [](void** state, int index, double value, double slope_per_frame) {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetControl(index, value, slope_per_frame);
      },
      [](void** state, const void* data, int size) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetData(data, size);
      },
      [](void** state, double pitch) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOff(pitch);
      },
      [](void** state, double pitch) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOn(pitch);
      },
      control_definitions);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_INSTRUMENTS_GENERIC_INSTRUMENT_H_
