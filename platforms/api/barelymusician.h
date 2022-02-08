#ifndef BARELYMUSICIAN_API_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_API_BARELYMUSICIAN_H_

#include <cassert>
#include <functional>
#include <limits>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Status.
enum class Status : BarelyStatus {
  /// Success.
  kOk = BarelyStatus_kOk,
  /// Invalid argument error.
  kInvalidArgument = BarelyStatus_kInvalidArgument,
  /// Not found error.
  kNotFound = BarelyStatus_kNotFound,
  /// Already exists error.
  kAlreadyExists = BarelyStatus_kAlreadyExists,
  /// Failed precondition error.
  kFailedPrecondition = BarelyStatus_kFailedPrecondition,
  /// Unimplemented error.
  kUnimplemented = BarelyStatus_kUnimplemented,
  /// Internal error.
  kInternal = BarelyStatus_kInternal,
  /// Unknown error.
  kUnknown = BarelyStatus_kUnknown,
};

/// Returns whether status is okay or not.
///
/// @param status Status.
/// @return True if okay, false otherwise.
inline bool IsOk(Status status) { return status == Status::kOk; }

/// Returns corresponding status string.
///
/// @param Status.
/// @return String.
inline std::string ToString(Status status) {
  switch (status) {
    case Status::kOk:
      return "Ok";
    case Status::kInvalidArgument:
      return "Invalid argument error";
    case Status::kNotFound:
      return "Not found error";
    case Status::kAlreadyExists:
      return "Already exists error";
    case Status::kFailedPrecondition:
      return "Failed precondition error";
    case Status::kUnimplemented:
      return "Unimplemented error";
    case Status::kInternal:
      return "Internal error";
    case Status::kUnknown:
    default:
      return "Unknown error";
  }
}

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  StatusOr(Status error_status) : value_or_(error_status) {
    assert(error_status != Status::kOk);
  }

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
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

/// Note pitch type.
enum class NotePitchType : BarelyNotePitchType {
  /// Absolute pitch.
  kAbsolutePitch = BarelyNotePitchType_kAbsolutePitch,
  /// Relative pitch with respect to conductor root note.
  kRelativePitch = BarelyNotePitchType_kRelativePitch,
  /// Scale index with respect to conductor root note and scale.
  kScaleIndex = BarelyNotePitchType_kScaleIndex,
};

/// Note definition.
struct NoteDefinition {
  /// Constructs new `NoteDefinition`.
  ///
  /// @param duration Note duration.
  /// @param pitch_type Note pitch type.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param bypass_adjustment True to bypass conductor adjustment.
  NoteDefinition(double duration, NotePitchType pitch_type, float pitch,
                 float intensity = 1.0f, bool bypass_adjustment = false)
      : duration(duration),
        pitch_type(pitch_type),
        pitch(pitch),
        intensity(intensity),
        bypass_adjustment(bypass_adjustment) {}

  /// Constructs new `NoteDefinition` with absolute pitch.
  ///
  /// @param duration Note duration.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param bypass_adjustment True to bypass conductor adjustment.
  NoteDefinition(double duration, float pitch, float intensity = 1.0f,
                 bool bypass_adjustment = false)
      : NoteDefinition(duration, NotePitchType::kAbsolutePitch, pitch,
                       intensity, bypass_adjustment) {}

  /// Duration.
  double duration;

  /// Pitch type.
  NotePitchType pitch_type;

  /// Pitch value.
  float pitch;

  /// Intensity.
  float intensity;

  /// Denotes whether conductor adjust should be bypassed or not.
  bool bypass_adjustment;
};

/// Parameter definition.
struct ParameterDefinition {
  /// Constructs new `ParameterDefinition` for a float value.
  ///
  /// @param default_value Default float value.
  /// @param min_value Minimum float value.
  /// @param max_value Maximum float value.
  explicit ParameterDefinition(
      float default_value,
      float min_value = std::numeric_limits<float>::lowest(),
      float max_value = std::numeric_limits<float>::max())
      : default_value(default_value),
        min_value(min_value),
        max_value(max_value) {}

  /// Constructs new `ParameterDefinition` for a boolean value.
  ///
  /// @param default_value Default boolean value.
  explicit ParameterDefinition(bool default_value)
      : ParameterDefinition(static_cast<float>(default_value)) {}

