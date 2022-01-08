#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <any>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/param_definition.h"

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
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  virtual void SetParam(barelyapi::ParamId id, float value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
barelyapi::InstrumentDefinition GetInstrumentDefinition(
    std::function<InstrumentType(int)> create_instrument_fn,
    std::vector<barelyapi::ParamDefinition> param_definitions) noexcept {
  return barelyapi::InstrumentDefinition{
      .create_fn =
          [create_instrument_fn](barelyapi::InstrumentState* state,
                                 int sample_rate) noexcept {
            state->emplace<InstrumentType>(create_instrument_fn(sample_rate));
          },
      .destroy_fn =
          [](barelyapi::InstrumentState* state) noexcept { state->reset(); },
      .process_fn =
          [](barelyapi::InstrumentState* state, float* output, int num_channels,
             int num_frames) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_custom_data_fn =
          [](barelyapi::InstrumentState* state, std::any data) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetCustomData(std::move(data));
          },
      .set_note_off_fn =
          [](barelyapi::InstrumentState* state, float pitch) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOff(pitch);
          },
      .set_note_on_fn =
          [](barelyapi::InstrumentState* state, float pitch,
             float intensity) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->NoteOn(pitch, intensity);
          },
      .set_param_fn =
          [](barelyapi::InstrumentState* state, barelyapi::ParamId param_id,
             float param_value) noexcept {
            auto* instrument = std::any_cast<InstrumentType>(state);
            instrument->SetParam(param_id, param_value);
          },
      .param_definitions = std::move(param_definitions)};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
