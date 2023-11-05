#ifndef BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
#include <vector>

namespace barely {

/// Custom instrument interface.
class CustomInstrument {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~CustomInstrument() = default;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  virtual void Process(double* output_samples, int output_channel_count,
                       int output_frame_count) noexcept = 0;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_frame Control slope in value change per frame.
  virtual void SetControl(int index, double value, double slope_per_frame) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_frame Note control slope in value change per frame.
  virtual void SetNoteControl(double pitch, int index, double value,
                              double slope_per_frame) noexcept = 0;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOff(double pitch) noexcept = 0;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void SetNoteOn(double pitch, double intensity) noexcept = 0;

  /// Returns the definition for `CustomInstrumentType`.
  ///
  /// @param control_definitions Array of control definitions.
  /// @param note_control_definitions Array of note control definitions.
  /// @return Instrument definition.
  template <typename CustomInstrumentType>
  static InstrumentDefinition GetDefinition(
      const std::vector<ControlDefinition>& control_definitions,
      const std::vector<ControlDefinition>& note_control_definitions) noexcept {
    class PublicInstrument : public CustomInstrumentType {
     public:
      explicit PublicInstrument(int frame_rate) : CustomInstrumentType(frame_rate) {}
      using CustomInstrumentType::Process;
      using CustomInstrumentType::SetControl;
      using CustomInstrumentType::SetData;
      using CustomInstrumentType::SetNoteControl;
      using CustomInstrumentType::SetNoteOff;
      using CustomInstrumentType::SetNoteOn;
    };
    return InstrumentDefinition(
        [](void** state, int frame_rate) noexcept {
          // NOLINTNEXTLINE(bugprone-unhandled-exception-at-new)
          *state = static_cast<void*>(new PublicInstrument(frame_rate));
        },
        [](void** state) noexcept {
          delete static_cast<PublicInstrument*>(*state);
          *state = nullptr;
        },
        [](void** state, double* output_samples, int output_channel_count,
           int output_frame_count) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->Process(output_samples, output_channel_count, output_frame_count);
        },
        [](void** state, int index, double value, double slope_per_frame) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetControl(index, value, slope_per_frame);
        },
        [](void** state, const void* data, int size) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetData(data, size);
        },
        [](void** state, double pitch, int index, double value, double slope_per_frame) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteControl(pitch, index, value, slope_per_frame);
        },
        [](void** state, double pitch) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOff(pitch);
        },
        [](void** state, double pitch, double intensity) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOn(pitch, intensity);
        },
        control_definitions, note_control_definitions);
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_
