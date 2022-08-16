#ifndef BARELYMUSICIAN_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_BARELYMUSICIAN_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_EXPORT __attribute__((dllexport))
#else  // __GNUC__
#define BARELY_EXPORT __declspec(dllexport)
#endif  // __GNUC__
#else   // BARELYMUSICIAN_EXPORTS
#ifdef __GNUC__
#define BARELY_EXPORT __attribute__((dllimport))
#else  // __GNUC__
#define BARELY_EXPORT __declspec(dllimport)
#endif  // __GNUC__
#endif  // BARELYMUSICIAN_EXPORTS
#else   // defined(_WIN32) || defined(__CYGWIN__)
#if __GNUC__ >= 4
#define BARELY_EXPORT __attribute__((visibility("default")))
#else  // __GNUC__ >= 4
#define BARELY_EXPORT
#endif  // __GNUC__ >= 4
#endif  // defined(_WIN32) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Musician handle.
typedef struct BarelyMusician* BarelyMusicianHandle;

/// Identifier alias.
typedef int64_t BarelyId;

/// Identifier values.
enum BarelyId_Values {
  /// Invalid identifier.
  BarelyId_kInvalid = 0,
};

/// Status enum alias.
typedef int32_t BarelyStatus;

/// Status enum values.
enum BarelyStatus_Values {
  /// Success.
  BarelyStatus_kOk = 0,
  /// Invalid argument error.
  BarelyStatus_kInvalidArgument = 1,
  /// Not found error.
  BarelyStatus_kNotFound = 2,
  /// Already exists error.
  BarelyStatus_kAlreadyExists = 3,
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 4,
  /// Internal error.
  BarelyStatus_kInternal = 5,
  /// Unknown error.
  BarelyStatus_kUnknown = 6,
};

/// Instrument create callback signature.
///
/// @param state Pointer to instrument state.
/// @param frame_rate Frame rate in hz.
typedef void (*BarelyInstrumentDefinition_CreateCallback)(void** state,
                                                          int32_t frame_rate);

/// Instrument destroy callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyCallback)(void** state);

/// Instrument process callback signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    void** state, double* output, int32_t num_output_channels,
    int32_t num_output_frames);

/// Instrument set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Pointer to data.
/// @param size Data size in bytes.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state,
                                                           const void* data,
                                                           int32_t size);

/// Instrument set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state,
                                                              double pitch);

/// Instrument set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state,
                                                             double pitch,
                                                             double intensity);

/// Instrument set parameter callback signature.
///
/// @param state Pointer to instrument state.
/// @param index Parameter index.
/// @param value Parameter value.
/// @param slope Parameter slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetParameterCallback)(void** state,
                                                                int32_t index,
                                                                double value,
                                                                double slope);

/// Parameter definition.
typedef struct BarelyParameterDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyParameterDefinition;

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create callback.
  BarelyInstrumentDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyInstrumentDefinition_ProcessCallback process_callback;

  /// Set data callback.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback;

  /// Set note off callback.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback;

  /// Set note on callback.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback;

  /// Set parameter callback.
  BarelyInstrumentDefinition_SetParameterCallback set_parameter_callback;

  /// List of parameter definitions.
  const BarelyParameterDefinition* parameter_definitions;

  /// Number of parameter definitions.
  int32_t num_parameter_definitions;
} BarelyInstrumentDefinition;

/// Instrument note off callback signature.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_NoteOffCallback)(double pitch, void* user_data);

/// Instrument note on callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_NoteOnCallback)(double pitch, double intensity,
                                                void* user_data);

/// Sequencer event callback signature.
///
/// @param user_data Pointer to user data.
typedef void (*BarelySequencer_EventCallback)(void* user_data);

/// Creates new instrument.
///
/// @param handle Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hz.
/// @param out_instrument_id Output instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Create(
    BarelyMusicianHandle handle, BarelyInstrumentDefinition definition,
    int32_t frame_rate, BarelyId* out_instrument_id);

/// Destroys instrument.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                                    BarelyId instrument_id);

/// Gets instrument parameter value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    double* out_value);

