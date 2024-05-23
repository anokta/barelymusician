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
  /// @param id Control identifier.
  /// @param value Control value.
  virtual void SetControl(int id, double value) noexcept = 0;

  /// Sets data.
  ///
  /// @param data Data.
  /// @param size Data size in bytes.
  virtual void SetData(const void* data, int size) noexcept = 0;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @param value Note control value.
  virtual void SetNoteControl(double pitch, int id, double value) noexcept = 0;

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
        [](void** state, int32_t id, double value) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetControl(id, value);
        },
        [](void** state, const void* data, int32_t size) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetData(data, size);
        },
        [](void** state, double pitch, int32_t id, double value) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteControl(pitch, id, value);
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
