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

/// Parameter identifier type.
using ParamId = std::int32_t;

/// Parameter definition.
struct ParamDefinition {
  /// Constructs new `ParamDefinition`.
  ///
  /// @param id Identifier.
  explicit ParamDefinition(ParamId id);

  /// Constructs new `ParamDefinition` for a float value.
  ///
  /// @param id Identifier.
  /// @param default_value Default float value.
  /// @param min_value Minimum float value.
  /// @param max_value Maximum float value.
  ParamDefinition(ParamId id, float default_value,
                  float min_value = std::numeric_limits<float>::lowest(),
                  float max_value = std::numeric_limits<float>::max());

  /// Constructs new `ParamDefinition` for a boolean value.
  ///
  /// @param id Identifier.
  /// @param default_value Default boolean value.
  ParamDefinition(ParamId id, bool default_value);

  /// Constructs new `ParamDefinition` for an integer value.
  ///
  /// @param id Identifier.
  /// @param default_value Default integer value.
  /// @param min_value Minimum integer value.
  /// @param max_value Maximum integer value.
  ParamDefinition(ParamId id, int default_value,
                  int min_value = std::numeric_limits<int>::lowest(),
                  int max_value = std::numeric_limits<int>::max());

  /// Identifier.
  ParamId id;

  /// Default value.
  float default_value;

  /// Minimum value.
  float min_value;

  /// Maximum value.
  float max_value;

 private:
  friend class Conductor;
  friend struct ConductorDefinition;
  friend class Instrument;
  friend struct InstrumentDefinition;

  /// Returns corresponding `ParamDefinition` for C type for internal use.
  static ParamDefinition FromBarelyParamDefinition(
      BarelyParamDefinition definition);

  // Returns corresponding C type for internal use.
  BarelyParamDefinition GetBarelyParamDefinition() const;
};

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

/// Value or error status.
template <typename ValueType>
class StatusOr {
 public:
  /// Constructs new `StatusOr` with an error status.
  ///
  /// @param error_status Error status.
  StatusOr(Status error_status);

  /// Constructs new `StatusOr` with a value.
  ///
  /// @param value Value.
  StatusOr(ValueType value);

  /// Returns contained error status.
  ///
  /// @return Error status.
  Status GetErrorStatus() const;

  /// Returns contained value.
  ///
  /// @return Value.
  const ValueType& GetValue() const;

  /// Returns contained value.
  ///
  /// @return Mutable value.
  ValueType& GetValue();

  /// Returns whether a value is contained or not.
  ///
  /// @return True if contained, false otherwise.
  bool IsOk() const;

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// BarelyMusician C++ api.
class Api {
 public:
  /// Constructs new `Api`.
  Api();

  /// Constructs new `Api` with an initial sampling rate.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit Api(int sample_rate);

  /// Destroys `Api`.
  ~Api();

  /// Non-copyable and non-movable.
  Api(const Api& other) = delete;
  Api& operator=(const Api& other) noexcept = delete;
  Api(Api&& other) = delete;
  Api& operator=(Api&& other) noexcept = delete;

  /// Returns sampling rate.
  ///
  /// @return Sampling rate in hz, or error status.
  StatusOr<int> GetSampleRate() const;

  /// Sets sampling rate.
  ///
  /// @param sample_rate Sampling rate in hz.
  /// @return Status.
  Status SetSampleRate(int sample_rate);

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp);

 private:
  friend class Conductor;
  friend class Instrument;
  friend class Sequence;
  friend class Transport;

  // Internal C api handle.
  BarelyApi capi_;
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

 private:
  friend class Conductor;

  // Returns corresponding C type for internal use.
  std::vector<BarelyParamDefinition> GetBarelyParamDefinitions() const;
};

/// Conductor.
class Conductor {
 public:
  /// Constructs new `Conductor`.
  ///
  /// @param api BarelyMusician C++ api.
  explicit Conductor(const Api& api);

  // TODO(#85): Should `Conductor` be non-movable and non-copyable?