/// Gets whether instrument note is playing or not.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle, BarelyId instrument_id,
                          double pitch, bool* out_is_note_on);

/// Processes instrument output buffer at timestamp.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyMusicianHandle handle, BarelyId instrument_id, double* output,
    int32_t num_output_channels, int32_t num_output_frames, double timestamp);

/// Resets all instrument parameters to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllParameters(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Resets instrument parameter to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index);

/// Sets instrument data.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param data Pointer to data.
/// @param size Data size in bytes.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                                    BarelyId instrument_id,
                                                    const void* data,
                                                    int32_t size);

/// Sets instrument note off callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note off callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback callback, void* user_data);

/// Sets instrument note on callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note on callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback callback, void* user_data);

/// Sets instrument parameter value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Parameter index.
/// @param value Parameter value.
/// @param slope Parameter slope in value change per second.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetParameter(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    double value, double slope);

/// Starts instrument note.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StartNote(BarelyMusicianHandle handle, BarelyId instrument_id,
                           double pitch, double intensity);

/// Stops all instrument notes.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopAllNotes(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Stops instrument note.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopNote(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Creates new musician.
///
/// @param out_handle Output musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Create(BarelyMusicianHandle* out_handle);

/// Destroys musician.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle);

/// Gets musician beats from seconds.
///
/// @param handle Musician handle.
/// @param seconds Number of seconds.
/// @param out_beats Output number of beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetBeats(BarelyMusicianHandle handle,
                                                   double seconds,
                                                   double* out_beats);

/// Gets musician seconds from beats.
///
/// @param handle Musician handle.
/// @param beats Number of beats.
/// @param out_seconds Output number of seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetSeconds(
    BarelyMusicianHandle handle, double beats, double* out_seconds);

/// Gets musician tempo.
///
/// @param handle Musician handle.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                                   double* out_tempo);

/// Gets musician timestamp.
///
/// @param handle Musician handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_GetTimestamp(BarelyMusicianHandle handle, double* out_timestamp);

/// Sets musician tempo.
///
/// @param handle Musician handle.
/// @param tempo Tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                                   double tempo);

/// Updates musician at timestamp.
///
/// @param handle Musician handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                                 double timestamp);

/// Adds sequencer event.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param position Event position in beats.
/// @param callback Event callback.
/// @param user_data Pointer to event user data.
/// @param out_event_id Output event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_AddEvent(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double position,
    BarelySequencer_EventCallback callback, void* user_data,
    BarelyId* out_event_id);

/// Creates new sequencer.
///
/// @param priority Sequencer priority for when executing events.
/// @param out_sequencer_id Output sequencer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_Create(BarelyMusicianHandle handle,
                                                  int32_t priority,
                                                  BarelyId* out_sequencer_id);

/// Destroys sequencer.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_Destroy(BarelyMusicianHandle handle,
                                                   BarelyId sequencer_id);

/// Gets sequencer event position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param event_id Event identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_GetEventPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id,
    double* out_position);

/// Gets sequencer loop begin position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id,
    double* out_loop_begin_position);

/// Gets sequencer loop length.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelySequencer_GetLoopLength(BarelyMusicianHandle handle,
                              BarelyId sequencer_id, double* out_loop_length);

/// Gets sequencer position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_GetPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double* out_position);

/// Gets whether sequencer is looping or not.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_IsLooping(
    BarelyMusicianHandle handle, BarelyId sequencer_id, bool* out_is_looping);

/// Gets whether sequencer is playing or not.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_IsPlaying(
    BarelyMusicianHandle handle, BarelyId sequencer_id, bool* out_is_playing);

/// Removes sequencer event.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param event_id Event identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_RemoveEvent(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id);

/// Schedules one-off sequencer event at position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param position Position in beats.
/// @param callback Event callback.
/// @param user_data Pointer to event user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_ScheduleOneOffEvent(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double position,
    BarelySequencer_EventCallback callback, void* user_data);