  /// Constructs new `ParameterDefinition` for an integer value.
  ///
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  explicit ParameterDefinition(
      int default_value, int min_value = std::numeric_limits<int>::lowest(),
      int max_value = std::numeric_limits<int>::max())
      : ParameterDefinition(static_cast<float>(default_value),
                            static_cast<float>(min_value),
                            static_cast<float>(max_value)) {}

  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
};

/// Conductor definition.
struct ConductorDefinition {
  /// Conductor adjust note duration function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param duration Pointer to note duration.
  using AdjustNoteDurationFn = void (*)(void** state, double* duration);

  /// Conductor adjust note intensity function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param intensity Pointer to note intensity.
  using AdjustNoteIntensityFn = void (*)(void** state, float* intensity);

  /// Conductor adjust note pitch function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param pitch_type Pointer to note pitch type.
  /// @param pitch Pointer to note pitch.
  using AdjustNotePitchFn = void (*)(void** state,
                                     BarelyNotePitchType* pitch_type,
                                     float* pitch);

  /// Conductor adjust tempo function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param tempo Tempo in bpm.
  using AdjustTempoFn = void (*)(void** state, double* tempo);

  /// Create function signature.
  ///
  /// @param state Pointer to conductor state.
  using CreateFn = void (*)(void** state);

  /// Destroy function signature.
  ///
  /// @param state Pointer to conductor state.
  using DestroyFn = void (*)(void** state);

  /// Set data function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param data Data.
  using SetDataFn = void (*)(void** state, void* data);

  /// Set energy function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param energy Energy.
  using SetEnergyFn = void (*)(void** state, float energy);

  /// Set parameter function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param index Parameter index.
  /// @param value Parameter value.
  using SetParameterFn = void (*)(void** state, int index, float value);

  /// Set stress function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param stress Stress.
  using SetStressFn = void (*)(void** state, float stress);

  /// Adjust note duration function.
  AdjustNoteDurationFn adjust_note_duration_fn;

  /// Adjust note intensity function.
  AdjustNoteIntensityFn adjust_note_intensity_fn;

  /// Adjust note pitch function.
  AdjustNotePitchFn adjust_note_pitch_fn;

  /// Adjust tempo function.
  AdjustTempoFn adjust_tempo_fn;

  /// Create function.
  CreateFn create_fn;

  /// Destroy function.
  DestroyFn destroy_fn;

  /// Set data function.
  SetDataFn set_data_fn;

  /// Set energy function.
  SetEnergyFn set_energy_fn;

  /// Set parameter function.
  SetParameterFn set_parameter_fn;

  /// Set stress function.
  SetStressFn set_stress_fn;

  /// List of parameter definitions.
  std::vector<ParameterDefinition> parameter_definitions;
};

/// Conductor.
class Conductor {
 public:
  /// Conducts note.
  ///
  /// @param pitch_type Note pitch type.
  /// @param pitch Note pitch.
  /// @param bypass_adjustment True to bypass conductor adjustment.
  /// @return Conducted note pitch.
  float ConductNote(NotePitchType pitch_type, float pitch,
                    bool bypass_adjustment = false) {
    float conducted_pitch = pitch;
    if (capi_) {
      const auto status = BarelyConductor_ConductNote(
          capi_, static_cast<BarelyNotePitchType>(pitch_type), pitch,
          bypass_adjustment, &conducted_pitch);
      assert(status == BarelyStatus_kOk);
    }
    return conducted_pitch;
  }

  /// Returns energy.
  ///
  /// @return Energy.
  [[nodiscard]] float GetEnergy() const {
    float energy = 0.0f;
    if (capi_) {
      const auto status = BarelyConductor_GetEnergy(capi_, &energy);
      assert(status == BarelyStatus_kOk);
    }
    return energy;
  }

