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
  /// Unimplemented error.
  BarelyStatus_kUnimplemented = 3,
  /// Internal error.
  BarelyStatus_kInternal = 4,
};

/// Task type enum alias.
typedef int32_t BarelyTaskType;

/// Task type enum values.
enum BarelyTask_Values {
  /// One-off task.
  BarelyTaskType_kOneOff = 0,
  /// Recurring task.
  BarelyTaskType_kRecurring = 1,
};

/// Instrument control event callback signature.
///
/// @param index Control index.
/// @param value Control value.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_ControlEventCallback)(int32_t index,
                                                      double value,
                                                      void* user_data);

/// Instrument note control event callback signature.
///
/// @param pitch Note pitch.
/// @param index Control index.
/// @param value Control value.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_NoteControlEventCallback)(double pitch,
                                                          int32_t index,
                                                          double value,
                                                          void* user_data);

/// Instrument note off event callback signature.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_NoteOffEventCallback)(double pitch,
                                                      void* user_data);

/// Instrument note on event callback signature.
///
/// @param pitch Note pitch.
/// @param user_data Pointer to user data.
typedef void (*BarelyInstrument_NoteOnEventCallback)(double pitch,
                                                     void* user_data);

/// Instrument definition create callback signature.
///
/// @param state Pointer to instrument state.
/// @param frame_rate Frame rate in hertz.
typedef void (*BarelyInstrumentDefinition_CreateCallback)(void** state,
                                                          int32_t frame_rate);

/// Instrument definition destroy callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyCallback)(void** state);

/// Instrument definition process callback signature.
///
/// @param state Pointer to instrument state.
/// @param output_samples Interleaved array of output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    void** state, double* output_samples, int32_t output_channel_count,
    int32_t output_frame_count);

/// Instrument definition set control callback signature.
///
/// @param state Pointer to instrument state.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_frame Control slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetControlCallback)(
    void** state, int32_t index, double value, double slope_per_frame);

/// Instrument definition set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Pointer to data.
/// @param size Data size in bytes.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state,
                                                           const void* data,
                                                           int32_t size);

/// Instrument definition set note control callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param index Note control index.
/// @param value Note control value.
/// @param slope_per_frame Note control slope in value change per frame.
typedef void (*BarelyInstrumentDefinition_SetNoteControlCallback)(
    void** state, double pitch, int32_t index, double value,
    double slope_per_frame);

/// Instrument definition set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(void** state,
                                                              double pitch);

/// Instrument definition set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(void** state,
                                                             double pitch);

/// Task definition create callback signature.
///
/// @param state Pointer to task state.
/// @param user_data Pointer to user data.
typedef void (*BarelyTaskDefinition_CreateCallback)(void** state,
                                                    void* user_data);

/// Task definition destroy callback signature.
///
/// @param state Pointer to task state.
typedef void (*BarelyTaskDefinition_DestroyCallback)(void** state);

/// Task definition process callback signature.
///
/// @param state Pointer to task state.
typedef void (*BarelyTaskDefinition_ProcessCallback)(void** state);

/// Control definition.
typedef struct BarelyControlDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyControlDefinition;

/// Instrument definition.
typedef struct BarelyInstrumentDefinition {
  /// Create callback.
  BarelyInstrumentDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyInstrumentDefinition_ProcessCallback process_callback;

  /// Set control callback.
  BarelyInstrumentDefinition_SetControlCallback set_control_callback;

  /// Set data callback.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback;

  /// Set note control callback.
  BarelyInstrumentDefinition_SetNoteControlCallback set_note_control_callback;

  /// Set note off callback.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback;

  /// Set note on callback.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback;

  /// List of control definitions.
  const BarelyControlDefinition* control_definitions;

  /// Number of control definitions.
  int32_t control_definition_count;

  /// List of note control definitions.
  const BarelyControlDefinition* note_control_definitions;

  /// Number of note control definitions.
  int32_t note_control_definition_count;
} BarelyInstrumentDefinition;

/// Task definition.
typedef struct BarelyTaskDefinition {
  /// Create callback.
  BarelyTaskDefinition_CreateCallback create_callback;

  /// Destroy callback.
  BarelyTaskDefinition_DestroyCallback destroy_callback;

  /// Process callback.
  BarelyTaskDefinition_ProcessCallback process_callback;
} BarelyTaskDefinition;

/// Creates new instrument.
///
/// @param handle Musician handle.
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hertz.
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

