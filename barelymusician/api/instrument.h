#ifndef BARELYMUSICIAN_API_INSTRUMENT_H_
#define BARELYMUSICIAN_API_INSTRUMENT_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Data definition move callback signature.
///
/// @param other_data Other data to move.
/// @param out_data Output data.
typedef void (*BarelyDataDefinition_MoveCallback)(void* other_data,
                                                  void** out_data);

/// Data definition destroy callback signature.
///
/// @param data Data to destroy.
typedef void (*BarelyDataDefinition_DestroyCallback)(void* data);

/// Data definition.
typedef struct BarelyDataDefinition {
  /// Move callback.
  BarelyDataDefinition_MoveCallback move_callback;

  /// Destroy callback.
  BarelyDataDefinition_DestroyCallback destroy_callback;

  /// Data.
  void* data;
} BarelyDataDefinition;

/// Parameter definition.
typedef struct BarelyParameterDefinition {
  /// Default value.
  double default_value;

  /// Minimum value.
  double min_value;

  /// Maximum value.
  double max_value;
} BarelyParameterDefinition;

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
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    void** state, double* output, int32_t num_output_channels,
    int32_t num_output_frames);

/// Instrument set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Data.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(void** state,
                                                           void* data);

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

/// Instrument alias.
typedef struct BarelyInstrument* BarelyInstrumentHandle;

/// Instrument note off callback signature.
///
/// @param pitch Note pitch.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOffCallback)(double pitch, double timestamp,
                                                 void* user_data);

/// Instrument note on callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOnCallback)(double pitch, double intensity,
                                                double timestamp,
                                                void* user_data);

/// Creates new instrument.
///
/// @param definition Instrument definition.
/// @param frame_rate Frame rate in hz.
/// @param out_handle Output instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Create(BarelyInstrumentDefinition definition,
                        int32_t frame_rate, BarelyInstrumentHandle* out_handle);

/// Destroys instrument.
///
/// @param handle Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Destroy(BarelyInstrumentHandle handle);

/// Gets instrument parameter value.
///
/// @param handle Instrument handle.
/// @param index Parameter index.
/// @param out_value Output parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameter(
    BarelyInstrumentHandle handle, int32_t index, double* out_value);

/// Gets instrument parameter definition.
///
/// @param handle Instrument handle.
/// @param index Parameter index.
/// @param out_definition Output parameter definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_GetParameterDefinition(
    BarelyInstrumentHandle handle, int32_t index,
    BarelyParameterDefinition* out_definition);

/// Gets whether instrument note is active or not.
///
/// @param handle Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if active, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(
    BarelyInstrumentHandle handle, double pitch, bool* out_is_note_on);

/// Processes instrument output buffer at timestamp.
///
/// @param handle Instrument handle.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyInstrumentHandle handle, double* output, int32_t num_output_channels,
    int32_t num_output_frames, double timestamp);

/// Resets all instrument parameters to default value.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetAllParameters(
    BarelyInstrumentHandle handle, double timestamp);

/// Resets instrument parameter to default value.
///
/// @param handle Instrument handle.
/// @param index Parameter index.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetParameter(
    BarelyInstrumentHandle handle, int32_t index, double timestamp);

/// Sets instrument data.
///
/// @param handle Instrument handle.
/// @param definition Data definition.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetData(BarelyInstrumentHandle handle,
                         BarelyDataDefinition definition, double timestamp);

/// Sets instrument note off callback.
///
/// @param handle Instrument handle.
/// @param note_off_callback Note off callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data);

/// Sets instrument note on callback.
///
/// @param handle Instrument handle.
/// @param note_on_callback Note on callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyInstrumentHandle handle,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data);

/// Sets instrument parameter value.
///
/// @param handle Instrument handle.
/// @param index Parameter index.
/// @param value Parameter value.
/// @param slope Parameter slope in value change per second.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetParameter(BarelyInstrumentHandle handle, int32_t index,
                              double value, double slope, double timestamp);

/// Starts instrument note.
///
/// @param handle Instrument handle.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StartNote(BarelyInstrumentHandle handle, double pitch,
                           double intensity, double timestamp);

/// Stops all instrument notes.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StopAllNotes(BarelyInstrumentHandle handle, double timestamp);

/// Stops instrument note.
///
/// @param handle Instrument handle.
/// @param pitch Note pitch.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopNote(
    BarelyInstrumentHandle handle, double pitch, double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

namespace barely {

/// Data definition.
struct DataDefinition : public BarelyDataDefinition {
  /// Move callback signature.
  using MoveCallback = BarelyDataDefinition_MoveCallback;

  /// Destroy callback signature.
  using DestroyCallback = BarelyDataDefinition_DestroyCallback;

  /// Constructs new `DataDefinition` of type.
  ///
  /// @param typed_data Typed data.
  template <typename DataType>
  explicit DataDefinition(DataType typed_data)
      : BarelyDataDefinition{
            [](void* other_data, void** out_data) {
              *out_data = static_cast<void*>(
                  new DataType(std::move(*static_cast<DataType*>(other_data))));
            },
            [](void* data) { delete static_cast<DataType*>(data); },
            static_cast<void*>(&typed_data)} {}