  /// Returns parameter value.
  ///
  /// @param index Parameter index.
  /// @return Parameter value, or error status.
  [[nodiscard]] StatusOr<float> GetParameter(int index) const {
    float value = 0.0f;
    if (const auto status = BarelyConductor_GetParameter(capi_, index, &value);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
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
            BarelyConductor_GetParameterDefinition(capi_, index, &definition);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return ParameterDefinition(definition.default_value, definition.min_value,
                               definition.max_value);
  }

  /// Returns root note.
  ///
  /// @return Root note pitch.
  [[nodiscard]] float GetRootNote() const {
    float root_pitch = 0.0f;
    if (capi_) {
      const auto status = BarelyConductor_GetRootNote(capi_, &root_pitch);
      assert(status == BarelyStatus_kOk);
    }
    return root_pitch;
  }

  /// Returns scale.
  ///
  /// @return List of scale note pitches.
  [[nodiscard]] std::vector<float> GetScale() const {
    float* scale_pitches = nullptr;
    int num_scale_pitches = 0;
    if (capi_) {
      const auto status =
          BarelyConductor_GetScale(capi_, &scale_pitches, &num_scale_pitches);
      assert(status == BarelyStatus_kOk);
    }
    return std::vector<float>{scale_pitches, scale_pitches + num_scale_pitches};
  }

  /// Returns stress.
  ///
  /// @return Stress.
  [[nodiscard]] float GetStress() const {
    float stress = 0.0f;
    if (capi_) {
      const auto status = BarelyConductor_GetStress(capi_, &stress);
      assert(status == BarelyStatus_kOk);
    }
    return stress;
  }

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParameters() {
    return static_cast<Status>(BarelyConductor_ResetAllParameters(capi_));
  }

  /// Resets parameter value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status ResetParameter(int index) {
    return static_cast<Status>(BarelyConductor_ResetParameter(capi_, index));
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  Status SetData(void* data) {
    return static_cast<Status>(BarelyConductor_SetData(capi_, data));
  }

  /// Sets definition.
  ///
  /// @param definition Conductor definition.
  /// @return Status.
  Status SetDefinition(const ConductorDefinition& definition) {
    std::vector<BarelyParameterDefinition> parameter_definitions;
    parameter_definitions.reserve(definition.parameter_definitions.size());
    for (const auto& parameter_definition : definition.parameter_definitions) {
      parameter_definitions.push_back(BarelyParameterDefinition{
          parameter_definition.default_value, parameter_definition.min_value,
          parameter_definition.max_value});
    }
    return static_cast<Status>(BarelyConductor_SetDefinition(
        capi_,
        BarelyConductorDefinition{
            definition.adjust_note_duration_fn,
            definition.adjust_note_intensity_fn,
            definition.adjust_note_pitch_fn, definition.adjust_tempo_fn,
            definition.create_fn, definition.destroy_fn, definition.set_data_fn,
            definition.set_energy_fn, definition.set_parameter_fn,
            definition.set_stress_fn, parameter_definitions.data(),
            static_cast<int>(parameter_definitions.size())}));
  }

  /// Sets energy.
  ///
  /// @param energy Energy.
  /// @return Status.
  Status SetEnergy(float energy) {
    return static_cast<Status>(BarelyConductor_SetEnergy(capi_, energy));
  }

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParameter(int index, float value) {
    return static_cast<Status>(
        BarelyConductor_SetParameter(capi_, index, value));
  }

  /// Sets root note.
  ///
  /// @param root_pitch Root note pitch.
  /// @return Status.
  Status SetRootNote(float root_pitch) {
    return static_cast<Status>(BarelyConductor_SetRootNote(capi_, root_pitch));
  }

  /// Sets scale.
  ///
  /// @param scale_pitches List of scale note pitches.
  /// @return Status.
  Status SetScale(std::vector<float> scale_pitches) {
    return static_cast<Status>(BarelyConductor_SetScale(
        capi_, scale_pitches.data(), static_cast<int>(scale_pitches.size())));
  }

  /// Sets stress.
  ///
  /// @param stress Stress.
  /// @return Status.
  Status SetStress(float stress) {
    return static_cast<Status>(BarelyConductor_SetStress(capi_, stress));
  }

 private:
  friend class Musician;

  // Constructs new `Conductor` with internal api handle.
  explicit Conductor(BarelyApi capi) : capi_(capi) {}

  // Default destructor.
  ~Conductor() = default;

  // Non-copyable.
  Conductor(const Conductor& other) = delete;
  Conductor& operator=(const Conductor& other) = delete;

  // Constructs new `Conductor` via move.
  Conductor(Conductor&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)) {}

  // Assigns `Conductor` via move.
  Conductor& operator=(Conductor&& other) noexcept {
    if (this != &other) {
      capi_ = std::exchange(other.capi_, nullptr);
    }
    return *this;
  }

  // Internal api handle.
  BarelyApi capi_;
};

/// Instrument definition.
struct InstrumentDefinition {
  /// Create function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param sample_rate Sampling rate in hz.
  using CreateFn = void (*)(void** state, int sample_rate);

  /// Destroy function signature.
  ///
  /// @param state Pointer to instrument state.
  using DestroyFn = void (*)(void** state);

