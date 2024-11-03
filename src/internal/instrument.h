#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <unordered_map>

#include "barelymusician.h"
#include "dsp/oscillator.h"
#include "dsp/sample_player.h"
#include "internal/event.h"
#include "internal/instrument_processor.h"
#include "internal/message_queue.h"
#include "internal/sample_data.h"

namespace barely::internal {

/// Class that controls an instrument.
class Instrument {
 public:
  /// Constructs a new `Instrument`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param update_frame Update frame.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(int frame_rate, double reference_frequency, int64_t update_frame) noexcept;

  /// Destroys `Instrument`.
  ~Instrument() noexcept;

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  [[nodiscard]] double GetControl(ControlType type) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @return Note control value.
  [[nodiscard]] const double* GetNoteControl(double pitch, NoteControlType type) const noexcept;

  /// Returns frame rate.
  ///
  /// @return Frame rate in hertz.
  [[nodiscard]] int GetFrameRate() const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param process_frame Process frame.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(double* output_samples, int output_channel_count, int output_frame_count,
               int64_t process_frame) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  void SetControl(ControlType type, double value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(double pitch, NoteControlType type, double value) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets the note off event.
  ///
  /// @param note_off_event Pointer to note off event.
  void SetNoteOffEvent(const NoteOffEvent* note_off_event) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch, double intensity) noexcept;

  /// Sets the note on event.
  ///
  /// @param note_on_event Pointer to note on event.
  void SetNoteOnEvent(const NoteOnEvent* note_on_event) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(double reference_frequency) noexcept;

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData sample_data) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_frame Update frame.
  void Update(int64_t update_frame) noexcept;

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
        : value(static_cast<double>(default_value)),
          min_value(static_cast<double>(min_value)),
          max_value(static_cast<double>(max_value)) {
      static_assert(std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
                    "ValueType is not supported");
      assert(default_value >= min_value && default_value <= max_value);
    }

    bool SetValue(double new_value) noexcept {
      new_value = std::min(std::max(new_value, min_value), max_value);
      if (value != new_value) {
        value = new_value;
        return true;
      }
      return false;
    }

    // Value.
    double value = 0.0;

    // Minimum value.
    double min_value = std::numeric_limits<double>::lowest();

    // Maximum value.
    double max_value = std::numeric_limits<double>::max();
  };
  using ControlArray = std::array<Control, static_cast<int>(BarelyControlType_kCount)>;
  using NoteControlArray = std::array<Control, static_cast<int>(BarelyNoteControlType_kCount)>;

  // Frame rate in hertz.
  const int frame_rate_ = 0;

  // Array of controls.
  ControlArray controls_ = {
      Control(1.0, 0.0, 1.0),                                                // kGain
      Control(8, 1, 32),                                                     // kVoiceCount
      Control(0, 0, static_cast<int>(BarelyOscillatorShape_kCount) - 1),     // kOscillatorShape
      Control(0, 0, static_cast<int>(BarelySamplePlaybackMode_kCount) - 1),  // kSamplePlaybackMode
      Control(0.05, 0.0, 60.0),                                              // kAttack
      Control(0.0, 0.0, 60.0),                                               // kDecay
      Control(1.0, 0.0, 1.0),                                                // kSustain
      Control(0.25, 0.0, 60.0),                                              // kRelease
      Control(0.0),                                                          // kPitchShift
      Control(false),                                                        // kRetrigger
  };

  // Map of note control arrays by their pitches.
  std::unordered_map<double, NoteControlArray> note_controls_;

  // Note off event.
  Event<NoteOffEvent, double> note_off_event_;

  // Note on event.
  Event<NoteOnEvent, double, double> note_on_event_;

  // Update frame.
  int64_t update_frame_ = 0;

  // Message queue.
  MessageQueue message_queue_;

  // Processor.
  InstrumentProcessor processor_;
};

}  // namespace barely::internal

struct BarelyInstrument : public barely::internal::Instrument {};
static_assert(sizeof(BarelyInstrument) == sizeof(barely::internal::Instrument));

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