  /// Constructs new `DataDefinition` from internal type.
  ///
  /// @param definition Internal data definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  DataDefinition(BarelyDataDefinition definition)
      : BarelyDataDefinition{definition} {}
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
      : BarelyParameterDefinition{default_value, min_value, max_value} {}

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
      : BarelyParameterDefinition{definition} {}
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
  InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetDataCallback set_data_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      SetParameterCallback set_parameter_callback = nullptr,
      const std::vector<ParameterDefinition>& parameter_definitions = {})
      : BarelyInstrumentDefinition{
            create_callback,
            destroy_callback,
            process_callback,
            set_data_callback,
            set_note_off_callback,
            set_note_on_callback,
            set_parameter_callback,
            parameter_definitions.data(),
            static_cast<int>(parameter_definitions.size())} {}

  /// Constructs new `InstrumentDefinition` from internal type.
  ///
  /// @param definition Internal instrument definition.
  // NOLINTNEXTLINE(google-explicit-constructor)
  InstrumentDefinition(BarelyInstrumentDefinition definition)
      : BarelyInstrumentDefinition{definition} {}
};

/// Instrument.
class Instrument {
 public:
  /// Note off callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Note timestamp in seconds.
  using NoteOffCallback = std::function<void(double pitch, double timestamp)>;

  /// Note on callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Note timestamp in seconds.
  using NoteOnCallback =
      std::function<void(double pitch, double intensity, double timestamp)>;

  /// Constructs new `Instrument`.
  Instrument(InstrumentDefinition definition, int frame_rate) {
    const auto status =
        BarelyInstrument_Create(definition, frame_rate, &handle_);
    assert(IsOk(static_cast<Status>(status)));
  }

  /// Destroys `Instrument`.
  ~Instrument() {
    if (handle_) {
      const auto status = BarelyInstrument_Destroy(handle_);
      assert(IsOk(static_cast<Status>(status)));
      handle_ = nullptr;
    }
  }

  /// Non-copyable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;

  /// Constructs new `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument(Instrument&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {
    SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
    SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        const auto status = BarelyInstrument_Destroy(handle_);
        assert(IsOk(static_cast<Status>(status)));
      }
      handle_ = std::exchange(other.handle_, nullptr);
      SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
      SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
    }
    return *this;
  }

  /// Returns parameter value.
  ///
  /// @param index Parameter index.
  /// @return Parameter value, or error status.
  [[nodiscard]] StatusOr<double> GetParameter(int index) const {
    double value = 0.0;
    if (const auto status = static_cast<Status>(
            BarelyInstrument_GetParameter(handle_, index, &value));
        !IsOk(status)) {
      return status;
    }
    return value;
  }

  /// Returns parameter definition.
  ///
  /// @param index Parameter index.
  /// @return Parameter definition, or error status.
  [[nodiscard]] StatusOr<ParameterDefinition> GetParameterDefinition(
      int index) const {
    BarelyParameterDefinition definition;
    if (const auto status =
            static_cast<Status>(BarelyInstrument_GetParameterDefinition(
                handle_, index, &definition));
        !IsOk(status)) {
      return status;
    }
    return ParameterDefinition(definition);
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const {
    bool is_note_on = false;
    const auto status = BarelyInstrument_IsNoteOn(handle_, pitch, &is_note_on);
    assert(IsOk(static_cast<Status>(status)));
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
                 double timestamp = 0.0) {
    return static_cast<Status>(BarelyInstrument_Process(
        handle_, output, num_output_channels, num_output_frames, timestamp));
  }

  /// Resets all parameters at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ResetAllParameters(double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_ResetAllParameters(handle_, timestamp));
  }

  /// Resets parameter value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ResetParameter(int index, double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_ResetParameter(handle_, index, timestamp));
  }

  /// Sets data at timestamp.
  ///
  /// @param data Data.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  template <typename DataType>
  Status SetData(DataType data, double timestamp = 0.0) {
    return static_cast<Status>(BarelyInstrument_SetData(
        handle_, DataDefinition(std::move(data)), timestamp));
  }

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback note_off_callback) {
    if (note_off_callback) {
      note_off_callback_ = std::move(note_off_callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOffCallback(
          handle_,
          [](double pitch, double timestamp, void* user_data) {
            (*static_cast<NoteOffCallback*>(user_data))(pitch, timestamp);
          },
          static_cast<void*>(&note_off_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOffCallback(handle_, nullptr, nullptr));
  }

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback note_on_callback) {
    if (note_on_callback) {
      note_on_callback_ = std::move(note_on_callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOnCallback(
          handle_,
          [](double pitch, double intensity, double timestamp,
             void* user_data) {
            (*static_cast<NoteOnCallback*>(user_data))(pitch, intensity,
                                                       timestamp);
          },
          static_cast<void*>(&note_on_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOnCallback(handle_, nullptr, nullptr));
  }

  /// Sets parameter value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @param slope Parameter slope in value change per second.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetParameter(int index, double value, double slope = 0.0,
                      double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_SetParameter(handle_, index, value, slope, timestamp));
  }

  /// Starts note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status StartNote(double pitch, double intensity = 1.0,
                   double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_StartNote(handle_, pitch, intensity, timestamp));
  }

  /// Stops all notes at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status StopAllNotes(double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_StopAllNotes(handle_, timestamp));
  }

  /// Stops note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status StopNote(double pitch, double timestamp = 0.0) {
    return static_cast<Status>(
        BarelyInstrument_StopNote(handle_, pitch, timestamp));
  }

 private:
  // Internal handle.
  BarelyInstrumentHandle handle_ = nullptr;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_INSTRUMENT_H_