  /// Process function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param output Output buffer.
  /// @param num_output_channels Number of channels.
  /// @param num_output_frames Number of frames.
  using ProcessFn = void (*)(void** state, float* output,
                             int num_output_channels, int num_output_frames);

  /// Set data function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param data Data.
  using SetDataFn = void (*)(void** state, void* data);

  /// Set note off function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param pitch Note pitch.
  using SetNoteOffFn = void (*)(void** state, float pitch);

  /// Set note on function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  using SetNoteOnFn = void (*)(void** state, float pitch, float intensity);

  /// Set parameter function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param index Parameter index.
  /// @param value Parameter value.
  using SetParameterFn = void (*)(void** state, int index, float value);

  /// Create function.
  CreateFn create_fn;

  /// Destroy function.
  DestroyFn destroy_fn;

  /// Process function.
  ProcessFn process_fn;

  /// Set data function.
  SetDataFn set_data_fn;

  /// Set note off function.
  SetNoteOffFn set_note_off_fn;

  /// Set note on function.
  SetNoteOnFn set_note_on_fn;

  /// Set parameter function.
  SetParameterFn set_parameter_fn;

  /// List of parameter definitions.
  std::vector<ParameterDefinition> parameter_definitions;
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

  /// Destroys `Instrument`.
  ~Instrument() {
    if (id_ != BarelyId_kInvalid) {
      BarelyInstrument_Destroy(capi_, id_);
      id_ = BarelyId_kInvalid;
    }
  }

  /// Non-copyable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;

