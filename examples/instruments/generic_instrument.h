#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <functional>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi::examples {

/// Generic instrument interface.
class GenericInstrument {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~GenericInstrument() = default;

  /// Stops note with the given |pitch|.
  ///
  /// @param pitch Note pitch.
  virtual void NoteOff(float pitch) = 0;

  /// Starts note with the given |pitch| and |intensity|.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void NoteOn(float pitch, float intensity) = 0;

  /// Processes the next |output| buffer.
  ///
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  virtual void Process(float* output, int num_channels, int num_frames) = 0;

  /// Sets custom |data|.
  ///
  /// @param data Custom data.
  virtual void SetCustomData(void* data) = 0;

  /// Sets param |value| with the given |id|.
  ///
  /// @param id Param id.
  /// @param value Param value.
  virtual void SetParam(int id, float value) = 0;
};

/// Returns instrument definition for the given create instrument function.
InstrumentDefinition GetInstrumentDefinition(
    std::function<std::unique_ptr<GenericInstrument>(int)>
        create_instrument_fn);

}  // namespace barelyapi::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
