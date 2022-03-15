#ifndef BARELYMUSICIAN_API_CONDUCTOR_H_
#define BARELYMUSICIAN_API_CONDUCTOR_H_

// NOLINTBEGIN
#include <stdbool.h>
#include <stdint.h>

#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Note pitch type enum alias.
typedef int32_t BarelyNotePitchType;

/// Note pitch type enum values.
enum BarelyNotePitchType_Values {
  /// Absolute pitch.
  BarelyNotePitchType_kAbsolutePitch = 0,
  /// Relative pitch with respect to root note.
  BarelyNotePitchType_kRelativePitch = 1,
  /// Scale index with respect to root note and scale.
  BarelyNotePitchType_kScaleIndex = 2,
};

/// Note definition pitch.
typedef struct BarelyNoteDefinition_Pitch {
  /// Type.
  BarelyNotePitchType type;

  /// Value.
  union {
    /// Absolute pitch.
    double absolute_pitch;
    /// Relative pitch.
    double relative_pitch;
    /// Scale index.
    int32_t scale_index;
  };
} BarelyNoteDefinition_Pitch;

/// Note definition.
typedef struct BarelyNoteDefinition {
  /// Duration.
  double duration;

  /// Intensity.
  double intensity;

  /// Pitch.
  BarelyNoteDefinition_Pitch pitch;
} BarelyNoteDefinition;

/// Parameter automation definition.
typedef struct BarelyParameterAutomationDefinition {
  /// Index.
  int32_t index;

  /// Value.
  double value;
} BarelyParameterAutomationDefinition;

/// Conductor alias.
typedef struct BarelyConductor* BarelyConductorHandle;

/// Conductor adjust note callback signature.
///
/// @param definition Mutable note definition.
/// @param user_data User data.
typedef void (*BarelyConductor_AdjustNoteCallback)(
    BarelyNoteDefinition* definition, void* user_data);

/// Conductor adjust parameter automation callback signature.
///
/// @param definition Mutable parameter automation definition.
/// @param user_data User data.
typedef void (*BarelyConductor_AdjustParameterAutomationCallback)(
    BarelyParameterAutomationDefinition* definition, void* user_data);

/// Conductor adjust tempo callback signature.
///
/// @param tempo Mutable tempo in bpm.
/// @param user_data User data.
typedef void (*BarelyConductor_AdjustTempoCallback)(double* tempo,
                                                    void* user_data);

/// Conductor beat callback signature.
///
/// @param position Beat position in beats.
/// @param timestamp Beat timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyConductor_BeatCallback)(double position, double timestamp,
                                             void* user_data);

/// Conductor update callback signature.
///
/// @param begin_position Begin position in beats.
/// @param end_position End position in beats.
/// @param begin_timestamp Begin timestamp in seconds.
/// @param end_timestamp End timestamp in seconds.
/// @param user_data User data.
typedef void (*BarelyConductor_UpdateCallback)(double begin_position,
                                               double end_position,
                                               double begin_timestamp,
                                               double end_timestamp,
                                               void* user_data);

/// Creates new conductor.
///
/// @param out_handle Output conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_Create(BarelyConductorHandle* out_handle);

/// Destroys conductor.
///
/// @param handle Conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_Destroy(BarelyConductorHandle handle);

/// Gets conductor note.
///
/// @param handle Conductor handle.
/// @param pitch Note pitch.
/// @param out_pitch Output note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_GetNote(BarelyConductorHandle handle,
                        BarelyNoteDefinition_Pitch pitch, double* out_pitch);

/// Gets conductor position.
///
/// @param handle Conductor handle.
/// @param out_position Output position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_GetPosition(BarelyConductorHandle handle, double* out_position);

/// Gets conductor root note.
///
/// @param handle Conductor handle.
/// @param out_root_pitch Output root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetRootNote(
    BarelyConductorHandle handle, double* out_root_pitch);

/// Gets conductor scale.
///
/// @param handle Conductor handle.
/// @param out_scale_pitches Output list of scale note pitches.
/// @param out_num_scale_pitches Output number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetScale(
    BarelyConductorHandle handle, double** out_scale_pitches,
    int32_t* out_num_scale_pitches);