  /// Constructs new `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument(Instrument&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {
    SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
    SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
  }

  /// Assigns `Instrument` via move.
  ///
  /// @param other Other instrument.
  Instrument& operator=(Instrument&& other) noexcept {
    if (this != &other) {
      if (id_ != BarelyId_kInvalid) {
        BarelyInstrument_Destroy(capi_, id_);
      }
      capi_ = std::exchange(other.capi_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
      SetNoteOffCallback(std::exchange(other.note_off_callback_, nullptr));
      SetNoteOnCallback(std::exchange(other.note_on_callback_, nullptr));
    }
    return *this;
  }

  /// Returns gain.
  ///
  /// @return Gain in amplitude.
  [[nodiscard]] float GetGain() const {
    float gain = 0.0f;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelyInstrument_GetGain(capi_, id_, &gain);
      assert(status == BarelyStatus_kOk);
    }
    return gain;
  }

  /// Returns parameter value.
  ///
  /// @param index Parameter index.
  /// @return Parameter value, or error status.
  [[nodiscard]] StatusOr<float> GetParameter(int index) const {
    float value = 0.0f;
    if (const auto status =
            BarelyInstrument_GetParameter(capi_, id_, index, &value);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
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
    if (const auto status = BarelyInstrument_GetParameterDefinition(
            capi_, id_, index, &definition);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return ParameterDefinition(definition.default_value, definition.min_value,
                               definition.max_value);
  }

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise.
  [[nodiscard]] bool IsMuted() const {
    bool is_muted = false;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelyInstrument_IsMuted(capi_, id_, &is_muted);
      assert(status == BarelyStatus_kOk);
    }
    return is_muted;
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const {
    bool is_note_on = false;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelyInstrument_IsNoteOn(capi_, id_, pitch, &is_note_on);
      assert(status == BarelyStatus_kOk);
    }
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
        capi_, id_, timestamp, output, num_output_channels, num_output_frames));
  }

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParameters() {
    return static_cast<Status>(BarelyInstrument_ResetAllParameters(capi_, id_));
  }

  /// Resets parameter value.
  ///
  /// @param index Parameter index.
  /// @return Status.
  Status ResetParameter(int index) {
    return static_cast<Status>(
        BarelyInstrument_ResetParameter(capi_, id_, index));
  }

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  Status SetData(void* data) {
    return static_cast<Status>(BarelyInstrument_SetData(capi_, id_, data));
  }

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetGain(float gain) {
    return static_cast<Status>(BarelyInstrument_SetGain(capi_, id_, gain));
  }

  /// Sets whether instrument should be muted or not.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetMuted(bool is_muted) {
    return static_cast<Status>(BarelyInstrument_SetMuted(capi_, id_, is_muted));
  }

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback note_off_callback) {
    if (note_off_callback) {
      note_off_callback_ = std::move(note_off_callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOffCallback(
          capi_, id_,
          [](float pitch, double timestamp, void* user_data) {
            (*static_cast<NoteOffCallback*>(user_data))(pitch, timestamp);
          },
          static_cast<void*>(&note_off_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOffCallback(capi_, id_, nullptr, nullptr));
  }

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback note_on_callback) {
    if (note_on_callback) {
      note_on_callback_ = std::move(note_on_callback);
      return static_cast<Status>(BarelyInstrument_SetNoteOnCallback(
          capi_, id_,
          [](float pitch, float intensity, double timestamp, void* user_data) {
            (*static_cast<NoteOnCallback*>(user_data))(pitch, intensity,
                                                       timestamp);
          },
          static_cast<void*>(&note_on_callback_)));
    }
    return static_cast<Status>(
        BarelyInstrument_SetNoteOnCallback(capi_, id_, nullptr, nullptr));
  }

  /// Sets parameter value.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParameter(int index, float value) {
    return static_cast<Status>(
        BarelyInstrument_SetParameter(capi_, id_, index, value));
  }

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(float pitch, float intensity = 1.0f) {
    return static_cast<Status>(
        BarelyInstrument_StartNote(capi_, id_, pitch, intensity));
  }

  /// Stops all notes.
  ///
  /// @return Status.
  Status StopAllNotes() {
    return static_cast<Status>(BarelyInstrument_StopAllNotes(capi_, id_));
  }

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopNote(float pitch) {
    return static_cast<Status>(BarelyInstrument_StopNote(capi_, id_, pitch));
  }

 private:
  friend class Musician;
  friend class Sequence;

  // Constructs new `Instrument` with internal api handle and definition.
  Instrument(BarelyApi capi, const InstrumentDefinition& definition,
             int sample_rate)
      : capi_(capi),
        id_(BarelyId_kInvalid),
        note_off_callback_(nullptr),
        note_on_callback_(nullptr) {
    if (capi_) {
      std::vector<BarelyParameterDefinition> parameter_definitions;
      parameter_definitions.reserve(definition.parameter_definitions.size());
      for (const auto& parameter_definition :
           definition.parameter_definitions) {
        parameter_definitions.push_back(BarelyParameterDefinition{
            parameter_definition.default_value, parameter_definition.min_value,
            parameter_definition.max_value});
      }
      const auto status = BarelyInstrument_Create(
          capi_,
          BarelyInstrumentDefinition{
              definition.create_fn, definition.destroy_fn,
              definition.process_fn, definition.set_data_fn,
              definition.set_note_off_fn, definition.set_note_on_fn,
              definition.set_parameter_fn, parameter_definitions.data(),
              static_cast<int>(parameter_definitions.size())},
          sample_rate, &id_);
      assert(status == BarelyStatus_kOk);
    }
  }

  // Internal api handle.
  BarelyApi capi_;

  // Identifier.
  BarelyId id_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

/// Note reference.
class NoteReference {
 public:
  // Default destructor.
  ~NoteReference() = default;

  // Copyable.
  NoteReference(const NoteReference& other) = default;
  NoteReference& operator=(const NoteReference& other) = default;

  // Constructs new `NoteReference` via move.
  NoteReference(NoteReference&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)),
        sequence_id_(std::exchange(other.sequence_id_, BarelyId_kInvalid)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)) {}

  // Assigns `NoteReference` via move.
  NoteReference& operator=(NoteReference&& other) noexcept {
    if (this != &other) {
      capi_ = std::exchange(other.capi_, nullptr);
      sequence_id_ = std::exchange(other.sequence_id_, BarelyId_kInvalid);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
    }
    return *this;
  }

  /// Returns note definition.
  ///
  /// @return Note definition.
  [[nodiscard]] NoteDefinition GetNoteDefinition() const {
    BarelyNoteDefinition definition;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelySequence_GetNoteDefinition(capi_, sequence_id_,
                                                           id_, &definition);
      assert(status == BarelyStatus_kOk);
    }
    return {definition.duration,
            static_cast<NotePitchType>(definition.pitch_type), definition.pitch,
            definition.intensity, definition.bypass_adjustment};
  }

  /// Returns note position.
  ///
  /// @return Note position.
  [[nodiscard]] double GetNotePosition() const {
    double position = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetNotePosition(capi_, sequence_id_, id_, &position);
      assert(status == BarelyStatus_kOk);
    }
    return position;
  }

  /// Sets note definition.
  ///
  /// @param definition Note definition.
  /// @retun Status.
  Status SetNoteDefinition(NoteDefinition definition) {
    return static_cast<Status>(BarelySequence_SetNoteDefinition(
        capi_, sequence_id_, id_,
        BarelyNoteDefinition{
            definition.duration,
            static_cast<BarelyNotePitchType>(definition.pitch_type),
            definition.pitch, definition.intensity,
            definition.bypass_adjustment}));
  }

  /// Sets note position.
  ///
  /// @param position Note position in beats.
  /// @retun Status.
  Status SetNotePosition(double position) {
    return static_cast<Status>(
        BarelySequence_SetNotePosition(capi_, sequence_id_, id_, position));
  }

 private:
  friend class Sequence;

  // Creates new `NoteReference` with internal api and identifiers.
  NoteReference(BarelyApi capi, BarelyId sequence_id, BarelyId id)
      : capi_(capi), sequence_id_(sequence_id), id_(id) {}

  // Internal api handle.
  BarelyApi capi_;

  // Sequence identifier.
  BarelyId sequence_id_;

  // Identifier.
  BarelyId id_;
};