/// Gets instrument control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_GetControl(BarelyMusicianHandle handle, BarelyId instrument_id,
                            int32_t index, double* out_value);

/// Gets instrument note control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Control index.
/// @param out_value Output control value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index, double* out_value);

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

/// Processes instrument output samples at timestamp.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param output_samples Interleaved array of output samples.
/// @param output_channel_count Number of output channels.
/// @param output_frame_count Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyMusicianHandle handle, BarelyId instrument_id, double* output_samples,
    int32_t output_channel_count, int32_t output_frame_count, double timestamp);

/// Resets all instrument controls to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllControls(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Resets all instrument note controls to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllNoteControls(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Resets instrument control to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index);

/// Resets instrument note control to default value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Control index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index);

/// Sets all instrument notes off.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetAllNotesOff(
    BarelyMusicianHandle handle, BarelyId instrument_id);

/// Sets instrument control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetControl(BarelyMusicianHandle handle, BarelyId instrument_id,
                            int32_t index, double value, double slope_per_beat);

/// Sets instrument control event callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Control event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_ControlEventCallback callback, void* user_data);

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

/// Sets instrument note control value.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @param index Control index.
/// @param value Control value.
/// @param slope_per_beat Control slope in value change per beat.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControl(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch,
    int32_t index, double value, double slope_per_beat);

/// Sets instrument note control event callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note control event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteControlEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteControlEventCallback callback, void* user_data);

/// Sets instrument note off.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOff(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Sets instrument note off event callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note off event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffEventCallback callback, void* user_data);

/// Sets instrument note on.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOn(
    BarelyMusicianHandle handle, BarelyId instrument_id, double pitch);

/// Sets instrument note on event callback.
///
/// @param handle Musician handle.
/// @param instrument_id Instrument identifier.
/// @param callback Note on event callback.
/// @param user_data Pointer to user data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnEventCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnEventCallback callback, void* user_data);

/// Creates new musician.
///
/// @param out_handle Output musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_Create(BarelyMusicianHandle* out_handle);

/// Creates new musician task at position.
///
/// @param handle Musician handle.
/// @param definition Task definition.
/// @param timestamp Task timestamp in seconds.
/// @param user_data Pointer to user data.
/// @param out_task_id Output task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_CreateTask(
    BarelyMusicianHandle handle, BarelyTaskDefinition definition,
    double timestamp, void* user_data, BarelyId* out_task_id);

/// Destroys musician.
///
/// @param handle Musician handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle);

/// Destroys musician task.
///
/// @param handle Musician handle.
/// @param task_id Task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyMusician_DestroyTask(BarelyMusicianHandle handle, BarelyId task_id);

/// Gets task timestamp.
///
/// @param handle Musician handle.
/// @param task_id Task identifier.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_GetTaskTimestamp(
    BarelyMusicianHandle handle, BarelyId task_id, double* out_timestamp);

/// Gets musician tempo.
///
/// @param handle Musician handle.
/// @param out_tempo Output tempo in beats per minute.
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

/// Sets musician task timestamp.
///
/// @param handle Musician handle.
/// @param task_id Task identiier.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyMusician_SetTaskTimestamp(
    BarelyMusicianHandle handle, BarelyId task_id, double timestamp);

/// Sets musician tempo.
///
/// @param handle Musician handle.
/// @param tempo Tempo in beats per minute.
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

/// Creates new performer.
///
/// @param order Task execution order amongst other performers.
/// @param out_performer_id Output performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Create(BarelyMusicianHandle handle,
                                                  int32_t order,
                                                  BarelyId* out_performer_id);

/// Creates new performer task at position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param definition Task definition.
/// @param position Task position in beats.
/// @param type Task type.
/// @param user_data Pointer to user data.
/// @param out_task_id Output task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_CreateTask(
    BarelyMusicianHandle handle, BarelyId performer_id,
    BarelyTaskDefinition definition, double position, BarelyTaskType type,
    void* user_data, BarelyId* out_task_id);

/// Destroys performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Destroy(BarelyMusicianHandle handle,
                                                   BarelyId performer_id);

/// Destroys performer task.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_DestroyTask(
    BarelyMusicianHandle handle, BarelyId performer_id, BarelyId task_id);

/// Gets performer loop begin position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_loop_begin_position Output loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double* out_loop_begin_position);

