#ifndef BARELYMUSICIAN_API_INSTRUMENT_H_
#define BARELYMUSICIAN_API_INSTRUMENT_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/data.h"
#include "barelymusician/api/parameter.h"
#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Instrument state.
typedef struct BarelyInstrumentState {
  /// Sampling rate in hz.
  int32_t sample_rate;

  /// List of parameters.
  const BarelyParameterState* parameters;

  /// Number of parameters.
  int32_t num_parameters;

  /// Mutable user state.
  void* user_state;
} BarelyInstrumentState;

/// Instrument create callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_CreateCallback)(
    BarelyInstrumentState* state);

/// Instrument destroy callback signature.
///
/// @param state Pointer to instrument state.
typedef void (*BarelyInstrumentDefinition_DestroyCallback)(
    BarelyInstrumentState* state);

/// Instrument process callback signature.
///
/// @param state Pointer to instrument state.
/// @param output Output buffer.
/// @param num_output_channels Number of channels.
/// @param num_output_frames Number of frames.
typedef void (*BarelyInstrumentDefinition_ProcessCallback)(
    BarelyInstrumentState* state, float* output, int32_t num_output_channels,
    int32_t num_output_frames);

/// Instrument set data callback signature.
///
/// @param state Pointer to instrument state.
/// @param data Data.
typedef void (*BarelyInstrumentDefinition_SetDataCallback)(
    BarelyInstrumentState* state, void* data);

/// Instrument set note off callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
typedef void (*BarelyInstrumentDefinition_SetNoteOffCallback)(
    BarelyInstrumentState* state, float pitch);

/// Instrument set note on callback signature.
///
/// @param state Pointer to instrument state.
/// @param pitch Note pitch.
/// @param intensity Note intensity.
typedef void (*BarelyInstrumentDefinition_SetNoteOnCallback)(
    BarelyInstrumentState* state, float pitch, float intensity);

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

  /// List of parameter definitions.
  BarelyParameterDefinition* parameter_definitions;

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
typedef void (*BarelyInstrument_NoteOffCallback)(float pitch, double timestamp,
                                                 void* user_data);

/// Instrument note on callback signature.
///
/// @param pitch Note pitch.
/// @param intensity Note intensity.
/// @param timestamp Note timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyInstrument_NoteOnCallback)(float pitch, float intensity,
                                                double timestamp,
                                                void* user_data);

/// Creates new instrument.
///
/// @param definition Instrument definition.
/// @param sample_rate Sampling rate in hz.
/// @param out_handle Output instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Create(
    BarelyInstrumentDefinition definition, int32_t sample_rate,
    BarelyInstrumentHandle* out_handle);

/// Destroys instrument.
///
/// @param handle Instrument handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_Destroy(BarelyInstrumentHandle handle);

/// Gets instrument gain.
///
/// @param handle Instrument handle.
/// @param out_gain Output gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_GetGain(BarelyInstrumentHandle handle, double* out_gain);

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

/// Gets whether instrument is muted or not.
///
/// @param handle Instrument handle.
/// @param out_is_muted Output true if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_IsMuted(BarelyInstrumentHandle handle, bool* out_is_muted);

/// Gets whether instrument note is playing or not.
///
/// @param handle Instrument handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_IsNoteOn(
    BarelyInstrumentHandle handle, float pitch, bool* out_is_note_on);

/// Processes instrument output buffer at timestamp.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @param output Output buffer.
/// @param num_output_channels Number of output channels.
/// @param num_output_frames Number of output frames.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_Process(
    BarelyInstrumentHandle handle, double timestamp, float* output,
    int32_t num_output_channels, int32_t num_output_frames);

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
/// @param timestamp Timestamp in seconds.
/// @param index Parameter index.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_ResetParameter(
    BarelyInstrumentHandle handle, double timestamp, int32_t index);

/// Sets instrument data.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @param definition Data definition.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetData(BarelyInstrumentHandle handle, double timestamp,
                         BarelyDataDefinition definition);

/// Sets instrument gain.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @param gain Gain in amplitude.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetGain(
    BarelyInstrumentHandle handle, double timestamp, double gain);

/// Sets whether volume should be muted or not.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @param is_muted True if muted, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_SetMuted(
    BarelyInstrumentHandle handle, double timestamp, bool is_muted);

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
/// @param timestamp Timestamp in seconds.
/// @param index Parameter index.
/// @param value Parameter value.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_SetParameter(BarelyInstrumentHandle handle, double timestamp,
                              int32_t index, double value);