/// Gets conductor tempo.
///
/// @param handle Conductor handle.
/// @param out_tempo Output tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_GetTempo(BarelyConductorHandle handle, double* out_tempo);

/// Gets conductor timestamp.
///
/// @param handle Conductor handle.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetTimestamp(
    BarelyConductorHandle handle, double* out_timestamp);

/// Gets conductor timestamp at position.
///
/// @param handle Conductor handle.
/// @param position Position in beats.
/// @param out_timestamp Output timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_GetTimestampAtPosition(
    BarelyConductorHandle handle, double position, double* out_timestamp);

/// Gets whether conductor is playing or not.
///
/// @param handle Conductor handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_IsPlaying(BarelyConductorHandle handle, bool* out_is_playing);

/// Sets conductor adjust note callback.
///
/// @param handle Conductor handle.
/// @param callback Adjust note callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetAdjustNoteCallback(
    BarelyConductorHandle handle, BarelyConductor_AdjustNoteCallback callback,
    void* user_data);

/// Sets conductor adjust parameter automation callback.
///
/// @param handle Conductor handle.
/// @param callback Adjust parameter automation callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetAdjustParameterAutomationCallback(
    BarelyConductorHandle handle,
    BarelyConductor_AdjustParameterAutomationCallback callback,
    void* user_data);

/// Sets conductor adjust tempo callback.
///
/// @param handle Conductor handle.
/// @param callback Adjust tempo callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetAdjustTempoCallback(
    BarelyConductorHandle handle, BarelyConductor_AdjustTempoCallback callback,
    void* user_data);

/// Sets conductor beat callback.
///
/// @param handle Conductor handle.
/// @param callback Beat callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetBeatCallback(
    BarelyConductorHandle handle, BarelyConductor_BeatCallback callback,
    void* user_data);

/// Sets conductor position.
///
/// @param handle Conductor handle.
/// @param position Position in beats.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetPosition(BarelyConductorHandle handle, double position);

/// Sets conductor root note.
///
/// @param handle Conductor handle.
/// @param root_pitch Root note pitch.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetRootNote(BarelyConductorHandle handle, double root_pitch);

/// Sets conductor scale.
///
/// @param handle Conductor handle.
/// @param scale_pitches List of scale note pitches.
/// @param num_scale_pitches Number of scale note pitches.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetScale(BarelyConductorHandle handle, double* scale_pitches,
                         int32_t num_scale_pitches);

/// Sets conductor tempo.
///
/// @param handle Conductor handle.
/// @param tempo Tempo in bpm.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetTempo(BarelyConductorHandle handle, double tempo);

/// Sets conductor timestamp.
///
/// @param handle Conductor handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus
BarelyConductor_SetTimestamp(BarelyConductorHandle handle, double timestamp);

/// Sets conductor update callback.
///
/// @param handle Conductor handle.
/// @param callback Update callback.
/// @param user_data User data.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_SetUpdateCallback(
    BarelyConductorHandle handle, BarelyConductor_UpdateCallback callback,
    void* user_data);

/// Starts conductor playback.
///
/// @param handle Conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_Start(BarelyConductorHandle handle);

/// Stops conductor playback.
///
/// @param handle Conductor handle.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_Stop(BarelyConductorHandle handle);

/// Updates conductor at timestamp.
///
/// @param handle Conductor handle.
/// @param timestamp Timestamp in seconds.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyConductor_Update(BarelyConductorHandle handle,
                                                  double timestamp);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <functional>

namespace barely {

/// Conductor.
class Conductor {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param timestamp Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Update callback signature.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @param begin_timestamp Begin timestamp in seconds.
  /// @param end_timestamp End timestamp in seconds.
  using UpdateCallback =
      std::function<void(double begin_position, double end_position,
                         double begin_timestamp, double end_timestamp)>;

  /// Constructs new `Conductor`.
  Conductor() {
    const auto status = BarelyConductor_Create(&handle_);
    assert(IsOk(static_cast<Status>(status)));
  }

  /// Destroys `Conductor`.
  ~Conductor() {
    if (handle_) {
      const auto status = BarelyConductor_Destroy(handle_);
      assert(IsOk(static_cast<Status>(status)));
      handle_ = nullptr;
    }
  }

