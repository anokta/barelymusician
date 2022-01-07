#ifndef PLATFORMS_API_BARELYMUSICIAN_H_
#define PLATFORMS_API_BARELYMUSICIAN_H_

#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>
#include <variant>
#include <vector>

#include "platforms/capi/barelymusician_v2.h"

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
  Status GetErrorStatus() const {
    assert(std::holds_alternative<Status>(value_or_));
    return std::get<Status>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Value.
  const ValueType& GetValue() const {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns contained value.
  ///
  /// @return Mutable value.
  ValueType& GetValue() {
    assert(std::holds_alternative<ValueType>(value_or_));
    return std::get<ValueType>(value_or_);
  }

  /// Returns whether value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  bool IsOk() const { return std::holds_alternative<ValueType>(value_or_); }

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// Parameter identifier type.
using ParamId = std::int32_t;

/// Parameter definition.
struct ParamDefinition {
  /// Constructs new `ParamDefinition`.
  ///
  /// @param id Identifier.
  explicit ParamDefinition(ParamId id) : ParamDefinition(id, 0.0f) {}

  /// Constructs new `ParamDefinition` for a float value.
  ///
  /// @param id Identifier.
  /// @param default_value Default float value.
  /// @param min_value Minimum float value.
  /// @param max_value Maximum float value.
  ParamDefinition(ParamId id, float default_value,
                  float min_value = std::numeric_limits<float>::lowest(),
                  float max_value = std::numeric_limits<float>::max())
      : id(id),
        default_value(default_value),
        min_value(min_value),
        max_value(max_value) {}

  /// Constructs new `ParamDefinition` for a boolean value.
  ///
  /// @param id Identifier.
  /// @param default_value Default boolean value.
  ParamDefinition(ParamId id, bool default_value)
      : ParamDefinition(id, static_cast<float>(default_value)) {}

  /// Constructs new `ParamDefinition` for an integer value.
  ///
  /// @param id Identifier.
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  ParamDefinition(ParamId id, int default_value,
                  int min_value = std::numeric_limits<int>::lowest(),
                  int max_value = std::numeric_limits<int>::max())
      : ParamDefinition(id, static_cast<float>(default_value),
                        static_cast<float>(min_value),
                        static_cast<float>(max_value)) {}

  /// Identifier.
  ParamId id;

  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;
};

/// Conductor definition.
struct ConductorDefinition {
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
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  using SetParamFn = void (*)(void** state, ParamId id, float value);

  /// Set stress function signature.
  ///
  /// @param state Pointer to conductor state.
  /// @param stress Stress.
  using SetStressFn = void (*)(void** state, float stress);

  // TODO(#85): Implement `BarelyConductorDefinition_TransformNoteDurationFn`.
  // TODO(#85): Implement `BarelyConductorDefinition_TransformNoteIntensityFn`.
  // TODO(#85): Implement `BarelyConductorDefinition_TransformNotePitchFn`.
  // TODO(#85): Implement `BarelyConductorDefinition_TransformTempoFn`.

  /// Create function.
  CreateFn create_fn;

  /// Destroy function.
  DestroyFn destroy_fn;

  /// Set data function.
  SetDataFn set_data_fn;

  /// Set energy function.
  SetEnergyFn set_energy_fn;

  /// Set parameter function.
  SetParamFn set_param_fn;

  /// Set stress function.
  SetStressFn set_stress_fn;

  // TODO(#85): Add `transform_duration_fn`.
  // TODO(#85): Add `transform_intensity_fn`.
  // TODO(#85): Add `transform_pitch_fn`.
  // TODO(#85): Add `transform_tempo_fn`.

  /// List of parameter definitions.
  std::vector<ParamDefinition> param_definitions;
};

/// Conductor.
class Conductor {
 public:
  /// Returns energy.
  ///
  /// @return Energy.
  float GetEnergy() const {
    float energy = 0.0f;
    if (capi_) {
      const auto status = BarelyConductor_GetEnergy(capi_, &energy);
      assert(status == BarelyStatus_kOk);
    }
    return energy;
  }

  /// Returns parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter value, or error status.
  StatusOr<float> GetParam(ParamId id) const {
    float value = 0.0f;
    if (const auto status = BarelyConductor_GetParam(capi_, id, &value);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return value;
  }

  /// Returns parameter definition.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter definition, or error status.
  StatusOr<ParamDefinition> GetParamDefinition(ParamId id) const {
    BarelyParamDefinition definition;
    if (const auto status =
            BarelyConductor_GetParamDefinition(capi_, id, &definition);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return ParamDefinition(definition.id, definition.default_value,
                           definition.min_value, definition.max_value);
  }

  /// Returns root note.
  ///
  /// @return Root note pitch.
  float GetRootNote() const {
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
  std::vector<float> GetScale() const {
    float* scale_pitches = nullptr;
    int num_scale_pitches = 0;
    if (capi_) {
      const auto status =
          BarelyConductor_GetScale(capi_, &scale_pitches, &num_scale_pitches);
      assert(status == BarelyStatus_kOk);
    }
    return std::vector<float>(scale_pitches, scale_pitches + num_scale_pitches);
  }

  /// Returns stress.
  ///
  /// @return Stress.
  float GetStress() const {
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
  Status ResetAllParams() {
    return static_cast<Status>(BarelyConductor_ResetAllParams(capi_));
  }

  /// Resets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Status.
  Status ResetParam(ParamId id) {
    return static_cast<Status>(BarelyConductor_ResetParam(capi_, id));
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
  Status SetDefinition(ConductorDefinition definition) {
    std::vector<BarelyParamDefinition> param_definitions;
    param_definitions.reserve(definition.param_definitions.size());
    for (const auto& param_definition : definition.param_definitions) {
      param_definitions.emplace_back(
          param_definition.id, param_definition.default_value,
          param_definition.min_value, param_definition.max_value);
    }
    // TODO(#85): Define and include the transform functions.
    return static_cast<Status>(BarelyConductor_SetDefinition(
        capi_,
        BarelyConductorDefinition{
            std::move(definition.create_fn), std::move(definition.destroy_fn),
            std::move(definition.set_data_fn),
            std::move(definition.set_energy_fn),
            std::move(definition.set_param_fn),
            std::move(definition.set_stress_fn),
            /*transform_duration_fn=*/nullptr,
            /*transform_intensity_fn=*/nullptr,
            /*transform_pitch_fn=*/nullptr,
            /*transform_tempo_fn=*/nullptr, param_definitions.data(),
            static_cast<int>(param_definitions.size())}));
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
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParam(ParamId id, float value) {
    return static_cast<Status>(BarelyConductor_SetParam(capi_, id, value));
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
  friend class Api;

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

/// Playback transport.
class Transport {
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

  /// Returns position.
  ///
  /// @return Position in beats.
  double GetPosition() const {
    double position = 0.0;
    if (capi_) {
      const auto status = BarelyTransport_GetPosition(capi_, &position);
      assert(status == BarelyStatus_kOk);
    }
    return position;
  }

  /// Returns tempo.
  ///
  /// @return Tempo in bpm.
  double GetTempo() const {
    double tempo = 0.0;
    if (capi_) {
      const auto status = BarelyTransport_GetTempo(capi_, &tempo);
      assert(status == BarelyStatus_kOk);
    }
    return tempo;
  }

  /// Returns whether transport is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const {
    bool is_playing = false;
    if (capi_) {
      const auto status = BarelyTransport_IsPlaying(capi_, &is_playing);
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
      return static_cast<Status>(BarelyTransport_SetBeatCallback(
          capi_,
          [](double position, double timestamp, void* user_data) {
            (*reinterpret_cast<BeatCallback*>(user_data))(position, timestamp);
          },
          reinterpret_cast<void*>(&beat_callback_)));
    } else {
      return static_cast<Status>(BarelyTransport_SetBeatCallback(
          capi_, /*beat_callback=*/nullptr, /*user_data=*/nullptr));
    }
  }

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position) {
    return static_cast<Status>(BarelyTransport_SetPosition(capi_, position));
  }

  /// Sets tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Status.
  Status SetTempo(double tempo) {
    return static_cast<Status>(BarelyTransport_SetTempo(capi_, tempo));
  }

  /// Sets update callback.
  ///
  /// @param update_callback Update callback.
  /// @return Status.
  Status SetUpdateCallback(UpdateCallback update_callback) {
    if (update_callback) {
      update_callback_ = std::move(update_callback);
      return static_cast<Status>(BarelyTransport_SetUpdateCallback(
          capi_,
          [](double begin_position, double end_position, double begin_timestamp,
             double end_timestamp, void* user_data) {
            (*reinterpret_cast<UpdateCallback*>(user_data))(
                begin_position, end_position, begin_timestamp, end_timestamp);
          },
          reinterpret_cast<void*>(&update_callback_)));
    } else {
      return static_cast<Status>(BarelyTransport_SetUpdateCallback(
          capi_, /*update_callback=*/nullptr, /*user_data=*/nullptr));
    }
  }

  /// Starts playback.
  Status Start() { return static_cast<Status>(BarelyTransport_Start(capi_)); }

  /// Stops playback.
  Status Stop() { return static_cast<Status>(BarelyTransport_Stop(capi_)); }

 private:
  friend class Api;

  // Constructs new `Transport` with internal api handle.
  explicit Transport(BarelyApi capi)
      : capi_(capi), beat_callback_(nullptr), update_callback_(nullptr) {}

  // Default destructor.
  ~Transport() = default;

  // Non-copyable.
  Transport(const Transport& other) = delete;
  Transport& operator=(const Transport& other) = delete;

  // Constructs new `Transport` via move.
  Transport(Transport&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)) {
    SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
    SetUpdateCallback(std::exchange(other.update_callback_, nullptr));
  }

  // Assigns `Transport` via move.
  Transport& operator=(Transport&& other) noexcept {
    if (this != &other) {
      if (capi_) {
        SetBeatCallback(nullptr);
        SetUpdateCallback(nullptr);
      }
      capi_ = std::exchange(other.capi_, nullptr);
      SetBeatCallback(std::exchange(other.beat_callback_, nullptr));
      SetUpdateCallback(std::exchange(other.update_callback_, nullptr));
    }
    return *this;
  }

  // Internal api handle.
  BarelyApi capi_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Update callback.
  UpdateCallback update_callback_;
};

/// BarelyMusician C++ api.
class Api {
 public:
  /// Constructs new `Api`.
  Api() : capi_(CreateCapi()), conductor_(capi_), transport_(capi_) {}

  /// Constructs new `Api` with an initial sampling rate.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit Api(int sample_rate) : Api() {
    const auto status = BarelyApi_SetSampleRate(capi_, sample_rate);
    assert(status == BarelyStatus_kOk);
  }

  /// Destroys `Api`.
  ~Api() {
    DestroyCapi(capi_);
    capi_ = nullptr;
  }

  /// Non-copyable.
  Api(const Api& other) = delete;
  Api& operator=(const Api& other) = delete;

  /// Constructs new `Api` via move.
  ///
  /// @param other Other api.
  Api(Api&& other) noexcept
      : capi_(std::exchange(other.capi_, nullptr)),
        conductor_(std::move(other.conductor_)),
        transport_(std::move(other.transport_)) {}

  /// Assigns `Api` via move.
  ///
  /// @param other Other api.
  Api& operator=(Api&& other) noexcept {
    if (this != &other) {
      conductor_ = std::move(other.conductor_);
      transport_ = std::move(other.transport_);
      DestroyCapi(capi_);
      capi_ = std::exchange(other.capi_, nullptr);
    }
    return *this;
  }

  /// Returns conductor.
  ///
  /// @return Conductor.
  const Conductor& GetConductor() const { return conductor_; }

  /// Returns conductor.
  ///
  /// @return Mutable conductor.
  Conductor& GetConductor() { return conductor_; }

  /// Returns sampling rate.
  ///
  /// @return Sampling rate in hz, or error status.
  int GetSampleRate() const {
    int sample_rate = 0;
    if (capi_) {
      const auto status = BarelyApi_GetSampleRate(capi_, &sample_rate);
      assert(status == BarelyStatus_kOk);
    }
    return sample_rate;
  }

  /// Returns transport.
  ///
  /// @return Transport.
  const Transport& GetTransport() const { return transport_; }

  /// Returns transport.
  ///
  /// @return Mutable transport.
  Transport& GetTransport() { return transport_; }

  /// Sets sampling rate.
  ///
  /// @param sample_rate Sampling rate in hz.
  /// @return Status.
  Status SetSampleRate(int sample_rate) {
    return static_cast<Status>(BarelyApi_SetSampleRate(capi_, sample_rate));
  }

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp) {
    return static_cast<Status>(BarelyApi_Update(capi_, timestamp));
  }

 private:
  friend class Instrument;
  friend class Sequence;

  // Creates new internal api and returns corresponding handle.
  BarelyApi CreateCapi() {
    BarelyApi capi = nullptr;
    const auto status = BarelyApi_Create(&capi);
    assert(status == BarelyStatus_kOk);
    return capi;
  }

  // Destroys internal api with corresponding handle.
  void DestroyCapi(BarelyApi capi) {
    if (capi) {
      const auto status = BarelyApi_Destroy(capi);
      assert(status == BarelyStatus_kOk);
    }
  }

  // Internal api handle.
  BarelyApi capi_;

  // Conductor.
  Conductor conductor_;

  // Playback transport.
  Transport transport_;
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
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  using SetParamFn = void (*)(void** state, ParamId id, float value);

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
  SetParamFn set_param_fn;

  /// List of parameter definitions.
  std::vector<ParamDefinition> param_definitions;
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
  ///
  /// @param api BarelyMusician C++ api.
  /// @param definition Definition.
  Instrument(const Api& api, InstrumentDefinition definition)
      : capi_(api.capi_) {
    std::vector<BarelyParamDefinition> param_definitions;
    param_definitions.reserve(definition.param_definitions.size());
    for (const auto& param_definition : definition.param_definitions) {
      param_definitions.emplace_back(
          param_definition.id, param_definition.default_value,
          param_definition.min_value, param_definition.max_value);
    }
    const auto status = BarelyInstrument_Create(
        capi_,
        BarelyInstrumentDefinition{
            std::move(definition.create_fn), std::move(definition.destroy_fn),
            std::move(definition.process_fn), std::move(definition.set_data_fn),
            std::move(definition.set_note_off_fn),
            std::move(definition.set_note_on_fn),
            std::move(definition.set_param_fn), param_definitions.data(),
            static_cast<int>(param_definitions.size())},
        &id_);
    assert(status == BarelyStatus_kOk);
  }

  /// Destroys `Instrument`.
  ~Instrument() { BarelyInstrument_Destroy(capi_, id_); }

  // TODO(#85): Should `Instrument` be non-movable and non-copyable?

  // TODO(#85): Implement `BarelyInstrument_Clone` (via copy?).

  // TODO(#85): Implement `BarelyInstrument_CancelAllScheduledNotes`.
  // TODO(#85): Implement `BarelyInstrument_CancelScheduledNote`.

  /// Returns gain.
  ///
  /// @return Gain in amplitude, or error status.
  StatusOr<float> GetGain() const {
    float gain = 0.0f;
    if (const auto status = BarelyInstrument_GetGain(capi_, id_, &gain);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return gain;
  }

  /// Returns parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter value, or error status.
  StatusOr<float> GetParam(ParamId id) const {
    float value = 0.0f;
    if (const auto status = BarelyInstrument_GetParam(capi_, id_, id, &value);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return value;
  }

  /// Returns parameter definition.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter definition, or error status.
  StatusOr<ParamDefinition> GetParamDefinition(ParamId id) const {
    BarelyParamDefinition definition;
    if (const auto status =
            BarelyInstrument_GetParamDefinition(capi_, id_, id, &definition);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return ParamDefinition(definition.id, definition.default_value,
                           definition.min_value, definition.max_value);
  }

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise, or error status.
  StatusOr<bool> IsMuted() const {
    bool is_muted = false;
    if (const auto status = BarelyInstrument_IsMuted(capi_, id_, &is_muted);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return is_muted;
  }

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise, or error status.
  StatusOr<bool> IsNoteOn(float pitch) const {
    bool is_note_on = false;
    // TODO(#85): Support other `NotePitchType`s.
    if (const auto status = BarelyInstrument_IsNoteOn(
            capi_, id_, BarelyNotePitchType_AbsolutePitch, pitch, &is_note_on);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
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
  Status ResetAllParams() {
    return static_cast<Status>(BarelyInstrument_ResetAllParams(capi_, id_));
  }

  /// Resets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Status.
  Status ResetParam(ParamId id) {
    return static_cast<Status>(BarelyInstrument_ResetParam(capi_, id_, id));
  }

  // TODO(#85): Implement `BarelyInstrument_ScheduleNote`.

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
    } else {
      return static_cast<Status>(
          BarelyInstrument_SetNoteOffCallback(capi_, id_, nullptr, nullptr));
    }
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
    } else {
      return static_cast<Status>(
          BarelyInstrument_SetNoteOnCallback(capi_, id_, nullptr, nullptr));
    }
  }

  /// Sets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParam(ParamId id, float value) {
    return static_cast<Status>(
        BarelyInstrument_SetParam(capi_, id_, id, value));
  }

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(float pitch, float intensity = 1.0f) {
    // TODO(#85): Support other `NotePitchType`s.
    return static_cast<Status>(BarelyInstrument_StartNote(
        capi_, id_, BarelyNotePitchType_AbsolutePitch, pitch, intensity));
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
    // TODO(#85): Support other `NotePitchType`s.
    return static_cast<Status>(BarelyInstrument_StopNote(
        capi_, id_, BarelyNotePitchType_AbsolutePitch, pitch));
  }

 private:
  // Internal C api handle.
  const BarelyApi& capi_;

  // Identifier.
  BarelyId id_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

/// Note sequence.
class Sequence {
 public:
  /// Constructs new `Sequence`.
  ///
  /// @param api BarelyMusician C++ api.
  explicit Sequence(const Api& api) : capi_(api.capi_) {
    const auto status = BarelySequence_Create(capi_, &id_);
    assert(status == BarelyStatus_kOk);
  }

  /// Destroys `Sequence`.
  ~Sequence() { BarelySequence_Destroy(capi_, id_); }

  // TODO(#85): Should `Sequence` be non-movable and non-copyable?

  // TODO(#85): Implement `BarelySequence_Clone` (via copy?).

  // TODO(#85): Implement `BarelySequence_AddNote`.

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats, or error status.
  StatusOr<double> GetBeginOffset() const {
    double begin_offset = 0.0;
    if (const auto status =
            BarelySequence_GetBeginOffset(capi_, id_, &begin_offset);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return begin_offset;
  }

  /// Returns begin position.
  ///
  /// @return Begin position in beats, or error status.
  StatusOr<double> GetBeginPosition() const {
    double begin_position = 0.0;
    if (const auto status =
            BarelySequence_GetBeginPosition(capi_, id_, &begin_position);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return begin_position;
  }

  /// Returns end position.
  ///
  /// @return End position in beats, or error status.
  StatusOr<double> GetEndPosition() const {
    double end_position = 0.0;
    if (const auto status =
            BarelySequence_GetEndPosition(capi_, id_, &end_position);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return end_position;
  }

  // TODO(#85): Implement `BarelySequence_GetInstrument`.

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats, or error status.
  StatusOr<double> GetLoopBeginOffset() const {
    double loop_begin_offset = 0.0;
    if (const auto status =
            BarelySequence_GetLoopBeginOffset(capi_, id_, &loop_begin_offset);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return loop_begin_offset;
  }

  /// Returns loop length.
  ///
  /// @return Loop length in beats, or error status.
  StatusOr<double> GetLoopLength() const {
    double loop_length = 0.0;
    if (const auto status =
            BarelySequence_GetLoopLength(capi_, id_, &loop_length);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return loop_length;
  }

  // TODO(#85): Implement `BarelySequence_GetNoteDuration`.
  // TODO(#85): Implement `BarelySequence_GetNoteIntensity`.
  // TODO(#85): Implement `BarelySequence_GetNotePitch`.
  // TODO(#85): Implement `BarelySequence_GetNotePosition`.

  /// Returns whether sequence is empty or not.
  ///
  /// @return True if empty, false otherwise, or error status.
  StatusOr<bool> IsEmpty() const {
    bool is_empty = false;
    if (const auto status = BarelySequence_IsEmpty(capi_, id_, &is_empty);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return is_empty;
  }

  /// Returns whether sequence should be looping or not.
  ///
  /// @return True if looping, false otherwise, or error status.
  StatusOr<bool> IsLooping() const {
    bool is_looping = false;
    if (const auto status = BarelySequence_IsLooping(capi_, id_, &is_looping);
        status != BarelyStatus_kOk) {
      return static_cast<Status>(status);
    }
    return is_looping;
  }

  // TODO(#85): Implement `BarelySequence_RemoveAllNotes`.
  // TODO(#85): Implement `BarelySequence_RemoveNote`.

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

  // TODO(#85): Implement `BarelySequence_SetInstrument`.

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

  // TODO(#85): Implement `BarelySequence_SetNoteDuration`.
  // TODO(#85): Implement `BarelySequence_SetNoteIntensity`.
  // TODO(#85): Implement `BarelySequence_SetNotePitch`.
  // TODO(#85): Implement `BarelySequence_SetNotePosition`.

 private:
  // Internal api handle.
  const BarelyApi& capi_;

  // Identifier.
  BarelyId id_;
};

}  // namespace barely

#endif PLATFORMS_API_BARELYMUSICIAN_H_
