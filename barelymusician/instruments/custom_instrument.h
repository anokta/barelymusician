#ifndef BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
#include <cassert>
#include <cstdint>
#include <new>
#include <span>

namespace barely {

/// Custom instrument interface.
class CustomInstrument {
 protected:
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
  /// @param control_id Control identifier.
  /// @param value Control value.
  virtual void SetControl(int control_id, double value) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;

  /// Sets a note control value.
  ///
  /// @param note_id Note identifier.
  /// @param control_id Note control identifier.
  /// @param value Note control value.
  virtual void SetNoteControl(int note_id, int control_id, double value) noexcept = 0;

  /// Sets a note off.
  ///
  /// @param note_id Note identifier.
  virtual void SetNoteOff(int note_id) noexcept = 0;

  /// Sets a note on.
  ///
  /// @param note_id Note identifier.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void SetNoteOn(int note_id, double pitch, double intensity) noexcept = 0;

  /// Returns the definition for `CustomInstrumentType`.
  ///
  /// @param control_definitions Span of control definitions.
  /// @param note_control_definitions Span of note control definitions.
  /// @return Instrument definition.
  template <typename CustomInstrumentType>
  static InstrumentDefinition GetDefinition(
      std::span<const ControlDefinition> control_definitions,
      std::span<const ControlDefinition> note_control_definitions) noexcept {
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
        [](void** state, int32_t frame_rate) noexcept {
          *state = static_cast<void*>(new (std::nothrow) PublicInstrument(frame_rate));
          assert(*state);
        },
        [](void** state) noexcept {
          delete static_cast<PublicInstrument*>(*state);
          *state = nullptr;
        },
        [](void** state, double* output_samples, int32_t output_channel_count,
           int32_t output_frame_count) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->Process(output_samples, output_channel_count, output_frame_count);
        },
        [](void** state, int32_t control_id, double value) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetControl(control_id, value);
        },
        [](void** state, const void* data, int32_t size) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetData(data, size);
        },
        [](void** state, int32_t note_id, int32_t control_id, double value) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteControl(note_id, control_id, value);
        },
        [](void** state, int32_t note_id) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOff(note_id);
        },
        [](void** state, int32_t note_id, double pitch, double intensity) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOn(note_id, pitch, intensity);
        },
        control_definitions, note_control_definitions);
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_