  /// Returns energy.
  ///
  /// @return Energy, or error status.
  StatusOr<float> GetEnergy() const;

  /// Returns parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter value, or error status.
  StatusOr<float> GetParam(ParamId id) const;

  /// Returns parameter definition.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter definition, or error status.
  StatusOr<ParamDefinition> GetParamDefinition(ParamId id) const;

  /// Returns root note.
  ///
  /// @return Root note pitch, or error status.
  StatusOr<float> GetRootNote() const;

  /// Returns scale.
  ///
  /// @return List of scale note pitches, or error status.
  StatusOr<std::vector<float>> GetScale() const;

  /// Returns stress.
  ///
  /// @return Stress, or error status.
  StatusOr<float> GetStress() const;

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParams();

  /// Resets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Status.
  Status ResetParam(ParamId id);

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  Status SetData(void* data);

  /// Sets definition.
  ///
  /// @param definition Conductor definition.
  /// @return Status.
  Status SetDefinition(ConductorDefinition definition);

  /// Sets energy.
  ///
  /// @param energy Energy.
  /// @return Status.
  Status SetEnergy(float energy);

  /// Sets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParam(ParamId id, float value);

  /// Sets root note.
  ///
  /// @param root_pitch Root note pitch.
  /// @return Status.
  Status SetRootNote(float root_pitch);

  /// Sets scale.
  ///
  /// @param scale_pitches List of scale note pitches.
  /// @return Status.
  Status SetScale(std::vector<float> scale_pitches);

  /// Sets stress.
  ///
  /// @param stress Stress.
  /// @return Status.
  Status SetStress(float stress);

 private:
  // Internal api handle.
  const BarelyApi& capi_;
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

 private:
  friend class Instrument;

  // Returns corresponding C type for internal use.
  std::vector<BarelyParamDefinition> GetBarelyParamDefinitions() const;
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
  Instrument(const Api& api, InstrumentDefinition definition);

  /// Destroys `Instrument`.
  ~Instrument();

  // TODO(#85): Should `Instrument` be non-movable and non-copyable?

  // TODO(#85): Implement `BarelyInstrument_Clone` (via copy?).

  // TODO(#85): Implement `BarelyInstrument_CancelAllScheduledNotes`.
  // TODO(#85): Implement `BarelyInstrument_CancelScheduledNote`.

  /// Returns gain.
  ///
  /// @return Gain in amplitude, or error status.
  StatusOr<float> GetGain() const;

  /// Returns parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter value, or error status.
  StatusOr<float> GetParam(ParamId id) const;

  /// Returns parameter definition.
  ///
  /// @param id Parameter identifier.
  /// @return Parameter definition, or error status.
  StatusOr<ParamDefinition> GetParamDefinition(ParamId id) const;

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise, or error status.
  StatusOr<bool> IsMuted() const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise, or error status.
  StatusOr<bool> IsNoteOn(float pitch) const;

  /// Processes output buffer at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @return Status.
  Status Process(double timestamp, float* output, int num_output_channels,
                 int num_output_frames);

  /// Resets all parameters.
  ///
  /// @return Status.
  Status ResetAllParams();

  /// Resets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @return Status.
  Status ResetParam(ParamId id);

  // TODO(#85): Implement `BarelyInstrument_ScheduleNote`.

  /// Sets data.
  ///
  /// @param data Data.
  /// @return Status.
  Status SetData(void* data);

  /// Sets gain.
  ///
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetGain(float gain);

  /// Sets whether instrument should be muted or not.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetMuted(bool is_muted);

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets parameter value.
  ///
  /// @param id Parameter identifier.
  /// @param value Parameter value.
  /// @return Status.
  Status SetParam(ParamId id, float value);

  /// Starts note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status StartNote(float pitch, float intensity = 1.0f);

  /// Stops all notes.
  ///
  /// @return Status.
  Status StopAllNotes();

  /// Stops note.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status StopNote(float pitch);

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
  explicit Sequence(const Api& api);

  /// Destroys `Sequence`.
  ~Sequence();