/// Sets sequencer event callback.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param event_id Event identifier.
/// @param callback Event callback.
/// @param user_data Pointer to event user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetEventCallback(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id,
    BarelySequencer_EventCallback callback, void* user_data);

/// Sets sequencer event position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param event_id Event identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetEventPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id, BarelyId event_id,
    double position);

/// Sets sequencer loop begin position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param loop_begin_position Loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id,
    double loop_begin_position);

/// Sets sequencer loop length.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetLoopLength(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double loop_length);

/// Sets whether sequencer should be looping or not.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetLooping(
    BarelyMusicianHandle handle, BarelyId sequencer_id, bool is_looping);

/// Sets sequencer position.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_SetPosition(
    BarelyMusicianHandle handle, BarelyId sequencer_id, double position);

/// Starts sequencer.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_Start(BarelyMusicianHandle handle,
                                                 BarelyId sequencer_id);

/// Stops sequencer.
///
/// @param handle Musician handle.
/// @param sequencer_id Sequencer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelySequencer_Stop(BarelyMusicianHandle handle,
                                                BarelyId sequencer_id);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <compare>
#include <functional>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

namespace barely {

/// Status.
class Status {
 public:
  /// Enum values.
  enum Enum : BarelyStatus {
    /// Success.
    kOk = BarelyStatus_kOk,
    /// Invalid argument error.
    kInvalidArgument = BarelyStatus_kInvalidArgument,
    /// Not found error.
    kNotFound = BarelyStatus_kNotFound,
    /// Already exists error.
    kAlreadyExists = BarelyStatus_kAlreadyExists,
    /// Unimplemented error.
    kUnimplemented = BarelyStatus_kUnimplemented,
    /// Internal error.
    kInternal = BarelyStatus_kInternal,
    /// Unknown error.
    kUnknown = BarelyStatus_kUnknown,
  };

  /// Constructs new `Status`.
  ///
  /// @param status Status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(Enum status) : status_(status) {}

  /// Constructs new `Status` from internal type.
  ///
  /// @param status Internal status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(BarelyStatus status) : status_(static_cast<Enum>(status)) {}

  /// Returns enum value.
  ///
  /// @return Enum value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Enum() const { return status_; }

  /// Enum comparators.
  auto operator<=>(Enum status) const { return status_ <=> status; }

  /// Returns whether status is okay or not.
  ///
  /// @return True if okay, false otherwise.
  [[nodiscard]] bool IsOk() const { return status_ == kOk; }

  /// Returns status string.
  ///
  /// @return Status string.
  [[nodiscard]] std::string ToString() const {
    switch (status_) {
      case kOk:
        return "Ok";
      case kInvalidArgument:
        return "Invalid argument error";
      case kNotFound:
        return "Not found error";
      case kAlreadyExists:
        return "Already exists error";
      case kUnimplemented:
        return "Unimplemented error";
      case kInternal:
        return "Internal error";
      case kUnknown:
      default:
        return "Unknown error";
    }
  }

 private:
  // Enum value.
  Enum status_;
};

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(Status error_status) : value_or_(error_status) {
    assert(!error_status.IsOk());
  }

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) : value_or_(std::move(value)) {}

  /// Returns contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// Parameter definition.
struct ParameterDefinition : public BarelyParameterDefinition {
  /// Constructs new `ParameterDefinition`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  explicit ParameterDefinition(
      double default_value,
      double min_value = std::numeric_limits<double>::lowest(),
      double max_value = std::numeric_limits<double>::max())
      : ParameterDefinition(
            BarelyParameterDefinition{default_value, min_value, max_value}) {}

  /// Constructs new `ParameterDefinition` for a boolean value.
  ///
  /// @param default_value Default boolean value.
  explicit ParameterDefinition(bool default_value)
      : ParameterDefinition(static_cast<double>(default_value)) {}

  /// Constructs new `ParameterDefinition` for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ParameterDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max())
      : ParameterDefinition(static_cast<double>(default_value),
                            static_cast<double>(min_value),
                            static_cast<double>(max_value)) {}

  /// Constructs new `ParameterDefinition` from internal type.
  ///
  /// @param definition Internal parameter definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ParameterDefinition(BarelyParameterDefinition definition)
      : BarelyParameterDefinition{definition} {
    assert(default_value >= min_value && default_value <= max_value);
  }
};

