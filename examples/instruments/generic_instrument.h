#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_

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

  /// Sets data.
  ///
  /// @param data Data.
  virtual void SetData(void* data) noexcept = 0;

  /// Sets parameter value.
  ///
  /// @param id Parameter index.
  /// @param value Parameter value.
  virtual void SetParam(int index, float value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
barelyapi::InstrumentDefinition GetInstrumentDefinition(
    std::vector<barelyapi::ParamDefinition> param_definitions) noexcept {
  return barelyapi::InstrumentDefinition{
      .create_fn =
          [](void** state, int sample_rate) noexcept {
            *state = reinterpret_cast<void*>(new InstrumentType(sample_rate));
          },
      .destroy_fn =
          [](void** state) noexcept {
            delete reinterpret_cast<InstrumentType*>(*state);
          },
      .process_fn =
          [](void** state, float* output, int num_channels,
             int num_frames) noexcept {
            auto* instrument = reinterpret_cast<InstrumentType*>(*state);
            instrument->Process(output, num_channels, num_frames);
          },
      .set_data_fn =
          [](void** state, void* data) noexcept {
            auto* instrument = reinterpret_cast<InstrumentType*>(*state);
            instrument->SetData(data);
          },
      .set_note_off_fn =
          [](void** state, float pitch) noexcept {
            auto* instrument = reinterpret_cast<InstrumentType*>(*state);
            instrument->NoteOff(pitch);
          },
      .set_note_on_fn =
          [](void** state, float pitch, float intensity) noexcept {
            auto* instrument = reinterpret_cast<InstrumentType*>(*state);
            instrument->NoteOn(pitch, intensity);
          },
      .set_param_fn =
          [](void** state, int index, float value) noexcept {
            auto* instrument = reinterpret_cast<InstrumentType*>(*state);
            instrument->SetParam(index, value);
          },
      .param_definitions = std::move(param_definitions)};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