  // TODO(#85): Should `Sequence` be non-movable and non-copyable?

  // TODO(#85): Implement `BarelySequence_Clone` (via copy?).

  // TODO(#85): Implement `BarelySequence_AddNote`.

  /// Returns begin offset.
  ///
  /// @return Begin offset in beats, or error status.
  StatusOr<double> GetBeginOffset() const;

  /// Returns begin position.
  ///
  /// @return Begin position in beats, or error status.
  StatusOr<double> GetBeginPosition() const;

  /// Returns end position.
  ///
  /// @return End position in beats, or error status.
  StatusOr<double> GetEndPosition() const;

  // TODO(#85): Implement `BarelySequence_GetInstrument`.

  /// Returns loop begin offset.
  ///
  /// @return Loop begin offset in beats, or error status.
  StatusOr<double> GetLoopBeginOffset() const;

  /// Returns loop length.
  ///
  /// @return Loop length in beats, or error status.
  StatusOr<double> GetLoopLength() const;

  // TODO(#85): Implement `BarelySequence_GetNoteDuration`.
  // TODO(#85): Implement `BarelySequence_GetNoteIntensity`.
  // TODO(#85): Implement `BarelySequence_GetNotePitch`.
  // TODO(#85): Implement `BarelySequence_GetNotePosition`.

  /// Returns whether sequence is empty or not.
  ///
  /// @return True if empty, false otherwise, or error status.
  StatusOr<bool> IsEmpty() const;

  /// Returns whether sequence should be looping or not.
  ///
  /// @return True if looping, false otherwise, or error status.
  StatusOr<bool> IsLooping() const;

  // TODO(#85): Implement `BarelySequence_RemoveAllNotes`.
  // TODO(#85): Implement `BarelySequence_RemoveNote`.

  /// Sets begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetBeginOffset(double begin_offset);

  /// Sets begin position.
  ///
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetBeginPosition(double begin_position);

  /// Sets end position.
  ///
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetEndPosition(double end_position);

  // TODO(#85): Implement `BarelySequence_SetInstrument`.

  /// Sets loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetLoopBeginOffset(double loop_begin_offset);

  /// Sets loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length);

  /// Sets whether sequence should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping);

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

  /// Constructs new `Transport`.
  ///
  /// @param api BarelyMusician C++ api.
  explicit Transport(const Api& api);

  // TODO(#85): Should `Transport` be non-movable and non-copyable?

  /// Returns position.
  ///
  /// @return Position in beats, or error status.
  StatusOr<double> GetPosition() const;

  /// Returns tempo.
  ///
  /// @return Tempo in BPM, or error status.
  StatusOr<double> GetTempo() const;

  /// Returns whether transport is playing or not.
  ///
  /// @return True if playing, false otherwise, or error status.
  StatusOr<bool> IsPlaying() const;

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback beat_callback);

  /// Sets position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position);

  /// Sets tempo.
  ///
  /// @param tempo Tempo in BPM.
  /// @return Status.
  Status SetTempo(double tempo);

  /// Sets update callback.
  ///
  /// @param update_callback Update callback.
  /// @return Status.
  Status SetUpdateCallback(UpdateCallback update_callback);

  /// Starts playback.
  Status Start();

  /// Stops playback.
  Status Stop();

 private:
  // Internal api handle.
  const BarelyApi& capi_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Update callback.
  UpdateCallback update_callback_;
};

ParamDefinition::ParamDefinition(ParamId id) : ParamDefinition(id, 0.0f) {}

ParamDefinition::ParamDefinition(ParamId id, float default_value,
                                 float min_value, float max_value)
    : id(id),
      default_value(default_value),
      min_value(min_value),
      max_value(max_value) {}

ParamDefinition::ParamDefinition(ParamId id, bool default_value)
    : ParamDefinition(id, static_cast<float>(default_value)) {}

ParamDefinition::ParamDefinition(ParamId id, int default_value, int min_value,
                                 int max_value)
    : ParamDefinition(id, static_cast<float>(default_value),
                      static_cast<float>(min_value),
                      static_cast<float>(max_value)) {}

