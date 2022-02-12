#ifndef BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
#define BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"

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
  virtual void SetParameter(int index, float value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
BarelyInstrumentDefinition GetInstrumentDefinition(
    std::vector<BarelyParameterDefinition>& parameter_definitions) noexcept {
  return {.create_callback =
              [](void** state, int sample_rate) noexcept {
                *state =
                    reinterpret_cast<void*>(new InstrumentType(sample_rate));
              },
          .destroy_callback =
              [](void** state) noexcept {
                delete reinterpret_cast<InstrumentType*>(*state);
              },
          .process_callback =
              [](void** state, float* output, int num_channels,
                 int num_frames) noexcept {
                auto* instrument = reinterpret_cast<InstrumentType*>(*state);
                instrument->Process(output, num_channels, num_frames);
              },
          .set_data_callback =
              [](void** state, void* data) noexcept {
                auto* instrument = reinterpret_cast<InstrumentType*>(*state);
                instrument->SetData(data);
              },
          .set_note_off_callback =
              [](void** state, float pitch) noexcept {
                auto* instrument = reinterpret_cast<InstrumentType*>(*state);
                instrument->NoteOff(pitch);
              },
          .set_note_on_callback =
              [](void** state, float pitch, float intensity) noexcept {
                auto* instrument = reinterpret_cast<InstrumentType*>(*state);
                instrument->NoteOn(pitch, intensity);
              },
          .set_parameter_callback =
              [](void** state, int index, float value) noexcept {
                auto* instrument = reinterpret_cast<InstrumentType*>(*state);
                instrument->SetParameter(index, value);
              },
          .parameter_definitions = parameter_definitions.data(),
          .num_parameter_definitions =
              static_cast<int>(parameter_definitions.size())};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