  /// Non-copyable.
  Conductor(const Conductor& other) = delete;
  Conductor& operator=(const Conductor& other) = delete;

  /// Constructs new `Conductor` via move.
  ///
  /// @param other Other conductor.
  Conductor(Conductor&& other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {
    SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
    SetUpdateCallback(std::exchange(other.update_callback_, nullptr));
  }

  /// Assigns `Conductor` via move.
  ///
  /// @param other Other conductor.
  Conductor& operator=(Conductor&& other) noexcept {
    if (this != &other) {
      if (handle_) {
        const auto status = BarelyConductor_Destroy(handle_);
        assert(IsOk(static_cast<Status>(status)));
      }
      handle_ = std::exchange(other.handle_, nullptr);
      SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
      SetUpdateCallback(std::exchange(other.update_callback_, nullptr));
    }
    return *this;
  }

  /// Returns position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const {
    double position = 0.0;
    if (handle_) {
      const auto status = BarelyConductor_GetPosition(handle_, &position);
      assert(IsOk(static_cast<Status>(status)));
    }
    return position;
  }

  /// Returns tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetTempo() const {
    double tempo = 0.0;
    const auto status = BarelyConductor_GetTempo(handle_, &tempo);
    assert(IsOk(static_cast<Status>(status)));
    return tempo;
  }

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const {
    double timestamp = 0.0;
    const auto status = BarelyConductor_GetTimestamp(handle_, &timestamp);
    assert(IsOk(static_cast<Status>(status)));
    return timestamp;
  }

  /// Returns timestamp at position.
  ///
  /// @param position Position in beats.
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestampAtPosition(double position) const {
    double timestamp = 0.0;
    const auto status =
        BarelyConductor_GetTimestampAtPosition(handle_, position, &timestamp);
    assert(IsOk(static_cast<Status>(status)));
    return timestamp;
  }

  /// Returns whether conductor is playing or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const {
    bool is_playing = false;
    const auto status = BarelyConductor_IsPlaying(handle_, &is_playing);
    assert(status == BarelyStatus_kOk);
    return is_playing;
  }

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback callback) {
    if (callback) {
      beat_callback_ = std::move(callback);
      return static_cast<Status>(BarelyConductor_SetBeatCallback(
          handle_,
          [](double beat, double timestamp, void* user_data) {
            (*static_cast<BeatCallback*>(user_data))(beat, timestamp);
          },
          static_cast<void*>(&beat_callback_)));
    }
    return static_cast<Status>(
        BarelyConductor_SetBeatCallback(handle_, nullptr, nullptr));
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return static_cast<Status>(BarelyConductor_SetPosition(handle_, position));
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return static_cast<Status>(BarelyConductor_SetTempo(handle_, tempo));
  }

  /// Sets timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetTimestamp(double timestamp) {
    return static_cast<Status>(
        BarelyConductor_SetTimestamp(handle_, timestamp));
  }

  /// Sets update callback.
  ///
  /// @param callback Update callback.
  /// @return Status.
  Status SetUpdateCallback(UpdateCallback callback) {
    if (callback) {
      update_callback_ = std::move(callback);
      return static_cast<Status>(BarelyConductor_SetUpdateCallback(
          handle_,
          [](double begin_position, double end_position, double begin_timestamp,
             double end_timestamp, void* user_data) {
            (*static_cast<UpdateCallback*>(user_data))(
                begin_position, end_position, begin_timestamp, end_timestamp);
          },
          static_cast<void*>(&update_callback_)));
    }
    return static_cast<Status>(
        BarelyConductor_SetUpdateCallback(handle_, nullptr, nullptr));
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() { return static_cast<Status>(BarelyConductor_Start(handle_)); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() { return static_cast<Status>(BarelyConductor_Stop(handle_)); }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return static_cast<Status>(BarelyConductor_Update(handle_, timestamp));
  }

 private:
  friend class Performer;

  // Internal handle.
  BarelyConductorHandle handle_ = nullptr;

  // Beat callback.
  BeatCallback beat_callback_;

  // Update callback.
  UpdateCallback update_callback_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_CONDUCTOR_H_
