#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <any>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Constructs new |Instrument|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param definition Instrument definition.
  Instrument(int sample_rate, InstrumentDefinition definition);

  /// Destroys |Instrument|.
  ~Instrument();

  /// Copyable and movable.
  Instrument(const Instrument& other) = default;
  Instrument& operator=(const Instrument& other) = default;
  Instrument(Instrument&& other) = default;
  Instrument& operator=(Instrument&& other) = default;

  /// Processes the next output buffer.
  ///
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(float* output, int num_channels, int num_frames);

  /// Sets custom data.
  ///
  /// @param data Custom data.
  void SetCustomData(std::any data);

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch);

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(float pitch, float intensity);

  /// Sets parameter.
  ///
  /// @param id Parameter id.
  /// @param value Parameter value.
  void SetParam(int id, float value);

 private:
  // Instrument definition.
  InstrumentDefinition definition_;

  // Instrument state.
  InstrumentState state_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
