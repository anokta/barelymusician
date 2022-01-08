#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Constructs new |Instrument|.
  ///
  /// @param sample_rate Sampling rate in hz.
  /// @param definition Instrument definition.
  Instrument(int sample_rate, InstrumentDefinition definition) noexcept;

  /// Destroys |Instrument|.
  ~Instrument() noexcept;

  /// Copyable and movable.
  Instrument(const Instrument& other) = default;
  Instrument& operator=(const Instrument& other) = default;
  Instrument(Instrument&& other) noexcept = default;
  Instrument& operator=(Instrument&& other) noexcept = default;

  /// Processes the next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  void Process(float* output, int num_channels, int num_frames) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(void* data) noexcept;

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  void SetGain(float gain) noexcept;

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept;

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  void SetNoteOn(float pitch, float intensity) noexcept;

  /// Sets parameter.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  void SetParam(int index, float value) noexcept;

 private:
  // Instrument destroy function.
  InstrumentDefinition::DestroyFn destroy_fn_;

  // Instrument process function.
  InstrumentDefinition::ProcessFn process_fn_;

  // Instrument set data function.
  InstrumentDefinition::SetDataFn set_data_fn_;

  // Instrument set note off function.
  InstrumentDefinition::SetNoteOffFn set_note_off_fn_;

  // Instrument set note on function.
  InstrumentDefinition::SetNoteOnFn set_note_on_fn_;

  // Instrument set parameter function.
  InstrumentDefinition::SetParamFn set_param_fn_;

  // Instrument state.
  void* state_;

  // Instrument gain.
  float gain_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
