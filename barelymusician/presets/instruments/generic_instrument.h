#ifndef BARELYMUSICIAN_PRESETS_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_PRESETS_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Generic instrument interface.
class GenericInstrument {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~GenericInstrument() = default;

  /// Processes the next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  virtual void Process(double* output, int num_channels,
                       int num_frames) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  virtual void SetData(void* data) noexcept = 0;

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOff(double pitch) noexcept = 0;

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void SetNoteOn(double pitch, double intensity) noexcept = 0;

  /// Sets parameter value.
  ///
  /// @param id Parameter index.
  /// @param value Parameter value.
  /// @param slope Parameter slope in value change per frame.
  virtual void SetParameter(int index, double value, double slope) noexcept = 0;
};

/// Returns instrument definition for `GenericInstrumentType`.
template <typename GenericInstrumentType>
barely::InstrumentDefinition GetInstrumentDefinition(
    const std::vector<barely::ParameterDefinition>&
        parameter_definitions) noexcept {
  return barely::InstrumentDefinition(
      [](void** state, int frame_rate) noexcept {
        *state = static_cast<void*>(new GenericInstrumentType(frame_rate));
      },
      [](void** state) noexcept {
        delete static_cast<GenericInstrumentType*>(*state);
        *state = nullptr;
      },
      [](void** state, double* output, int num_channels,
         int num_frames) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->Process(output, num_channels, num_frames);
      },
      [](void** state, void* data) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetData(data);
      },
      [](void** state, double pitch) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOff(pitch);
      },
      [](void** state, double pitch, double intensity) noexcept {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetNoteOn(pitch, intensity);
      },
      [](void** state, int index, double value, double slope) {
        auto* instrument = static_cast<GenericInstrumentType*>(*state);
        instrument->SetParameter(index, value, slope);
      },
      parameter_definitions);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_INSTRUMENTS_GENERIC_INSTRUMENT_H_