/// Note sequence.
class Sequence {
 public:
  /// Destroys `Sequence`.
  ~Sequence() {
    if (id_ != BarelyId_kInvalid) {
      BarelySequence_Destroy(capi_, id_);
      id_ = BarelyId_kInvalid;
    }
  }

  /// Non-copyable.
  Sequence(const Sequence& other) = delete;
  Sequence& operator=(const Sequence& other) = delete;

  /// Constructs new `Sequence` via move.
  ///
  /// @param other Other sequence.
  Sequence(Sequence&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)),
        id_(std::exchange(other.id_, BarelyId_kInvalid)),
        instrument_(std::exchange(other.instrument_, nullptr)) {}

  /// Assigns `Sequence` via move.
  ///
  /// @param other Other sequence.
  Sequence& operator=(Sequence&& other) noexcept {
    if (this != &other) {
      if (id_ != BarelyId_kInvalid) {
        BarelySequence_Destroy(capi_, id_);
      }
      capi_ = std::exchange(other.capi_, nullptr);
      id_ = std::exchange(other.id_, BarelyId_kInvalid);
      instrument_ = std::exchange(other.instrument_, nullptr);
    }
    return *this;
  }

  /// Adds note at position.
  ///
  /// @param position Note position.
  /// @param definition Note definition.
  /// @return Note reference.
  NoteReference AddNote(double position, NoteDefinition definition) {
    BarelyId note_id = BarelyId_kInvalid;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelySequence_AddNote(
          capi_, id_, position,
          BarelyNoteDefinition{
              definition.duration,
              static_cast<BarelyNotePitchType>(definition.pitch_type),
              definition.pitch, definition.intensity,
              definition.bypass_adjustment},
          &note_id);
      assert(status == BarelyStatus_kOk);
    }
    return {capi_, id_, note_id};
  }

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats.
  [[nodiscard]] double GetBeginOffset() const {
    double begin_offset = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetBeginOffset(capi_, id_, &begin_offset);
      assert(status == BarelyStatus_kOk);
    }
    return begin_offset;
  }

  /// Returns begin position.
  ///
  /// @return Begin position in beats.
  [[nodiscard]] StatusOr<double> GetBeginPosition() const {
    double begin_position = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetBeginPosition(capi_, id_, &begin_position);
      assert(status == BarelyStatus_kOk);
    }
    return begin_position;
  }

  /// Returns end position.
  ///
  /// @return End position in beats.
  [[nodiscard]] StatusOr<double> GetEndPosition() const {
    double end_position = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetEndPosition(capi_, id_, &end_position);
      assert(status == BarelyStatus_kOk);
    }
    return end_position;
  }

  /// Returns instrument.
  ///
  /// @return Pointer to instrument, or nullptr.
  [[nodiscard]] const Instrument* GetInstrument() const { return instrument_; }

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats.
  [[nodiscard]] double GetLoopBeginOffset() const {
    double loop_begin_offset = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetLoopBeginOffset(capi_, id_, &loop_begin_offset);
      assert(status == BarelyStatus_kOk);
    }
    return loop_begin_offset;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats.
  [[nodiscard]] double GetLoopLength() const {
    double loop_length = 0.0;
    if (id_ != BarelyId_kInvalid) {
      const auto status =
          BarelySequence_GetLoopLength(capi_, id_, &loop_length);
      assert(status == BarelyStatus_kOk);
    }
    return loop_length;
  }

  /// Returns whether sequence is empty or not.
  ///
  /// @return True if empty, false otherwise.
  [[nodiscard]] StatusOr<bool> IsEmpty() const {
    bool is_empty = false;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelySequence_IsEmpty(capi_, id_, &is_empty);
      assert(status == BarelyStatus_kOk);
    }
    return is_empty;
  }

  /// Returns whether sequence should be looping or not.
  ///
  /// @return True if looping, false otherwise.
  [[nodiscard]] StatusOr<bool> IsLooping() const {
    bool is_looping = false;
    if (id_ != BarelyId_kInvalid) {
      const auto status = BarelySequence_IsLooping(capi_, id_, &is_looping);
      assert(status == BarelyStatus_kOk);
    }
    return is_looping;
  }

  /// Removes all notes.
  ///
  /// @return Status.
  Status RemoveAllNotes() {
    return static_cast<Status>(BarelySequence_RemoveAllNotes(capi_, id_));
  }

  /// Removes all notes at position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status RemoveAllNotes(double position) {
    return static_cast<Status>(
        BarelySequence_RemoveAllNotesAtPosition(capi_, id_, position));
  }

  /// Removes all notes at range.
  ///
  /// @param begin_position Begin position in beats.
  /// @param end_position End position in beats.
  /// @return Status.
  Status RemoveAllNotes(double begin_position, double end_position) {
    return static_cast<Status>(BarelySequence_RemoveAllNotesAtRange(
        capi_, id_, begin_position, end_position));
  }

  /// Removes note.
  ///
  /// @param note Note reference.
  /// @return Status.
  Status RemoveNote(NoteReference note) {
    return static_cast<Status>(BarelySequence_RemoveNote(capi_, id_, note.id_));
  }

  /// Sets begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetBeginOffset(double begin_offset) {
    return static_cast<Status>(
        BarelySequence_SetBeginOffset(capi_, id_, begin_offset));
  }

  /// Sets begin position.
  ///
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetBeginPosition(double begin_position) {
    return static_cast<Status>(
        BarelySequence_SetBeginPosition(capi_, id_, begin_position));
  }

  /// Sets end position.
  ///
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetEndPosition(double end_position) {
    return static_cast<Status>(
        BarelySequence_SetEndPosition(capi_, id_, end_position));
  }

  /// Sets instrument.
  ///
  /// @param instrument Pointer to instrument, or nullptr.
  /// @return Status.
  Status SetInstrument(const Instrument* instrument) {
    instrument_ = instrument;
    return static_cast<Status>(BarelySequence_SetInstrument(
        capi_, id_,
        instrument_ ? instrument_->id_
                    : static_cast<BarelyId>(BarelyId_kInvalid)));
  }

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetLoopBeginOffset(double loop_begin_offset) {
    return static_cast<Status>(
        BarelySequence_SetLoopBeginOffset(capi_, id_, loop_begin_offset));
  }

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length) {
    return static_cast<Status>(
        BarelySequence_SetLoopLength(capi_, id_, loop_length));
  }

  /// Sets whether sequence should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping) {
    return static_cast<Status>(
        BarelySequence_SetLooping(capi_, id_, is_looping));
  }

 private:
  friend class Musician;

  /// Constructs new `Sequence` with internal api handle and instrument.
  Sequence(BarelyApi capi, const Instrument* instrument)
      : capi_(capi), id_(BarelyId_kInvalid), instrument_(instrument) {
    if (capi_) {
      auto status = BarelySequence_Create(capi_, &id_);
      assert(status == BarelyStatus_kOk);
      if (instrument_) {
        status = BarelySequence_SetInstrument(capi_, id_, instrument_->id_);
        assert(status == BarelyStatus_kOk);
      }
    }
  }

  // Internal api handle.
  BarelyApi capi_;

  // Identifier.
  BarelyId id_;

  // Instrument.
  const Instrument* instrument_;
};