/// Gets performer loop length.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_loop_length Output loop length.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyPerformer_GetLoopLength(BarelyMusicianHandle handle,
                              BarelyId performer_id, double* out_loop_length);

/// Gets performer position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, double* out_position);

/// Gets performer task position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_GetTaskPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, BarelyId task_id,
    double* out_position);

/// Gets whether performer is looping or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_is_looping Output true if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsLooping(
    BarelyMusicianHandle handle, BarelyId performer_id, bool* out_is_looping);

/// Gets whether performer is playing or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_IsPlaying(
    BarelyMusicianHandle handle, BarelyId performer_id, bool* out_is_playing);

/// Sets performer loop begin position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param loop_begin_position Loop begin position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopBeginPosition(
    BarelyMusicianHandle handle, BarelyId performer_id,
    double loop_begin_position);

/// Sets performer loop length.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param loop_length Loop length in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLoopLength(
    BarelyMusicianHandle handle, BarelyId performer_id, double loop_length);

/// Sets whether performer should be looping or not.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param is_looping True if looping, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetLooping(
    BarelyMusicianHandle handle, BarelyId performer_id, bool is_looping);

/// Sets performer position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, double position);

/// Sets performer task position.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @param task_id Task identifier.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_SetTaskPosition(
    BarelyMusicianHandle handle, BarelyId performer_id, BarelyId task_id,
    double position);

/// Starts performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Start(BarelyMusicianHandle handle,
                                                 BarelyId performer_id);