/// Instrument definition.
struct InstrumentDefinition : public BarelyInstrumentDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyInstrumentDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyInstrumentDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyInstrumentDefinition_ProcessCallback;

  /// Set data callback signature.
  using SetDataCallback = BarelyInstrumentDefinition_SetDataCallback;

  /// Set note off callback signature
  using SetNoteOffCallback = BarelyInstrumentDefinition_SetNoteOffCallback;

  /// Set note on callback signature.
  using SetNoteOnCallback = BarelyInstrumentDefinition_SetNoteOnCallback;

  /// Set parameter callback signature.
  using SetParameterCallback = BarelyInstrumentDefinition_SetParameterCallback;

  /// Constructs new `InstrumentDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_data_callback Set data callback.
  /// @param set_note_off_callback Set note off callback.
  /// @param set_note_on_callback Set note on callback.
  /// @param set_parameter_callback Set parameter callback.
  /// @param parameter_definitions List of parameter definitions.
  /// @param user_data Pointer to user data.
  InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetDataCallback set_data_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      SetParameterCallback set_parameter_callback = nullptr,
      std::span<const ParameterDefinition> parameter_definitions = {})
      : InstrumentDefinition(
            {create_callback, destroy_callback, process_callback,
             set_data_callback, set_note_off_callback, set_note_on_callback,
             set_parameter_callback, parameter_definitions.data(),
             static_cast<int>(parameter_definitions.size())}) {}

  /// Constructs new `InstrumentDefinition` from internal type.
  ///
  /// @param definition Internal instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  InstrumentDefinition(BarelyInstrumentDefinition definition)
      : BarelyInstrumentDefinition{definition} {
    assert(parameter_definitions || num_parameter_definitions == 0);
    assert(num_parameter_definitions >= 0);
  }
};

/// Instrument.
class Instrument {
 public:
  /// Note off callback signature.
  ///
  /// @param pitch Note pitch.
  using NoteOffCallback = std::function<void(double pitch)>;

  /// Note on callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  using NoteOnCallback = std::function<void(double pitch, double intensity)>;

  /// Destroys `Instrument`.
  ~Instrument() {
    BarelyInstrument_Destroy(std::exchange(handle_, nullptr),
                             std::exchange(id_, BarelyId_kInvalid));
  }

  /// Non-copyable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;