/// BarelyMusician C++ api.
class Musician {
 public:
  /// Beat callback signature.
  ///
  /// @param position Beat position in beats.
  /// @param timestamp Beat timestamp in seconds.
  using BeatCallback = std::function<void(double position, double timestamp)>;

  /// Constructs new `Musician`.
  Musician()
      : beat_callback_(nullptr), capi_(CreateCapi()), conductor_(capi_) {}

  /// Destroys `Musician`.
  ~Musician() {
    if (capi_) {
      const auto status = BarelyMusician_Destroy(capi_);
      assert(status == BarelyStatus_kOk);
      capi_ = nullptr;
    }
  }

  /// Non-copyable.
  Musician(const Musician& other) = delete;
  Musician& operator=(const Musician& other) = delete;

  /// Constructs new `Musician` via move.
  ///
  /// @param other Other api.
  Musician(Musician&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)),
        conductor_(std::move(other.conductor_)) {
    SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
  }

  /// Assigns `Musician` via move.
  ///
  /// @param other Other api.
  Musician& operator=(Musician&& other) noexcept {
    if (this != &other) {
      conductor_ = std::move(other.conductor_);
      if (capi_) {
        const auto status = BarelyMusician_Destroy(capi_);
        assert(status == BarelyStatus_kOk);
      }
      capi_ = std::exchange(other.capi_, nullptr);
      SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
    }
    return *this;
  }

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in hz.
  /// @return Instrument.
  Instrument CreateInstrument(const InstrumentDefinition& definition,
                              int sample_rate) {
    return Instrument{capi_, definition, sample_rate};
  }

  /// Creates new sequence.
  ///
  /// @param instrument Pointer to instrument.
  /// @return Sequence.
  Sequence CreateSequence(const Instrument* instrument = nullptr) {
    return Sequence{capi_, instrument};
  }

  /// Returns conductor.
  ///
  /// @return Conductor.
  [[nodiscard]] const Conductor& GetConductor() const { return conductor_; }

  /// Returns conductor.
  ///
  /// @return Mutable conductor.
  [[nodiscard]] Conductor& GetConductor() { return conductor_; }

  /// Returns playback position.
  ///
  /// @return Position in beats.
  [[nodiscard]] double GetPosition() const {
    double position = 0.0;
    if (capi_) {
      const auto status = BarelyMusician_GetPosition(capi_, &position);
      assert(status == BarelyStatus_kOk);
    }
    return position;
  }

  /// Returns playback tempo.
  ///
  /// @return Tempo in bpm.
  [[nodiscard]] double GetTempo() const {
    double tempo = 0.0;
    if (capi_) {
      const auto status = BarelyMusician_GetTempo(capi_, &tempo);
      assert(status == BarelyStatus_kOk);
    }
    return tempo;
  }

  /// Returns whether playback is active or not.
  ///
  /// @return True if playing, false otherwise.
  [[nodiscard]] bool IsPlaying() const {
    bool is_playing = false;
    if (capi_) {
      const auto status = BarelyMusician_IsPlaying(capi_, &is_playing);
      assert(status == BarelyStatus_kOk);
    }
    return is_playing;
  }

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback beat_callback) {
    if (beat_callback) {
      beat_callback_ = std::move(beat_callback);
      return static_cast<Status>(BarelyMusician_SetBeatCallback(
          capi_,
          [](double position, double timestamp, void* user_data) {
            (*reinterpret_cast<BeatCallback*>(user_data))(position, timestamp);
          },
          reinterpret_cast<void*>(&beat_callback_)));
    }
    return static_cast<Status>(BarelyMusician_SetBeatCallback(
        capi_, /*beat_callback=*/nullptr, /*user_data=*/nullptr));
  }

  /// Sets playback position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return static_cast<Status>(BarelyMusician_SetPosition(capi_, position));
  }

  /// Sets playback tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return static_cast<Status>(BarelyMusician_SetTempo(capi_, tempo));
  }

  /// Starts playback.
  ///
  /// @return Status.
  Status Start() { return static_cast<Status>(BarelyMusician_Start(capi_)); }

  /// Stops playback.
  ///
  /// @return Status.
  Status Stop() { return static_cast<Status>(BarelyMusician_Stop(capi_)); }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return static_cast<Status>(BarelyMusician_Update(capi_, timestamp));
  }

 private:
  // Creates new internal api and returns corresponding handle.
  static BarelyApi CreateCapi() {
    BarelyApi capi = nullptr;
    const auto status = BarelyMusician_Create(&capi);
    assert(status == BarelyStatus_kOk);
    return capi;
  }

  // Beat callback.
  BeatCallback beat_callback_;

  // Internal api handle.
  BarelyApi capi_;

  // Conductor.
  Conductor conductor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_API_BARELYMUSICIAN_H_
