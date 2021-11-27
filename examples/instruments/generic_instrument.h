#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <any>
#include <functional>
#include <utility>

#include "barelymusician/engine/instrument_definition.h"

namespace barely::examples {

/// Generic instrument interface.
class GenericInstrument {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~GenericInstrument() = default;

  /// Stops note with the given |pitch|.
  ///
  /// @param pitch Note pitch.
  virtual void NoteOff(float pitch) noexcept = 0;

  /// Starts note with the given |pitch| and |intensity|.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void NoteOn(float pitch, float intensity) noexcept = 0;

  /// Processes the next |output| buffer.
  ///
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  virtual void Process(float* output, int num_channels,
                       int num_frames) noexcept = 0;

  /// Sets custom |data|.
  ///
  /// @param data Custom data.
  virtual void SetCustomData(std::any data) noexcept = 0;

  /// Sets param |value| with the given |id|.
  ///
  /// @param id Param id.
  /// @param value Param value.
  virtual void SetParam(int id, float value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
InstrumentDefinition GetInstrumentDefinition(
    std::function<InstrumentType(int)> create_instrument_fn) noexcept {
  return InstrumentDefinition{
      .create_fn =
          [create_instrument_fn](InstrumentState* state, int sample_rate) {
            state->emplace<InstrumentType>(create_instrument_fn(sample_rate));
          },
      .destroy_fn = [](InstrumentState* state) { state->reset(); },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_custom_data_fn =
          [](InstrumentState* state, std::any data) {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetCustomData(std::move(data));
          },
      .set_note_off_fn =
          [](InstrumentState* state, float pitch) {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOff(pitch);
          },
      .set_note_on_fn =
          [](InstrumentState* state, float pitch, float intensity) {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOn(pitch, intensity);
          },
      .set_param_fn =
          [](InstrumentState* state, int param_id, float param_value) {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetParam(param_id, param_value);
          }};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
