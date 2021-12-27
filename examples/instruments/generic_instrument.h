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

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  virtual void NoteOff(float pitch) noexcept = 0;

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void NoteOn(float pitch, float intensity) noexcept = 0;

  /// Processes the next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  virtual void Process(float* output, int num_channels,
                       int num_frames) noexcept = 0;

  /// Sets custom data.
  ///
  /// @param data Custom data.
  virtual void SetCustomData(std::any data) noexcept = 0;

  /// Sets parameter value.
  ///
  /// @param id Parameter index.
  /// @param value Parameter value.
  virtual void SetParam(int index, float value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
InstrumentDefinition GetInstrumentDefinition(
    std::function<InstrumentType(int)> create_instrument_fn) noexcept {
  return InstrumentDefinition{
      .create_fn =
          [create_instrument_fn](InstrumentState* state,
                                 int sample_rate) noexcept {
            state->emplace<InstrumentType>(create_instrument_fn(sample_rate));
          },
      .destroy_fn = [](InstrumentState* state) noexcept { state->reset(); },
      .process_fn =
          [](InstrumentState* state, float* output, int num_channels,
             int num_frames) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_custom_data_fn =
          [](InstrumentState* state, std::any data) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetCustomData(std::move(data));
          },
      .set_note_off_fn =
          [](InstrumentState* state, float pitch) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOff(pitch);
          },
      .set_note_on_fn =
          [](InstrumentState* state, float pitch, float intensity) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOn(pitch, intensity);
          },
      .set_param_fn =
          [](InstrumentState* state, int param_index,
             float param_value) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetParam(param_index, param_value);
          }};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
