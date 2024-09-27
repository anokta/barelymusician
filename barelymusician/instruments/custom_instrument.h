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
  virtual void SetNoteControl(int pitch, int id, double value) noexcept = 0;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  virtual void SetNoteOff(int pitch) noexcept = 0;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  virtual void SetNoteOn(int pitch, double intensity) noexcept = 0;

  /// Sets the tuning.
  ///
  /// @param tuning Tuning definition.
  // TODO(#137): Temporary bypass, this should also be overridable.
  void SetTuning(const TuningDefinition&) noexcept { assert(false); }
  double GetFrequency(int pitch) noexcept {
    static constexpr double kSemitones[] = {
        1.0594630943592953,  // std::pow(2.0, 1.0 / 12.0)
        1.122462048309373,   // std::pow(2.0, 2.0 / 12.0)
        1.189207115002721,   // std::pow(2.0, 3.0 / 12.0)
        1.2599210498948732,  // std::pow(2.0, 4.0 / 12.0)
        1.3348398541700344,  // std::pow(2.0, 5.0 / 12.0)
        1.4142135623730951,  // std::pow(2.0, 6.0 / 12.0)
        1.4983070768766815,  // std::pow(2.0, 7.0 / 12.0)
        1.5874010519681994,  // std::pow(2.0, 8.0 / 12.0)
        1.681792830507429,   // std::pow(2.0, 9.0 / 12.0)
        1.7817974362806785,  // std::pow(2.0, 10.0 / 12.0)
        1.8877486253633868,  // std::pow(2.0, 11.0 / 12.0)
        2.0,                 // std::pow(2.0, 12.0 / 12.0)
    };
    static constexpr TuningDefinition midi_standard_tuning = {kSemitones, 440.0, 69};
    return midi_standard_tuning.GetFrequency(pitch);
  }

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
      using CustomInstrumentType::SetTuning;
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
        [](void** state, int32_t pitch, int32_t id, double value) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteControl(pitch, id, value);
        },
        [](void** state, int32_t pitch) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOff(pitch);
        },
        [](void** state, int32_t pitch, double intensity) noexcept {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetNoteOn(pitch, intensity);
        },
        [](void** state, const BarelyTuningDefinition* tuning) {
          auto* instrument = static_cast<PublicInstrument*>(*state);
          instrument->SetTuning(*tuning);
        },
        control_definitions, note_control_definitions);
  }
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_INSTRUMENTS_CUSTOM_INSTRUMENT_H_