/// Stops performer.
///
/// @param handle Musician handle.
/// @param performer_id Performer identifier.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPerformer_Stop(BarelyMusicianHandle handle,
                                                BarelyId performer_id);

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
    /// Unimplemented error.
    kUnimplemented = BarelyStatus_kUnimplemented,
    /// Internal error.
    kInternal = BarelyStatus_kInternal,
  };

  /// Constructs new `Status`.
  ///
  /// @param status Status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(Enum status) noexcept : status_(status) {}

  /// Constructs new `Status` from internal type.
  ///
  /// @param status Internal status enum.
  // NOLINTNEXTLINE(google-explicit-constructor)
  Status(BarelyStatus status) noexcept : status_(static_cast<Enum>(status)) {}

  /// Returns enum value.
  ///
  /// @return Enum value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Enum() const noexcept { return status_; }

  /// Enum comparators.
  auto operator<=>(Enum status) const noexcept { return status_ <=> status; }

  /// Returns whether status is okay or not.
  ///
  /// @return True if okay, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept { return status_ == kOk; }

  /// Returns status string.
  ///
  /// @return Status string.
  [[nodiscard]] std::string ToString() const noexcept {
    switch (status_) {
      case kOk:
        return "Ok";
      case kInvalidArgument:
        return "Invalid argument error";
      case kNotFound:
        return "Not found error";
      case kUnimplemented:
        return "Unimplemented error";
      case kInternal:
        return "Internal error";
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
  StatusOr(Status error_status) noexcept : value_or_(error_status) {
    assert(!error_status.IsOk());
  }

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
  // NOLINTNEXTLINE(google-explicit-constructor)
  StatusOr(ValueType value) noexcept : value_or_(std::move(value)) {}

  /// Member access operators.
  const ValueType& operator*() const noexcept { return GetValue(); }
  const ValueType* operator->() const noexcept { return &GetValue(); }

  /// Returns contained error status.
  ///
  /// @return Error status.
  [[nodiscard]] Status GetErrorStatus() const noexcept {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Value.
  [[nodiscard]] const ValueType& GetValue() const noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Mutable value.
  [[nodiscard]] ValueType& GetValue() noexcept {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  [[nodiscard]] bool IsOk() const noexcept {
    return std::holds_alternative<ValueType>(value_or_);
  }

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// Control definition.
struct ControlDefinition : public BarelyControlDefinition {
  /// Constructs new `ControlDefinition`.
  ///
  /// @param default_value Default value.
  /// @param min_value Minimum value.
  /// @param max_value Maximum value.
  explicit ControlDefinition(
      double default_value,
      double min_value = std::numeric_limits<double>::lowest(),
      double max_value = std::numeric_limits<double>::max()) noexcept
      : ControlDefinition(BarelyControlDefinition{
            default_value,
            min_value,
            max_value,
        }) {}

  /// Constructs new `ControlDefinition` for a boolean value.
  ///
  /// @param default_value Default boolean value.
  explicit ControlDefinition(bool default_value) noexcept
      : ControlDefinition(static_cast<double>(default_value)) {}

  /// Constructs new `ControlDefinition` for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ControlDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max()) noexcept
      : ControlDefinition(static_cast<double>(default_value),
                          static_cast<double>(min_value),
                          static_cast<double>(max_value)) {}

  /// Constructs new `ControlDefinition` from internal type.
  ///
  /// @param definition Internal control definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  ControlDefinition(BarelyControlDefinition definition) noexcept
      : BarelyControlDefinition{definition} {
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

  /// Set control callback signature.
  using SetControlCallback = BarelyInstrumentDefinition_SetControlCallback;

  /// Set data callback signature.
  using SetDataCallback = BarelyInstrumentDefinition_SetDataCallback;

  /// Set note control callback signature
  using SetNoteControlCallback =
      BarelyInstrumentDefinition_SetNoteControlCallback;

  /// Set note off callback signature
  using SetNoteOffCallback = BarelyInstrumentDefinition_SetNoteOffCallback;

  /// Set note on callback signature.
  using SetNoteOnCallback = BarelyInstrumentDefinition_SetNoteOnCallback;

  /// Constructs new `InstrumentDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_control_callback Set control callback.
  /// @param set_data_callback Set data callback.
  /// @param set_note_off_callback Set note off callback.
  /// @param set_note_on_callback Set note on callback.
  /// @param control_definitions List of control definitions.
  /// @param note_control_definitions List of note control definitions.
  explicit InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetControlCallback set_control_callback,
      SetDataCallback set_data_callback,
      SetNoteControlCallback set_note_control_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      std::span<const ControlDefinition> control_definitions,
      std::span<const ControlDefinition> note_control_definitions) noexcept
      : InstrumentDefinition({
            create_callback,
            destroy_callback,
            process_callback,
            set_control_callback,
            set_data_callback,
            set_note_control_callback,
            set_note_off_callback,
            set_note_on_callback,
            control_definitions.data(),
            static_cast<int>(control_definitions.size()),
            note_control_definitions.data(),
            static_cast<int>(note_control_definitions.size()),
        }) {}

  /// Constructs new `InstrumentDefinition` from internal type.
  ///
  /// @param definition Internal instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  InstrumentDefinition(BarelyInstrumentDefinition definition) noexcept
      : BarelyInstrumentDefinition{definition} {
    assert(control_definitions || control_definition_count == 0);
    assert(control_definition_count >= 0);
    assert(note_control_definitions || note_control_definition_count == 0);
    assert(note_control_definition_count >= 0);
  }
};

/// Task callback.
using TaskCallback = std::function<void()>;

/// Task definition.
struct TaskDefinition : public BarelyTaskDefinition {
  /// Create callback signature.
  using CreateCallback = BarelyTaskDefinition_CreateCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyTaskDefinition_DestroyCallback;

  /// Process callback signature.
  using ProcessCallback = BarelyTaskDefinition_ProcessCallback;

  /// Constructs new `TaskDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  explicit TaskDefinition(CreateCallback create_callback,
                          DestroyCallback destroy_callback,
                          ProcessCallback process_callback) noexcept
      : TaskDefinition(BarelyTaskDefinition{
            create_callback,
            destroy_callback,
            process_callback,
        }) {}

  /// Constructs new `TaskDefinition` from internal type.
  ///
  /// @param definition Internal task definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  TaskDefinition(BarelyTaskDefinition definition) noexcept
      : BarelyTaskDefinition{definition} {}
};

/// Task type.
enum class TaskType : BarelyTaskType {
  /// One-off task.
  kOneOff = BarelyTaskType_kOneOff,
  /// Recurring task.
  kRecurring = BarelyTaskType_kRecurring,
};

/// Instrument.
class Instrument {
 public:
  /// Control event callback signature.
  ///
  /// @param index Control index.
  /// @param value Control value.
  using ControlEventCallback = std::function<void(int index, double value)>;

  /// Note control event callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @param value Control value.
  using NoteControlEventCallback =
      std::function<void(double pitch, int index, double value)>;

  /// Note off event callback signature.
  ///
  /// @param pitch Note pitch.
  using NoteOffEventCallback = std::function<void(double pitch)>;

  /// Note on event callback signature.
  ///
  /// @param pitch Note pitch.
  using NoteOnEventCallback = std::function<void(double pitch)>;

  /// Destroys `Instrument`.
  ~Instrument() noexcept {
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
    SetControlEventCallback(
        std::exchange(other.control_event_callback_, nullptr));
    SetNoteControlEventCallback(
        std::exchange(other.note_control_event_callback_, nullptr));
    SetNoteOffEventCallback(
        std::exchange(other.note_off_event_callback_, nullptr));
    SetNoteOnEventCallback(
        std::exchange(other.note_on_event_callback_, nullptr));
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      BarelyInstrument_Destroy(handle_, id_);
      handle_ = std::exchange(other.handle_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
      SetControlEventCallback(
          std::exchange(other.control_event_callback_, nullptr));
      SetNoteControlEventCallback(
          std::exchange(other.note_control_event_callback_, nullptr));
      SetNoteOffEventCallback(
          std::exchange(other.note_off_event_callback_, nullptr));
      SetNoteOnEventCallback(
          std::exchange(other.note_on_event_callback_, nullptr));
    }
    return *this;
  }

  /// Returns control value.
  ///
  /// @param index Control index.
  /// @return Control value, or error status.
  template <typename IndexType, typename ValueType>
  [[nodiscard]] StatusOr<ValueType> GetControl(IndexType index) const noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    double value = 0.0;
    if (const Status status = BarelyInstrument_GetControl(
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
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const Status status =
        BarelyInstrument_IsNoteOn(handle_, id_, pitch, &is_note_on);
    assert(status.IsOk());
    return is_note_on;
  }

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Process(double* output_samples, int output_channel_count,
                 int output_frame_count, double timestamp) noexcept {
    return BarelyInstrument_Process(handle_, id_, output_samples,
                                    output_channel_count, output_frame_count,
                                    timestamp);
  }

  /// Resets all controls.
  ///
  /// @return Status.
  Status ResetAllControls() noexcept {
    return BarelyInstrument_ResetAllControls(handle_, id_);
  }

  /// Resets all controls.
  ///
  /// @param pitch Note pitch
  /// @return Status.
  Status ResetAllNoteControls(double pitch) noexcept {
    return BarelyInstrument_ResetAllNoteControls(handle_, id_, pitch);
  }

  /// Resets control value.
  ///
  /// @param index Control index.
  /// @return Status.
  template <typename IndexType>
  Status ResetControl(IndexType index) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyInstrument_ResetControl(handle_, id_, static_cast<int>(index));
  }

  /// Resets note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Status.
  template <typename IndexType>
  Status ResetNoteControl(double pitch, IndexType index) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    return BarelyInstrument_ResetNoteControl(handle_, id_, pitch,
                                             static_cast<int>(index));
  }

  /// Sets all notes off.
  ///
  /// @return Status.
  Status SetAllNotesOff() noexcept {
    return BarelyInstrument_SetAllNotesOff(handle_, id_);
  }

  /// Sets control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetControl(IndexType index, ValueType value,
                    double slope_per_beat = 0.0) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyInstrument_SetControl(handle_, id_, static_cast<int>(index),
                                       static_cast<double>(value),
                                       slope_per_beat);
  }

  /// Sets control event callback.
  ///
  /// @param callback Control event callback.
  /// @return Status.
  Status SetControlEventCallback(ControlEventCallback callback) noexcept {
    if (callback) {
      control_event_callback_ = std::move(callback);
      return BarelyInstrument_SetControlEventCallback(
          handle_, id_,
          [](int32_t index, double value, void* user_data) {
            (*static_cast<ControlEventCallback*>(user_data))(index, value);
          },
          static_cast<void*>(&control_event_callback_));
    }
    return BarelyInstrument_SetControlEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  template <typename DataType>
  Status SetData(const DataType& data) noexcept {
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
  Status SetData(const void* data, int size) noexcept {
    return BarelyInstrument_SetData(handle_, id_, data, size);
  }

  /// Sets note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return Status.
  template <typename IndexType, typename ValueType>
  Status SetNoteControl(double pitch, IndexType index, ValueType value,
                        double slope_per_beat = 0.0) noexcept {
    static_assert(
        std::is_integral<IndexType>::value || std::is_enum<IndexType>::value,
        "IndexType is not supported");
    static_assert(
        std::is_arithmetic<ValueType>::value || std::is_enum<ValueType>::value,
        "ValueType is not supported");
    return BarelyInstrument_SetNoteControl(
        handle_, id_, pitch, static_cast<int>(index),
        static_cast<double>(value), slope_per_beat);
  }

  /// Sets note control event callback.
  ///
  /// @param callback Note control event callback.
  /// @return Status.
  Status SetNoteControlEventCallback(
      NoteControlEventCallback callback) noexcept {
    if (callback) {
      note_control_event_callback_ = std::move(callback);
      return BarelyInstrument_SetNoteControlEventCallback(
          handle_, id_,
          [](double pitch, int32_t index, double value, void* user_data) {
            (*static_cast<NoteControlEventCallback*>(user_data))(pitch, index,
                                                                 value);
          },
          static_cast<void*>(&note_control_event_callback_));
    }
    return BarelyInstrument_SetNoteControlEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(double pitch) noexcept {
    return BarelyInstrument_SetNoteOff(handle_, id_, pitch);
  }

  /// Sets note off event callback.
  ///
  /// @param callback Note off event callback.
  /// @return Status.
  Status SetNoteOffEventCallback(NoteOffEventCallback callback) noexcept {
    if (callback) {
      note_off_event_callback_ = std::move(callback);
      return BarelyInstrument_SetNoteOffEventCallback(
          handle_, id_,
          [](double pitch, void* user_data) {
            (*static_cast<NoteOffEventCallback*>(user_data))(pitch);
          },
          static_cast<void*>(&note_off_event_callback_));
    }
    return BarelyInstrument_SetNoteOffEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status SetNoteOn(double pitch) noexcept {
    return BarelyInstrument_SetNoteOn(handle_, id_, pitch);
  }

  /// Sets note on event callback.
  ///
  /// @param callback Note on event callback.
  /// @return Status.
  Status SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept {
    if (callback) {
      note_on_event_callback_ = std::move(callback);
      return BarelyInstrument_SetNoteOnEventCallback(
          handle_, id_,
          [](double pitch, void* user_data) {
            (*static_cast<NoteOnEventCallback*>(user_data))(pitch);
          },
          static_cast<void*>(&note_on_event_callback_));
    }
    return BarelyInstrument_SetNoteOnEventCallback(
        handle_, id_, /*callback=*/nullptr, /*user_data=*/nullptr);
  }

 private:
  friend class Musician;

  // Constructs new `Instrument`.
  explicit Instrument(BarelyMusicianHandle handle,
                      InstrumentDefinition definition, int frame_rate) noexcept
      : handle_(handle) {
    [[maybe_unused]] const Status status =
        BarelyInstrument_Create(handle_, definition, frame_rate, &id_);
    assert(status.IsOk());
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;

  // Control event callback.
  ControlEventCallback control_event_callback_;

  // Note control event callback.
  NoteControlEventCallback note_control_event_callback_;

  // Note off event callback.
  NoteOffEventCallback note_off_event_callback_;

  // Note on event callback.
  NoteOnEventCallback note_on_event_callback_;
};

/// Performer.
class Performer {
 public:
  /// Task reference.
  class TaskReference {
   public:
    /// Returns position.
    ///
    /// @return Position in beats, or error status.
    [[nodiscard]] StatusOr<double> GetPosition() const noexcept {
      double position = 0.0;
      if (const Status status = BarelyPerformer_GetTaskPosition(
              handle_, performer_id_, id_, &position);
          !status.IsOk()) {
        return status;
      }
      return position;
    }

    /// Sets position.
    ///
    /// @param position Position in beats.
    /// @return Status.
    Status SetPosition(double position) noexcept {
      return BarelyPerformer_SetTaskPosition(handle_, performer_id_, id_,
                                             position);
    }

   private:
    friend class Performer;

    // Constructs new `TaskReference`.
    explicit TaskReference(BarelyMusicianHandle handle, BarelyId performer_id,
                           BarelyId id) noexcept
        : handle_(handle), performer_id_(performer_id), id_(id) {}

    // Internal musician handle.
    BarelyMusicianHandle handle_ = nullptr;

    // Performer identifier.
    BarelyId performer_id_ = BarelyId_kInvalid;

    // Identifier.
    BarelyId id_ = BarelyId_kInvalid;
  };

  /// Destroys `Performer`.
  ~Performer() noexcept {
    BarelyPerformer_Destroy(std::exchange(handle_, nullptr),
                            std::exchange(id_, BarelyId_kInvalid));
  }

  /// Non-copyable.
  Performer(const Performer& other) = delete;
  Performer& operator=(const Performer& other) = delete;

  /// Constructs new `Performer` via move.
  ///
  /// @param other Other performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Performer(Performer&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {}

  /// Assigns `Performer` via move.
  ///
  /// @param other Other performer.
  Performer& operator=(Performer&& other) noexcept {
    if (this != &other) {
      BarelyPerformer_Destroy(handle_, id_);
      handle_ = std::exchange(other.handle_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
    }
    return *this;
  }

  /// Creates new task at position.
  ///
  /// @param definition Task definition.
  /// @param position Task position in beats.
  /// @param type Task type.
  /// @param user_data Pointer to user data.
  /// @return Task reference.
  TaskReference CreateTask(TaskDefinition definition, double position,
                           TaskType type = TaskType::kRecurring,
                           void* user_data = nullptr) noexcept {
    BarelyId task_id = BarelyId_kInvalid;
    [[maybe_unused]] const Status status = BarelyPerformer_CreateTask(
        handle_, id_, definition, position, static_cast<BarelyTaskType>(type),
        user_data, &task_id);
    assert(status.IsOk());
    return TaskReference(handle_, id_, task_id);
  }

  /// Creates new task with callback.
  ///
  /// @param callback Task callback.
  /// @param position Task position in beats.
  /// @param type Task type.
  /// @return Task reference.
  TaskReference CreateTask(TaskCallback callback, double position,
                           TaskType type = TaskType::kRecurring) noexcept {
    return CreateTask(
        TaskDefinition(
            [](void** state, void* user_data) {
              *state = new TaskCallback(
                  std::move(*static_cast<TaskCallback*>(user_data)));
            },
            [](void** state) { delete static_cast<TaskCallback*>(*state); },
            [](void** state) {
              if (const auto& callback = *static_cast<TaskCallback*>(*state);
                  callback) {
                callback();
              }
            }),
        position, type, static_cast<void*>(&callback));
  }

  /// Destroys task.
  ///
  /// @param task_ref Task reference.
  /// @return Status.
  Status DestroyTask(TaskReference task_ref) noexcept {
    return BarelyPerformer_DestroyTask(handle_, id_, task_ref.id_);
  }

  /// Returns loop begin position.
  ///
  /// @return Loop begin position in beats.
  [[nodiscard]] double GetLoopBeginPosition() const noexcept {
    double loop_begin_position = 0.0;
    [[maybe_unused]] const Status status = BarelyPerformer_GetLoopBeginPosition(
        handle_, id_, &loop_begin_position);
    assert(status.IsOk());
    return loop_begin_position;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const noexcept {
    double loop_length = 0.0;
    [[maybe_unused]] const Status status =
        BarelyPerformer_GetLoopLength(handle_, id_, &loop_length);
    assert(status.IsOk());
    return loop_length;
  }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const noexcept {
    double position = 0.0;
    [[maybe_unused]] const Status status =
        BarelyPerformer_GetPosition(handle_, id_, &position);
    assert(status.IsOk());
    return position;
  }

  /// Returns whether performer should be looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] bool IsLooping() const noexcept {
    bool is_looping = false;
    [[maybe_unused]] const Status status =
        BarelyPerformer_IsLooping(handle_, id_, &is_looping);
    assert(status.IsOk());
    return is_looping;
  }

  /// Returns whether performer is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const Status status =
        BarelyPerformer_IsPlaying(handle_, id_, &is_playing);
    assert(status.IsOk());
    return is_playing;
  }

  /// Sets loop begin position.
  ///
  /// @param loop_begin_position Loop begin position in beats.
  /// @return Status.
  Status SetLoopBeginPosition(double loop_begin_position) noexcept {
    return BarelyPerformer_SetLoopBeginPosition(handle_, id_,
                                                loop_begin_position);
  }

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) noexcept {
    return BarelyPerformer_SetLoopLength(handle_, id_, loop_length);
  }

  /// Sets whether performer should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) noexcept {
    return BarelyPerformer_SetLooping(handle_, id_, is_looping);
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) noexcept {
    return BarelyPerformer_SetPosition(handle_, id_, position);
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() noexcept { return BarelyPerformer_Start(handle_, id_); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() noexcept { return BarelyPerformer_Stop(handle_, id_); }

 private:
  friend class Musician;

  // Constructs new `Performer`.
  explicit Performer(BarelyMusicianHandle handle, int priority) noexcept
      : handle_(handle) {
    [[maybe_unused]] const Status status =
        BarelyPerformer_Create(handle_, priority, &id_);
    assert(status.IsOk());
  }

  // Internal musician handle.
  BarelyMusicianHandle handle_ = nullptr;

  // Identifier.
  BarelyId id_ = BarelyId_kInvalid;
};

/// Musician.
class Musician {
 public:
  /// Task reference.
  class TaskReference {
   public:
    /// Returns timestamp.
    ///
    /// @return Timestamp in seconds, or error status.
    [[nodiscard]] StatusOr<double> GetTimestamp() const noexcept {
      double timestamp = 0.0;
      if (const Status status =
              BarelyMusician_GetTaskTimestamp(handle_, id_, &timestamp);
          !status.IsOk()) {
        return status;
      }
      return timestamp;
    }

    /// Sets timestamp.
    ///
    /// @param timestamp Timestamp in seconds.
    /// @return Status.
    Status SetTimestamp(double timestamp) noexcept {
      return BarelyMusician_SetTaskTimestamp(handle_, id_, timestamp);
    }

   private:
    friend class Musician;

    // Constructs new `TaskReference`.
    explicit TaskReference(BarelyMusicianHandle handle, BarelyId id) noexcept
        : handle_(handle), id_(id) {}

    // Internal musician handle.
    BarelyMusicianHandle handle_ = nullptr;

    // Identifier.
    BarelyId id_ = BarelyId_kInvalid;
  };

  /// Constructs new `Musician`.
  Musician() noexcept {
    [[maybe_unused]] const Status status = BarelyMusician_Create(&handle_);
    assert(status.IsOk());
  }

  /// Destroys `Musician`.
  ~Musician() noexcept {
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
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument.
  [[nodiscard]] Instrument CreateInstrument(InstrumentDefinition definition,
                                            int frame_rate) noexcept {
    return Instrument(handle_, definition, frame_rate);
  }

  /// Creates new performer.
  ///
  /// @param order Task execution order amongst other performers.
  /// @return Performer.
  [[nodiscard]] Performer CreatePerformer(int order = 0) noexcept {
    return Performer(handle_, order);
  }

  /// Creates new task at timestamp.
  ///
  /// @param definition Task definition.
  /// @param timestamp Task timestamp in seconds.
  /// @param user_data Pointer to user data.
  /// @return Task reference.
  TaskReference CreateTask(TaskDefinition definition, double timestamp,
                           void* user_data = nullptr) noexcept {
    BarelyId task_id = BarelyId_kInvalid;
    [[maybe_unused]] const Status status = BarelyMusician_CreateTask(
        handle_, definition, timestamp, user_data, &task_id);
    assert(status.IsOk());
    return TaskReference(handle_, task_id);
  }

  /// Creates new task with callback.
  ///
  /// @param callback Task callback.
  /// @param timestamp Task timestamp in seconds.
  /// @return Task reference.
  TaskReference CreateTask(TaskCallback callback, double timestamp) noexcept {
    return CreateTask(
        TaskDefinition(
            [](void** state, void* user_data) {
              *state = new TaskCallback(
                  std::move(*static_cast<TaskCallback*>(user_data)));
            },
            [](void** state) { delete static_cast<TaskCallback*>(*state); },
            [](void** state) {
              if (const auto& callback = *static_cast<TaskCallback*>(*state);
                  callback) {
                callback();
              }
            }),
        timestamp, static_cast<void*>(&callback));
  }

  /// Destroys task.
  ///
  /// @param task_ref Task reference.
  /// @return Status.
  Status DestroyTask(TaskReference task_ref) noexcept {
    return BarelyMusician_DestroyTask(handle_, task_ref.id_);
  }

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept {
    double tempo = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTempo(handle_, &tempo);
    assert(status.IsOk());
    return tempo;
  }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept {
    double timestamp = 0.0;
    [[maybe_unused]] const Status status =
        BarelyMusician_GetTimestamp(handle_, &timestamp);
    assert(status.IsOk());
    return timestamp;
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  /// @return Status.
  Status SetTempo(double tempo) noexcept {
    return BarelyMusician_SetTempo(handle_, tempo);
  }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) noexcept {
    return BarelyMusician_Update(handle_, timestamp);
  }

 private:
  // Internal handle.
  BarelyMusicianHandle handle_ = nullptr;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYMUSICIAN_H_