/// Starts instrument note.
///
/// @param handle Instrument handle.
/// @param timestamp Timestamp in seconds.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyInstrument_StartNote(BarelyInstrumentHandle handle, double timestamp,
                           float pitch, float intensity);

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
/// @param timestamp Timestamp in seconds.
/// @param pitch Note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyInstrument_StopNote(
    BarelyInstrumentHandle handle, double timestamp, float pitch);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace barely {

/// Instrument definition.
struct InstrumentDefinition : public BarelyInstrumentDefinition {
  /// Create function signature.
  using CreateCallback = BarelyInstrumentDefinition_CreateCallback;

  /// Destroy function signature.
  using DestroyCallback = BarelyInstrumentDefinition_DestroyCallback;

  /// Process function signature.
  using ProcessCallback = BarelyInstrumentDefinition_ProcessCallback;

  /// Set data function signature.
  using SetDataCallback = BarelyInstrumentDefinition_SetDataCallback;

  /// Set note off function signature
  using SetNoteOffCallback = BarelyInstrumentDefinition_SetNoteOffCallback;

  /// Set note on function signature.
  using SetNoteOnCallback = BarelyInstrumentDefinition_SetNoteOnCallback;

  /// Constructs new `InstrumentDefinition`.
  ///
  /// @param create_callback Create callback.
  /// @param destroy_callback Destroy callback.
  /// @param process_callback Process callback.
  /// @param set_data_callback Set data callback.
  /// @param set_note_off_callback Set note off callback.
  /// @param set_note_on_callback Set note on callback.x
  /// @param parameter_definitions List of parameter definitions.
  InstrumentDefinition(
      CreateCallback create_callback, DestroyCallback destroy_callback,
      ProcessCallback process_callback, SetDataCallback set_data_callback,
      SetNoteOffCallback set_note_off_callback,
      SetNoteOnCallback set_note_on_callback,
      std::vector<ParameterDefinition> parameter_definitions = {})
      : BarelyInstrumentDefinition{create_callback,       destroy_callback,
                                   process_callback,      set_data_callback,
                                   set_note_off_callback, set_note_on_callback},
        parameter_definitions_(std::move(parameter_definitions)) {
    this->parameter_definitions = parameter_definitions_.data();
    num_parameter_definitions = static_cast<int>(parameter_definitions_.size());
  }

  /// Constructs new `InstrumentDefinition` from internal type.
  ///
  /// @param definition Internal instrument definition.
  explicit InstrumentDefinition(BarelyInstrumentDefinition definition)
      : BarelyInstrumentDefinition(definition) {}

 private:
  /// List of parameter definitions.
  std::vector<ParameterDefinition> parameter_definitions_;
};

/// Instrument.
class Instrument {
 public:
  /// Note off callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Note timestamp in seconds.
  using NoteOffCallback = std::function<void(float pitch, double timestamp)>;

  /// Note on callback signature.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Note timestamp in seconds.
  using NoteOnCallback =
      std::function<void(float pitch, float intensity, double timestamp)>;

  /// Constructs new `Instrument`.
  Instrument(InstrumentDefinition definition, int sample_rate) {
    const auto status = static_cast<Status>(
        BarelyInstrument_Create(std::move(definition), sample_rate, &handle_));
    assert(IsOk(status));
  }

  /// Destroys `Instrument`.
  ~Instrument() {
    BarelyInstrument_Destroy(handle_);
    handle_ = nullptr;
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
      BarelyInstrument_Destroy(handle_);
      handle_ = std::exchange(other.handle_, nullptr);
      SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
      SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
    }
    return *this;
  }

  /// Returns gain.
  ///
  /// @return Gain in amplitude.
  [[nodiscard]] double GetGain() const {
    double gain = 0.0;
    const auto status = BarelyInstrument_GetGain(handle_, &gain);
    assert(status == BarelyStatus_kOk);
    return gain;
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
      return static_cast<Status>(status);
    }
    return ParameterDefinition(definition);
  }

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise.
  [[nodiscard]] bool IsMuted() const {
    bool is_muted = false;
    const auto status =
        static_cast<Status>(BarelyInstrument_IsMuted(handle_, &is_muted));
    assert(IsOk(status));
    return is_muted;
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const {
    bool is_note_on = false;
    const auto status = static_cast<Status>(
        BarelyInstrument_IsNoteOn(handle_, pitch, &is_note_on));
    assert(IsOk(status));
    return is_note_on;
  }

  /// Processes output buffer at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @return Status.
  Status Process(double timestamp, float* output, int num_output_channels,
                 int num_output_frames) {
    return static_cast<Status>(BarelyInstrument_Process(
        handle_, timestamp, output, num_output_channels, num_output_frames));
  }

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParameters(double timestamp) {
    return static_cast<Status>(
        BarelyInstrument_ResetAllParameters(handle_, timestamp));
  }

  /// Resets parameter value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status ResetParameter(double timestamp, int index) {
    return static_cast<Status>(
        BarelyInstrument_ResetParameter(handle_, timestamp, index));
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  template <typename DataType>
  Status SetData(double timestamp, DataType data) {
    return static_cast<Status>(BarelyInstrument_SetData(
        handle_,
        BarelyDataDefinition{
            [](void* other_data, void** out_data) {
              *out_data = reinterpret_cast<void*>(new DataType(
                  std::move(*reinterpret_cast<DataType*>(other_data))));
            },
            [](void* data) { delete reinterpret_cast<DataType*>(data); },
            reinterpret_cast<void*>(&data)}));
  }

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetGain(double timestamp, double gain) {
    return static_cast<Status>(
        BarelyInstrument_SetGain(handle_, timestamp, gain));
  }

  /// Sets whether instrument should be muted or not.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetMuted(double timestamp, bool is_muted) {
    return static_cast<Status>(
        BarelyInstrument_SetMuted(handle_, timestamp, is_muted));
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
          [](float pitch, double timestamp, void* user_data) {
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
          [](float pitch, float intensity, double timestamp, void* user_data) {
            (*static_cast<NoteOnCallback*>(user_data))(pitch, intensity,
                                                       timestamp);
          },
          static_cast<void*>(&note_on_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOnCallback(handle_, nullptr, nullptr));
  }

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParameter(double timestamp, int index, double value) {
    return static_cast<Status>(
        BarelyInstrument_SetParameter(handle_, timestamp, index, value));
  }

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(double timestamp, float pitch, float intensity = 1.0f) {
    return static_cast<Status>(
        BarelyInstrument_StartNote(handle_, timestamp, pitch, intensity));
  }

  /// Stops all notes.
  ///
  /// @return Status.
  Status StopAllNotes(double timestamp) {
    return static_cast<Status>(
        BarelyInstrument_StopAllNotes(handle_, timestamp));
  }

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopNote(double timestamp, float pitch) {
    return static_cast<Status>(
        BarelyInstrument_StopNote(handle_, timestamp, pitch));
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