  /// Constructs new `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument(Instrument&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {
    SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
    SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      BarelyInstrument_Destroy(handle_, id_);
      handle_ = std::exchange(other.handle_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
      SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
      SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
    }
    return *this;
  }

  /// Returns parameter value.
  ///
  /// @param index Parameter index.
  /// @return Parameter value, or error status.
  template <typename IndexType, typename ValueType>
  [[nodiscard]] StatusOr<ValueType> GetParameter(IndexType index) const {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    double value = 0.0;
    if (const Status status = BarelyInstrument_GetParameter(
            handle_, id_, static_cast<int>(index), &value);
        !status.IsOk()) {
      return status;
    }
    return static_cast<ValueType>(value);
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const {
    bool is_note_on = false;
    [[maybe_unused]] const Status status =
        BarelyInstrument_IsNoteOn(handle_, id_, pitch, &is_note_on);
    assert(status.IsOk());
    return is_note_on;
  }

  /// Processes output buffer at timestamp.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Process(double* output, int num_output_channels, int num_output_frames,
                 double timestamp) {
    return BarelyInstrument_Process(handle_, id_, output, num_output_channels,
                                    num_output_frames, timestamp);
  }

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParameters() {
    return BarelyInstrument_ResetAllParameters(handle_, id_);
  }

  /// Resets parameter value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  template <typename IndexType>
  Status ResetParameter(IndexType index) {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyInstrument_ResetParameter(handle_, id_,
                                           static_cast<int>(index));
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  template <typename DataType>
  Status SetData(const DataType& data) {
    static_assert(std::is_trivially_copyable<DataType>::value,
                  "DataType is not trivially copyable");
    return BarelyInstrument_SetData(
        handle_, id_, static_cast<const void*>(&data), sizeof(decltype(data)));
  }

  /// Sets data.
  ///
  /// @param data Pointer to data.
  /// @param size Data size in bytes.
  /// @return Status.
  Status SetData(const void* data, int size) {
    return BarelyInstrument_SetData(handle_, id_, data, size);
  }

  /// Sets note off callback.
  ///
  /// @param callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback callback) {
    if (callback) {
      note_off_callback_ = std::move(callback);
      return BarelyInstrument_SetNoteOffCallback(
          handle_, id_,
          [](double pitch, void* user_data) {
            (*static_cast<NoteOffCallback*>(user_data))(pitch);
          },
          static_cast<void*>(&note_off_callback_));
    }
    return BarelyInstrument_SetNoteOffCallback(handle_, id_, nullptr, nullptr);
  }

  /// Sets note on callback.
  ///
  /// @param callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback callback) {
    if (callback) {
      note_on_callback_ = std::move(callback);
      return BarelyInstrument_SetNoteOnCallback(
          handle_, id_,
          [](double pitch, double intensity, void* user_data) {
            (*static_cast<NoteOnCallback*>(user_data))(pitch, intensity);
          },
          static_cast<void*>(&note_on_callback_));
    }
    return BarelyInstrument_SetNoteOnCallback(handle_, id_, nullptr, nullptr);
  }

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @param value Parameter slope in value change per second.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetParameter(IndexType index, ValueType value, double slope = 0.0) {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyInstrument_SetParameter(handle_, id_, static_cast<int>(index),
                                         static_cast<double>(value), slope);
  }

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(double pitch, double intensity = 1.0) {
    return BarelyInstrument_StartNote(handle_, id_, pitch, intensity);
  }

  /// Stops all notes.
  ///
  /// @return Status.
  Status StopAllNotes() { return BarelyInstrument_StopAllNotes(handle_, id_); }

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopNote(double pitch) {
    return BarelyInstrument_StopNote(handle_, id_, pitch);
  }

 private:
  friend class Musician;

  // Constructs new `Instrument`.
  explicit Instrument(BarelyMusicianHandle handle,
                      InstrumentDefinition definition, int frame_rate)
      : handle_(handle) {
    [[maybe_unused]] const Status status =
        BarelyInstrument_Create(handle_, definition, frame_rate, &id_);
    assert(status.IsOk());
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

/// Sequencer.
class Sequencer {
 public:
  /// Event callback signature.
  using EventCallback = std::function<void()>;

  /// Event reference.
  class EventReference {
   public:
    /// Returns position.
    ///
    /// @return Position in beats, or error status.
    [[nodiscard]] StatusOr<double> GetPosition() const {
      double position = 0.0;
      if (const Status status = BarelySequencer_GetEventPosition(
              handle_, sequencer_id_, id_, &position);
          !status.IsOk()) {
        return status;
      }
      return position;
    }

    /// Sets callback.
    ///
    /// @param callback Callback.
    /// @return Status.
    Status SetCallback(EventCallback callback) {
      *callback_wrapper_ptr_ = std::move(callback);
      return BarelySequencer_SetEventCallback(
          handle_, sequencer_id_, id_,
          [](void* user_data) { (*static_cast<EventCallback*>(user_data))(); },
          static_cast<void*>(callback_wrapper_ptr_));
    }

    /// Sets position.
    ///
    /// @param position Position in beats.
    /// @return Status.
    Status SetPosition(double position) {
      return BarelySequencer_SetEventPosition(handle_, sequencer_id_, id_,
                                              position);
    }

   private:
    friend class Sequencer;

    // Constructs new `EventReference`.
    explicit EventReference(BarelyMusicianHandle handle, BarelyId sequencer_id,
                            BarelyId id, EventCallback* callback_wrapper_ptr)
        : handle_(handle),
          sequencer_id_(sequencer_id),
          id_(id),
          callback_wrapper_ptr_(callback_wrapper_ptr) {}

    // Internal musician handle.
    BarelyMusicianHandle handle_ = nullptr;

    // Sequencer identifier.
    BarelyId sequencer_id_ = BarelyId_kInvalid;

    // Identifier.
    BarelyId id_ = BarelyId_kInvalid;

    // Pointer to callback wrapper.
    EventCallback* callback_wrapper_ptr_ = nullptr;
  };

  /// Destroys `Sequencer`.
  ~Sequencer() {
    BarelySequencer_Destroy(std::exchange(handle_, nullptr),
                            std::exchange(id_, BarelyId_kInvalid));
  }

  /// Non-copyable.
  Sequencer(const Sequencer& other) = delete;
  Sequencer& operator=(const Sequencer& other) = delete;

  /// Constructs new `Sequencer` via move.
  ///
  /// @param other Other sequencer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Sequencer(Sequencer&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {}

  /// Assigns `Sequencer` via move.
  ///
  /// @param other Other sequencer.
  Sequencer& operator=(Sequencer&& other) noexcept {
    if (this != &other) {
      BarelySequencer_Destroy(handle_, id_);
      handle_ = std::exchange(other.handle_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
    }
    return *this;
  }

  /// Adds event.
  ///
  /// @param position Event position in beats.
  /// @param callback Event callback.
  /// @return Event reference.
  EventReference AddEvent(double position, EventCallback callback) {
    auto event_callback_wrapper =
        std::make_unique<EventCallback>(std::move(callback));
    EventCallback* event_callback_wrapper_ptr = event_callback_wrapper.get();
    const auto [it, success] = event_callback_wrappers_.emplace(
        event_callback_wrapper_ptr, std::move(event_callback_wrapper));
    BarelyId event_id = BarelyId_kInvalid;
    [[maybe_unused]] const Status status = BarelySequencer_AddEvent(
        handle_, id_, position,
        [](void* user_data) { (*static_cast<EventCallback*>(user_data))(); },
        static_cast<void*>(event_callback_wrapper_ptr), &event_id);
    assert(status.IsOk());
    return EventReference(handle_, id_, event_id, event_callback_wrapper_ptr);
  }

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const Status status = BarelySequencer_GetLoopBeginPosition(
        handle_, id_, &loop_begin_position);
    assert(status.IsOk());
    return loop_begin_position;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const {
    double loop_length = 0.0;
    [[maybe_unused]] const Status status =
        BarelySequencer_GetLoopLength(handle_, id_, &loop_length);
    assert(status.IsOk());
    return loop_length;
  }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const {
    double position = 0.0;
    [[maybe_unused]] const Status status =
        BarelySequencer_GetPosition(handle_, id_, &position);
    assert(status.IsOk());
    return position;
  }

  /// Returns whether sequencer should be looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const {
    bool is_looping = false;
    [[maybe_unused]] const Status status =
        BarelySequencer_IsLooping(handle_, id_, &is_looping);
    assert(status.IsOk());
    return is_looping;
  }

  /// Returns whether sequencer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const {
    bool is_playing = false;
    [[maybe_unused]] const Status status =
        BarelySequencer_IsPlaying(handle_, id_, &is_playing);
    assert(status.IsOk());
    return is_playing;
  }

  /// Removes event.
  ///
  /// @param event_reference Event reference.
  /// @return Status.
  Status RemoveEvent(EventReference event_reference) {
    event_callback_wrappers_.erase(event_reference.callback_wrapper_ptr_);
    return BarelySequencer_RemoveEvent(handle_, id_, event_reference.id_);
  }

  /// Schedules one-off event at position.
  ///
  /// @param position Position in beats.
  /// @param callback Event callback.
  /// @return Status.
  Status ScheduleOneOffEvent(double position, EventCallback callback) {
    auto event_callback_wrapper = std::make_unique<EventCallback>();
    EventCallback* event_callback_wrapper_ptr = event_callback_wrapper.get();
    const auto [it, success] = event_callback_wrappers_.emplace(
        event_callback_wrapper_ptr, std::move(event_callback_wrapper));
    *it->second = [this, event_callback = std::move(callback),
                   event_callback_wrapper_ptr]() {
      event_callback();
      event_callback_wrappers_.erase(event_callback_wrapper_ptr);
    };
    return BarelySequencer_ScheduleOneOffEvent(
        handle_, id_, position,
        [](void* user_data) { (*static_cast<EventCallback*>(user_data))(); },
        static_cast<void*>(event_callback_wrapper_ptr));
  }

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  /// @return Status.
  Status SetLoopBeginPosition(double loop_begin_position) {
    return BarelySequencer_SetLoopBeginPosition(handle_, id_,
                                                loop_begin_position);
  }

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) {
    return BarelySequencer_SetLoopLength(handle_, id_, loop_length);
  }

  /// Sets whether sequence should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) {
    return BarelySequencer_SetLooping(handle_, id_, is_looping);
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return BarelySequencer_SetPosition(handle_, id_, position);
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() { return BarelySequencer_Start(handle_, id_); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() { return BarelySequencer_Stop(handle_, id_); }

 private:
  friend class Musician;

  // Constructs new `Sequencer`.
  explicit Sequencer(BarelyMusicianHandle handle, int priority)
      : handle_(handle) {
    [[maybe_unused]] const Status status =
        BarelySequencer_Create(handle_, priority, &id_);
    assert(status.IsOk());
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Map of event callback wrappers by their raw pointers.
  std::unordered_map<EventCallback*, std::unique_ptr<EventCallback>>
      event_callback_wrappers_;
};

/// Musician.
class Musician {
 public:
  /// Constructs new `Musician`.
  Musician() {
    [[maybe_unused]] const Status status = BarelyMusician_Create(&handle_);
    assert(status.IsOk());
  }

  /// Destroys `Musician`.
  ~Musician() {
    if (handle_) {
      [[maybe_unused]] const Status status =
          BarelyMusician_Destroy(std::exchange(handle_, nullptr));
      assert(status.IsOk());
    }
  }

  /// Non-copyable.
  Musician(const Musician& other) = delete;
  Musician& operator=(const Musician& other) = delete;

  /// Constructs new `Musician` via move.
  ///
  /// @param other Other musician.
  Musician(Musician&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  /// Assigns `Musician` via move.
  ///
  /// @param other Other musician.
  Musician& operator=(Musician&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        [[maybe_unused]] const Status status = BarelyMusician_Destroy(handle_);
        assert(status.IsOk());
      }
      handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
  }

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  /// @return Instrument.
  [[nodiscard]] Instrument CreateInstrument(InstrumentDefinition definition,
                                            int frame_rate) {
    return Instrument(handle_, definition, frame_rate);
  }

  /// Creates new sequencer.
  ///
  /// @param priority Sequencer priority for when executing events.
  /// @return Sequencer.
  [[nodiscard]] Sequencer CreateSequencer(int priority = 0) {
    return Sequencer(handle_, priority);
  }

  /// Returns beats from seconds.
  ///
  /// @param seconds Number of seconds.
  /// @return Number of beats.
  [[nodiscard]] double GetBeats(double seconds) const {
    double beats = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetBeats(handle_, seconds, &beats);
    assert(status.IsOk());
    return beats;
  }

  /// Returns seconds from beats.
  ///
  /// @param beats Number of beats.
  /// @return Number of seconds.
  [[nodiscard]] double GetSeconds(double beats) const {
    double seconds = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetSeconds(handle_, beats, &seconds);
    assert(status.IsOk());
    return seconds;
  }

  /// Returns tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetTempo() const {
    double tempo = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTempo(handle_, &tempo);
    assert(status.IsOk());
    return tempo;
  }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const {
    double timestamp = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTimestamp(handle_, &timestamp);
    assert(status.IsOk());
    return timestamp;
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return BarelyMusician_SetTempo(handle_, tempo);
  }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return BarelyMusician_Update(handle_, timestamp);
  }

 private:
  // Internal handle.
  BarelyMusicianHandle handle_ = nullptr;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