ParamDefinition ParamDefinition::FromBarelyParamDefinition(
    BarelyParamDefinition definition) {
  return ParamDefinition(definition.id, definition.default_value,
                         definition.min_value, definition.max_value);
}

BarelyParamDefinition ParamDefinition::GetBarelyParamDefinition() const {
  return BarelyParamDefinition{id, default_value, min_value, max_value};
}

template <typename ValueType>
StatusOr<ValueType>::StatusOr(Status error_status) : value_or_(error_status) {
  assert(error_status != Status::kOk);
}

template <typename ValueType>
StatusOr<ValueType>::StatusOr(ValueType value) : value_or_(std::move(value)) {}

template <typename ValueType>
Status StatusOr<ValueType>::GetErrorStatus() const {
  assert(std::holds_alternative<Status>(value_or_));
  return std::get<Status>(value_or_);
}

template <typename ValueType>
const ValueType& StatusOr<ValueType>::GetValue() const {
  assert(std::holds_alternative<ValueType>(value_or_));
  return std::get<ValueType>(value_or_);
}

template <typename ValueType>
ValueType& StatusOr<ValueType>::GetValue() {
  assert(std::holds_alternative<ValueType>(value_or_));
  return std::get<ValueType>(value_or_);
}

template <typename ValueType>
bool StatusOr<ValueType>::IsOk() const {
  return std::holds_alternative<ValueType>(value_or_);
}

Api::Api() {
  const auto status = BarelyApi_Create(&capi_);
  assert(status == BarelyStatus_kOk);
}

Api::Api(int sample_rate) : Api() {
  const auto status = BarelyApi_SetSampleRate(capi_, sample_rate);
  assert(status == BarelyStatus_kOk);
}

Api::~Api() {
  const auto status = BarelyApi_Destroy(capi_);
  assert(status == BarelyStatus_kOk);
}

