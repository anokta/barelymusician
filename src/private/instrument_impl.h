#ifndef BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_
#define BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <unordered_map>

#include "barelymusician.h"
#include "common/callback.h"
#include "common/message_queue.h"
#include "dsp/decibels.h"
#include "dsp/instrument_processor.h"
#include "dsp/sample_data.h"

namespace barely {

/// Class that implements an instrument.
class InstrumentImpl {
 public:
  /// Note off callback alias.
  using NoteOffCallback = Callback<BarelyInstrument_NoteOffCallback>;

  /// Note on callback alias.
  using NoteOnCallback = Callback<BarelyInstrument_NoteOnCallback>;

  /// Constructs a new `InstrumentImpl`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param update_sample Update sample.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentImpl(int sample_rate, float reference_frequency, int64_t update_sample) noexcept;

  /// Destroys `InstrumentImpl`.
  ~InstrumentImpl() noexcept;

  /// Non-copyable and non-movable.
  InstrumentImpl(const InstrumentImpl& other) noexcept = delete;
  InstrumentImpl& operator=(const InstrumentImpl& other) noexcept = delete;
  InstrumentImpl(InstrumentImpl&& other) noexcept = delete;
  InstrumentImpl& operator=(InstrumentImpl&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  [[nodiscard]] float GetControl(ControlType type) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @return Note control value.
  [[nodiscard]] const float* GetNoteControl(float pitch, NoteControlType type) const noexcept;

  /// Returns sampling rate.
  ///
  /// @return Sampling rate in hertz.
  [[nodiscard]] int GetSampleRate() const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Span of mono output samples.
  /// @param process_sample Process sample.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(std::span<float> output_samples, int64_t process_sample) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  void SetControl(ControlType type, float value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, NoteControlType type, float value) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept;

  /// Sets the note off callback.
  ///
  /// @param callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback callback) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(float pitch, float intensity) noexcept;

  /// Sets the note on callback.
  ///
  /// @param callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback callback) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(float reference_frequency) noexcept;

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData sample_data) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_sample Update sample.
  void Update(int64_t update_sample) noexcept;

 private:
  // Control.
  struct Control {
    /// Constructs a new `Control`.
    ///
    /// @param default_value Default value.
    /// @param min_value Minimum value.
    /// @param max_value Maximum value.
    template <typename ValueType>
    constexpr Control(ValueType default_value,
                      ValueType min_value = std::numeric_limits<ValueType>::lowest(),
                      ValueType max_value = std::numeric_limits<ValueType>::max()) noexcept
        : value(static_cast<float>(default_value)),
          min_value(static_cast<float>(min_value)),
          max_value(static_cast<float>(max_value)) {
      static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                    "ValueType is not supported");
      assert(default_value >= min_value && default_value <= max_value);
    }

    bool SetValue(float new_value) noexcept {
      new_value = std::clamp(new_value, min_value, max_value);
      if (value != new_value) {
        value = new_value;
        return true;
      }
      return false;
    }

    // Value.
    float value = 0.0f;

    // Minimum value.
    float min_value = std::numeric_limits<float>::lowest();

    // Maximum value.
    float max_value = std::numeric_limits<float>::max();
  };
  using ControlArray = std::array<Control, BarelyControlType_kCount>;
  using NoteControlArray = std::array<Control, BarelyNoteControlType_kCount>;

  // Sampling rate in hertz.
  const int sample_rate_ = 0;

  // Array of controls.
  ControlArray controls_ = {
      Control(0.0f, kMinDecibels, 0.0f),                   // kGain
      Control(0.0f),                                       // kPitchShift
      Control(false),                                      // kRetrigger
      Control(8, 1, 20),                                   // kVoiceCount
      Control(0.0f, 0.0f, 60.0f),                          // kAttack
      Control(0.0f, 0.0f, 60.0f),                          // kDecay
      Control(1.0f, 0.0f, 1.0f),                           // kSustain
      Control(0.0f, 0.0f, 60.0f),                          // kRelease
      Control(0.0f, -1.0f, 1.0f),                          // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),             // kOscMode
      Control(0.0f, 0.0f, 1.0f),                           // kOscNoiseRatio
      Control(0.0f),                                       // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                           // kOscShape
      Control(0.0f, -0.5f, 0.5f),                          // kOscSkew
      Control(0, 0, BarelySamplePlaybackMode_kCount - 1),  // kSamplePlaybackMode
      Control(0, 0, BarelyFilterType_kCount - 1),          // kFilterType
      Control(0.0f, 0.0f),                                 // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),                      // kFilterQ
      Control(16.0f, 1.0f, 16.0f),                         // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                           // kBitCrusherRate
  };

  // Map of note control arrays by their pitches.
  std::unordered_map<float, NoteControlArray> note_controls_;

  // Note off callback.
  NoteOffCallback note_off_callback_ = {};

  // Note on callback.
  NoteOnCallback note_on_callback_ = {};

  // Update sample.
  int64_t update_sample_ = 0;

  // Message queue.
  MessageQueue message_queue_;

  // Processor.
  InstrumentProcessor processor_;

  // TODO(#126): Temp hack to allow destroying by handle.
 public:
  BarelyEngineHandle engine = nullptr;
};

}  // namespace barely

struct BarelyInstrument : public barely::InstrumentImpl {};
static_assert(sizeof(BarelyInstrument) == sizeof(barely::InstrumentImpl));

#endif  // BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_
