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
  virtual void SetParameter(int index, double value) noexcept = 0;
};

/// Returns instrument definition for the given create instrument function.
template <typename InstrumentType>
BarelyInstrumentDefinition GetInstrumentDefinition(
    std::vector<BarelyParameterDefinition>& parameter_definitions) noexcept {
  return {.create_callback =
              [](BarelyInstrumentContext* context) noexcept {
                auto* instrument = new InstrumentType(context->sample_rate);
                for (int index = 0; index < context->num_parameter_snapshots;
                     ++index) {
                  instrument->SetParameter(
                      index, context->parameter_snapshots[index].value);
                }
                context->state = reinterpret_cast<void*>(instrument);
              },
          .destroy_callback =
              [](BarelyInstrumentContext* context) noexcept {
                delete reinterpret_cast<InstrumentType*>(context->state);
                context->state = nullptr;
              },
          .process_callback =
              [](BarelyInstrumentContext* context, float* output,
                 int num_channels, int num_frames) noexcept {
                auto* instrument =
                    reinterpret_cast<InstrumentType*>(context->state);
                // TODO: temporary workaround to ensure parameters are updated.
                for (int index = 0; index < context->num_parameter_snapshots;
                     ++index) {
                  instrument->SetParameter(
                      index, context->parameter_snapshots[index].value);
                }
                instrument->Process(output, num_channels, num_frames);
              },
          .set_data_callback =
              [](BarelyInstrumentContext* context, void* data) noexcept {
                auto* instrument =
                    reinterpret_cast<InstrumentType*>(context->state);
                // TODO: temporary workaround to ensure parameters are updated.
                for (int index = 0; index < context->num_parameter_snapshots;
                     ++index) {
                  instrument->SetParameter(
                      index, context->parameter_snapshots[index].value);
                }
                instrument->SetData(data);
              },
          .set_note_off_callback =
              [](BarelyInstrumentContext* context, float pitch) noexcept {
                auto* instrument =
                    reinterpret_cast<InstrumentType*>(context->state);
                // TODO: temporary workaround to ensure parameters are updated.
                for (int index = 0; index < context->num_parameter_snapshots;
                     ++index) {
                  instrument->SetParameter(
                      index, context->parameter_snapshots[index].value);
                }
                instrument->NoteOff(pitch);
              },
          .set_note_on_callback =
              [](BarelyInstrumentContext* context, float pitch,
                 float intensity) noexcept {
                auto* instrument =
                    reinterpret_cast<InstrumentType*>(context->state);
                // TODO: temporary workaround to ensure parameters are updated.
                for (int index = 0; index < context->num_parameter_snapshots;
                     ++index) {
                  instrument->SetParameter(
                      index, context->parameter_snapshots[index].value);
                }
                instrument->NoteOn(pitch, intensity);
              },
          .parameter_definitions = parameter_definitions.data(),
          .num_parameter_definitions =
              static_cast<int>(parameter_definitions.size())};
}

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_INSTRUMENTS_GENERIC_INSTRUMENT_H_