StatusOr<int> Api::GetSampleRate() const {
  int sample_rate = 0;
  if (const auto status = BarelyApi_GetSampleRate(capi_, &sample_rate);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return sample_rate;
}

Status Api::SetSampleRate(int sample_rate) {
  return static_cast<Status>(BarelyApi_SetSampleRate(capi_, sample_rate));
}

Status Api::Update(double timestamp) {
  return static_cast<Status>(BarelyApi_Update(capi_, timestamp));
}

std::vector<BarelyParamDefinition>
ConductorDefinition::GetBarelyParamDefinitions() const {
  std::vector<BarelyParamDefinition> cparam_definitions;
  cparam_definitions.reserve(param_definitions.size());
  for (const auto& param_definition : param_definitions) {
    cparam_definitions.push_back(param_definition.GetBarelyParamDefinition());
  }
  return cparam_definitions;
}

Conductor::Conductor(const Api& api) : capi_(api.capi_) {}

StatusOr<float> Conductor::GetEnergy() const {
  float energy = 0.0f;
  if (const auto status = BarelyConductor_GetEnergy(capi_, &energy);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return energy;
}

StatusOr<float> Conductor::GetParam(ParamId id) const {
  float value = 0.0f;
  if (const auto status = BarelyConductor_GetParam(capi_, id, &value);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return value;
}

StatusOr<ParamDefinition> Conductor::GetParamDefinition(ParamId id) const {
  BarelyParamDefinition definition;
  if (const auto status =
          BarelyConductor_GetParamDefinition(capi_, id, &definition);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return ParamDefinition::FromBarelyParamDefinition(std::move(definition));
}

StatusOr<float> Conductor::GetRootNote() const {
  float root_pitch = 0.0f;
  if (const auto status = BarelyConductor_GetRootNote(capi_, &root_pitch);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return root_pitch;
}

StatusOr<std::vector<float>> Conductor::GetScale() const {
  float* scale_pitches;
  int num_scale_pitches;
  if (const auto status =
          BarelyConductor_GetScale(capi_, &scale_pitches, &num_scale_pitches);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return std::vector<float>(scale_pitches, scale_pitches + num_scale_pitches);
}

StatusOr<float> Conductor::GetStress() const {
  float stress = 0.0f;
  if (const auto status = BarelyConductor_GetStress(capi_, &stress);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return stress;
}

Status Conductor::ResetAllParams() {
  return static_cast<Status>(BarelyConductor_ResetAllParams(capi_));
}

Status Conductor::ResetParam(ParamId id) {
  return static_cast<Status>(BarelyConductor_ResetParam(capi_, id));
}

Status Conductor::SetData(void* data) {
  return static_cast<Status>(BarelyConductor_SetData(capi_, data));
}

Status Conductor::SetDefinition(ConductorDefinition definition) {
  auto cparam_definitions = definition.GetBarelyParamDefinitions();
  // TODO(#85): Define and include the transform functions.
  auto cdefinition =
      BarelyConductorDefinition{std::move(definition.create_fn),
                                std::move(definition.destroy_fn),
                                std::move(definition.set_data_fn),
                                std::move(definition.set_energy_fn),
                                std::move(definition.set_param_fn),
                                std::move(definition.set_stress_fn),
                                /*transform_duration_fn=*/nullptr,
                                /*transform_intensity_fn=*/nullptr,
                                /*transform_pitch_fn=*/nullptr,
                                /*transform_tempo_fn=*/nullptr,
                                cparam_definitions.data(),
                                static_cast<int>(cparam_definitions.size())};
  return static_cast<Status>(
      BarelyConductor_SetDefinition(capi_, std::move(cdefinition)));
}

Status Conductor::SetEnergy(float energy) {
  return static_cast<Status>(BarelyConductor_SetEnergy(capi_, energy));
}

Status Conductor::SetParam(ParamId id, float value) {
  return static_cast<Status>(BarelyConductor_SetParam(capi_, id, value));
}

Status Conductor::SetRootNote(float root_pitch) {
  return static_cast<Status>(BarelyConductor_SetRootNote(capi_, root_pitch));
}

Status Conductor::SetStress(float stress) {
  return static_cast<Status>(BarelyConductor_SetStress(capi_, stress));
}

std::vector<BarelyParamDefinition>
InstrumentDefinition::GetBarelyParamDefinitions() const {
  std::vector<BarelyParamDefinition> cparam_definitions;
  cparam_definitions.reserve(param_definitions.size());
  for (const auto& param_definition : param_definitions) {
    cparam_definitions.push_back(param_definition.GetBarelyParamDefinition());
  }
  return cparam_definitions;
}

Instrument::Instrument(const Api& api, InstrumentDefinition definition)
    : capi_(api.capi_) {
  auto cparam_definitions = definition.GetBarelyParamDefinitions();
  auto cdefinition =
      BarelyInstrumentDefinition{std::move(definition.create_fn),
                                 std::move(definition.destroy_fn),
                                 std::move(definition.process_fn),
                                 std::move(definition.set_data_fn),
                                 std::move(definition.set_note_off_fn),
                                 std::move(definition.set_note_on_fn),
                                 std::move(definition.set_param_fn),
                                 cparam_definitions.data(),
                                 static_cast<int>(cparam_definitions.size())};
  const auto status =
      BarelyInstrument_Create(capi_, std::move(cdefinition), &id_);
  assert(status == BarelyStatus_kOk);
}

Instrument::~Instrument() { BarelyInstrument_Destroy(capi_, id_); }

StatusOr<float> Instrument::GetGain() const {
  float gain = 0.0f;
  if (const auto status = BarelyInstrument_GetGain(capi_, id_, &gain);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return gain;
}

StatusOr<float> Instrument::GetParam(ParamId id) const {
  float value = 0.0f;
  if (const auto status = BarelyInstrument_GetParam(capi_, id_, id, &value);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return value;
}

StatusOr<ParamDefinition> Instrument::GetParamDefinition(ParamId id) const {
  BarelyParamDefinition definition;
  if (const auto status =
          BarelyInstrument_GetParamDefinition(capi_, id_, id, &definition);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return ParamDefinition::FromBarelyParamDefinition(std::move(definition));
}

StatusOr<bool> Instrument::IsMuted() const {
  bool is_muted = false;
  if (const auto status = BarelyInstrument_IsMuted(capi_, id_, &is_muted);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return is_muted;
}

StatusOr<bool> Instrument::IsNoteOn(float pitch) const {
  bool is_note_on = false;
  if (const auto status =
          BarelyInstrument_IsNoteOn(capi_, id_, pitch, &is_note_on);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return is_note_on;
}

Status Instrument::Process(double timestamp, float* output,
                           int num_output_channels, int num_output_frames) {
  return static_cast<Status>(BarelyInstrument_Process(
      capi_, id_, timestamp, output, num_output_channels, num_output_frames));
}

Status Instrument::ResetAllParams() {
  return static_cast<Status>(BarelyInstrument_ResetAllParams(capi_, id_));
}

Status Instrument::ResetParam(ParamId id) {
  return static_cast<Status>(BarelyInstrument_ResetParam(capi_, id_, id));
}

Status Instrument::SetData(void* data) {
  return static_cast<Status>(BarelyInstrument_SetData(capi_, id_, data));
}

Status Instrument::SetGain(float gain) {
  return static_cast<Status>(BarelyInstrument_SetGain(capi_, id_, gain));
}

Status Instrument::SetMuted(bool is_muted) {
  return static_cast<Status>(BarelyInstrument_SetMuted(capi_, id_, is_muted));
}

Status Instrument::SetNoteOffCallback(NoteOffCallback note_off_callback) {
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

Status Instrument::SetNoteOnCallback(NoteOnCallback note_on_callback) {
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

Status Instrument::SetParam(ParamId id, float value) {
  return static_cast<Status>(BarelyInstrument_SetParam(capi_, id_, id, value));
}

Status Instrument::StartNote(float pitch, float intensity) {
  return static_cast<Status>(
      BarelyInstrument_StartNote(capi_, id_, pitch, intensity));
}

Status Instrument::StopAllNotes() {
  return static_cast<Status>(BarelyInstrument_StopAllNotes(capi_, id_));
}

Status Instrument::StopNote(float pitch) {
  return static_cast<Status>(BarelyInstrument_StopNote(capi_, id_, pitch));
}

Sequence::Sequence(const Api& api) : capi_(api.capi_) {
  const auto status = BarelySequence_Create(capi_, &id_);
  assert(status == BarelyStatus_kOk);
}

Sequence::~Sequence() { BarelySequence_Destroy(capi_, id_); }

StatusOr<double> Sequence::GetBeginOffset() const {
  double begin_offset = 0.0;
  if (const auto status =
          BarelySequence_GetBeginOffset(capi_, id_, &begin_offset);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return begin_offset;
}

StatusOr<double> Sequence::GetBeginPosition() const {
  double begin_position = 0.0;
  if (const auto status =
          BarelySequence_GetBeginPosition(capi_, id_, &begin_position);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return begin_position;
}

StatusOr<double> Sequence::GetEndPosition() const {
  double end_position = 0.0;
  if (const auto status =
          BarelySequence_GetEndPosition(capi_, id_, &end_position);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return end_position;
}

StatusOr<double> Sequence::GetLoopBeginOffset() const {
  double loop_begin_offset = 0.0;
  if (const auto status =
          BarelySequence_GetLoopBeginOffset(capi_, id_, &loop_begin_offset);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return loop_begin_offset;
}

StatusOr<double> Sequence::GetLoopLength() const {
  double loop_length = 0.0;
  if (const auto status =
          BarelySequence_GetLoopLength(capi_, id_, &loop_length);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return loop_length;
}

StatusOr<bool> Sequence::IsEmpty() const {
  bool is_empty = false;
  if (const auto status = BarelySequence_IsEmpty(capi_, id_, &is_empty);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return is_empty;
}

StatusOr<bool> Sequence::IsLooping() const {
  bool is_looping = false;
  if (const auto status = BarelySequence_IsLooping(capi_, id_, &is_looping);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return is_looping;
}

Status Sequence::SetBeginOffset(double begin_offset) {
  return static_cast<Status>(
      BarelySequence_SetBeginOffset(capi_, id_, begin_offset));
}

Status Sequence::SetBeginPosition(double begin_position) {
  return static_cast<Status>(
      BarelySequence_SetBeginPosition(capi_, id_, begin_position));
}

Status Sequence::SetEndPosition(double end_position) {
  return static_cast<Status>(
      BarelySequence_SetEndPosition(capi_, id_, end_position));
}

Status Sequence::SetLoopBeginOffset(double loop_begin_offset) {
  return static_cast<Status>(
      BarelySequence_SetLoopBeginOffset(capi_, id_, loop_begin_offset));
}

Status Sequence::SetLoopLength(double loop_length) {
  return static_cast<Status>(
      BarelySequence_SetLoopLength(capi_, id_, loop_length));
}

Status Sequence::SetLooping(bool is_looping) {
  return static_cast<Status>(BarelySequence_SetLooping(capi_, id_, is_looping));
}

Transport::Transport(const Api& api)
    : capi_(api.capi_), beat_callback_(nullptr), update_callback_(nullptr) {}

StatusOr<double> Transport::GetPosition() const {
  double position = 0.0;
  if (const auto status = BarelyTransport_GetPosition(capi_, &position);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return position;
}

StatusOr<double> Transport::GetTempo() const {
  double tempo = 0.0;
  if (const auto status = BarelyTransport_GetTempo(capi_, &tempo);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return tempo;
}

StatusOr<bool> Transport::IsPlaying() const {
  bool is_playing = false;
  if (const auto status = BarelyTransport_IsPlaying(capi_, &is_playing);
      status != BarelyStatus_kOk) {
    return static_cast<Status>(status);
  }
  return is_playing;
}

// TODO(#85): Ensure that the beat callback is updated accordingly for when
//    transport is copied/moved/destroyed.
Status Transport::SetBeatCallback(BeatCallback beat_callback) {
  if (beat_callback) {
    beat_callback_ = std::move(beat_callback);
    return static_cast<Status>(BarelyTransport_SetBeatCallback(
        capi_,
        [](double position, double timestamp, void* user_data) {
          if (user_data) {
            (*static_cast<BeatCallback*>(user_data))(position, timestamp);
          }
        },
        static_cast<void*>(&beat_callback_)));
  } else {
    return static_cast<Status>(
        BarelyTransport_SetBeatCallback(capi_, nullptr, nullptr));
  }
}

Status Transport::SetPosition(double position) {
  return static_cast<Status>(BarelyTransport_SetPosition(capi_, position));
}

Status Transport::SetTempo(double tempo) {
  return static_cast<Status>(BarelyTransport_SetTempo(capi_, tempo));
}

// TODO(#85): Ensure that the update callback is updated accordingly for when
//    transport is copied/moved/destroyed.
Status Transport::SetUpdateCallback(UpdateCallback update_callback) {
  if (update_callback) {
    update_callback_ = std::move(update_callback);
    return static_cast<Status>(BarelyTransport_SetUpdateCallback(
        capi_,
        [](double begin_position, double end_position, double begin_timestamp,
           double end_timestamp, void* user_data) {
          if (user_data) {
            (*static_cast<UpdateCallback*>(user_data))(
                begin_position, end_position, begin_timestamp, end_timestamp);
          }
        },
        static_cast<void*>(&beat_callback_)));
  } else {
    return static_cast<Status>(
        BarelyTransport_SetUpdateCallback(capi_, nullptr, nullptr));
  }
}

Status Transport::Start() {
  return static_cast<Status>(BarelyTransport_Start(capi_));
}

Status Transport::Stop() {
  return static_cast<Status>(BarelyTransport_Stop(capi_));
}

}  // namespace barely

#endif PLATFORMS_API_BARELYMUSICIAN_H_
