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
  /// Constructs new |ParamDefinition|.
  ///
  /// @param id Identifier.
  explicit ParamDefinition(ParamId id);

  /// Constructs new |ParamDefinition| for a float value.
  ///
  /// @param id Identifier.
  /// @param default_value Default float value.
  /// @param min_value Minimum float value.
  /// @param max_value Maximum float value.
  ParamDefinition(ParamId id, float default_value,
                  float min_value = std::numeric_limits<float>::lowest(),
                  float max_value = std::numeric_limits<float>::max());

  /// Constructs new |ParamDefinition| for a boolean value.
  ///
  /// @param id Identifier.
  /// @param default_value Default boolean value.
  ParamDefinition(ParamId id, bool default_value);

  /// Constructs new |ParamDefinition| for an integer value.
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
  friend struct InstrumentDefinition;

  // Returns the corresponding C API type for internal use.
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
  /// Constructs new |StatusOr| with an error status.
  ///
  /// @param error_status Error status.
  StatusOr(Status error_status);

  /// Constructs new |StatusOr| with a value.
  ///
  /// @param value Value.
  StatusOr(ValueType value);

  /// Returns the error status.
  ///
  /// @return Error status.
  Status GetErrorStatus() const;

  /// Returns the value.
  ///
  /// @return Mutable value.
  ValueType& GetValue();

  /// Returns the value.
  ///
  /// @return Value.
  const ValueType& GetValue() const;

  /// Returns whether a value is present or not.
  ///
  /// @return True if a value is present, false otherwise.
  bool IsOk() const;

 private:
  // Value or error status.
  std::variant<Status, ValueType> value_or_;
};

/// BarelyMusician C++ API.
class Api {
 public:
  /// Constructs new |Api|.
  Api();

  /// Constructs new |Api| with an initial sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit Api(int sample_rate);

  /// Destroys |Api|.
  ~Api();

  /// Non-copyable and non-movable.
  Api(const Api& other) = delete;
  Api& operator=(const Api& other) noexcept = delete;
  Api(Api&& other) = delete;
  Api& operator=(Api&& other) noexcept = delete;

  /// Returns the sampling rate.
  ///
  /// @return Sampling rate in Hz, or error status.
  StatusOr<int> GetSampleRate() const;

  /// Sets the sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @return Status.
  Status SetSampleRate(int sample_rate);

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status Update(double timestamp);

 private:
  friend class Instrument;
  friend class Sequence;
  friend class Transport;

  // Internal C API handle.
  BarelyApi capi_;
};

/// Instrument definition.
struct InstrumentDefinition {
  /// Create function signature.
  ///
  /// @param state Pointer to instrument state.
  /// @param sample_rate Sampling rate in Hz.
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

  // Returns the corresponding C type for internal use.
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

  /// Constructs new |Instrument|.
  ///
  /// @param api BarelyMusician C++ API.
  /// @param definition Definition.
  Instrument(const Api& api, InstrumentDefinition definition);

  /// Destroys |Instrument|.
  ~Instrument();

  // TODO(#85): Should |Instrument| be non-movable and non-copyable?

  // TODO(#85): Implement |BarelyInstrument_Clone| (via copy?).

  // TODO(#85): Implement |BarelyInstrument_CancelAllScheduledNoteEvents|.
  // TODO(#85): Implement |BarelyInstrument_CancelScheduledNoteEvent|.

  /// Returns the gain.
  ///
  /// @return Gain in amplitude, or error status.
  StatusOr<float> GetGain() const;

  // TODO(#85): Implement |BarelyInstrument_GetParam|.
  // TODO(#85): Implement |BarelyInstrument_GetParamDefinition|.

  /// Returns whether the volume is muted or not.
  ///
  /// @return True if muted, false otherwise, or error status.
  StatusOr<bool> IsMuted() const;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise, or error status.
  StatusOr<bool> IsNoteOn(float pitch) const;

  /// Processes the next output buffer at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @return Status.
  Status Process(double timestamp, float* output, int num_output_channels,
                 int num_output_frames);

  // TODO(#85): Implement |BarelyInstrument_ScheduleNoteEvent|.

  /// Sets all notes off.
  ///
  /// @return Status.
  Status SetAllNotesOff();

  /// Sets the gain.
  ///
  /// @param gain Gain in amplitude.
  /// @return Status.
  Status SetGain(float gain);

  /// Sets whether the volume should be muted or not.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @return Status.
  Status SetMuted(bool is_muted);

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status SetNoteOff(float pitch);

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  /// @return Status.
  Status SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Status.
  Status SetNoteOn(float pitch, float intensity);

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  /// @return Status.
  Status SetNoteOnCallback(NoteOnCallback note_on_callback);

  // TODO(#85): Implement |BarelyInstrument_SetParam|.
  // TODO(#85): Implement |BarelyInstrument_SetParamToDefault|.

 private:
  // Internal C API handle.
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
  /// Constructs new |Sequence|.
  ///
  /// @param api BarelyMusician C++ API.
  explicit Sequence(const Api& api);

  /// Destroys |Sequence|.
  ~Sequence();

  // TODO(#85): Should |Sequence| be non-movable and non-copyable?

  // TODO(#85): Implement |BarelySequence_Clone| (via copy?).

  // TODO(#85): Implement |BarelySequence_AddNoteEvent|.

  /// Returns the begin offset.
  ///
  /// @return Begin offset in beats, or error status.
  StatusOr<double> GetBeginOffset() const;

  /// Returns the begin position.
  ///
  /// @return Begin position in beats, or error status.
  StatusOr<double> GetBeginPosition() const;

  /// Returns the end position.
  ///
  /// @return End position in beats, or error status.
  StatusOr<double> GetEndPosition() const;

  // TODO(#85): Implement |BarelySequence_GetInstrument|.

  /// Returns the loop begin offset.
  ///
  /// @return Loop begin offset in beats, or error status.
  StatusOr<double> GetLoopBeginOffset() const;

  /// Returns the loop length.
  ///
  /// @return Loop length in beats, or error status.
  StatusOr<double> GetLoopLength() const;

  // TODO(#85): Implement |BarelySequence_GetNoteEventDuration|.
  // TODO(#85): Implement |BarelySequence_GetNoteEventIntensity|.
  // TODO(#85): Implement |BarelySequence_GetNoteEventPitch|.

  /// Returns whether the sequence is empty or not.
  ///
  /// @return True if empty, false otherwise, or error status.
  StatusOr<bool> IsEmpty() const;

  /// Returns whether the sequence should be looping or not.
  ///
  /// @return True if looping, false otherwise, or error status.
  StatusOr<bool> IsLooping() const;

  // TODO(#85): Implement |BarelySequence_RemoveAllNoteEvents|.
  // TODO(#85): Implement |BarelySequence_RemoveNoteEvent|.

  /// Sets the begin offset.
  ///
  /// @param begin_offset Begin offset in beats.
  /// @return Status.
  Status SetBeginOffset(double begin_offset);

  /// Sets the begin position.
  ///
  /// @param begin_position Begin position in beats.
  /// @return Status.
  Status SetBeginPosition(double begin_position);

  /// Sets the end position.
  ///
  /// @param end_position End position in beats.
  /// @return Status.
  Status SetEndPosition(double end_position);

  // TODO(#85): Implement |BarelySequence_SetInstrument|.

  /// Sets the loop begin offset.
  ///
  /// @param loop_begin_offset Loop begin offset in beats.
  /// @return Status.
  Status SetLoopBeginOffset(double loop_begin_offset);

  /// Sets the loop length.
  ///
  /// @param loop_length Loop length in beats.
  /// @return Status.
  Status SetLoopLength(double loop_length);

  /// Sets whether the sequence should be looping or not.
  ///
  /// @param is_looping True if looping, false otherwise.
  /// @return Status.
  Status SetLooping(bool is_looping);

  // TODO(#85): Implement |BarelySequence_SetNoteEventDuration|.
  // TODO(#85): Implement |BarelySequence_SetNoteEventIntensity|.
  // TODO(#85): Implement |BarelySequence_SetNoteEventPitch|.

 private:
  // Internal API handle.
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

  /// Constructs new |Transport|.
  ///
  /// @param api BarelyMusician C++ API.
  explicit Transport(const Api& api);

  // TODO(#85): Should |Transport| be non-movable and non-copyable?

  /// Returns the position.
  ///
  /// @return Position in beats, or error status.
  StatusOr<double> GetPosition() const;

  /// Returns the tempo.
  ///
  /// @return Tempo in BPM, or error status.
  StatusOr<double> GetTempo() const;

  /// Returns whether the transport is playing or not.
  ///
  /// @return True if playing, false otherwise, or error status.
  StatusOr<bool> IsPlaying() const;

  /// Sets the beat callback.
  ///
  /// @param beat_callback Beat callback.
  /// @return Status.
  Status SetBeatCallback(BeatCallback beat_callback);

  /// Sets the position.
  ///
  /// @param position Position in beats.
  /// @return Status.
  Status SetPosition(double position);

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in BPM.
  /// @return Status.
  Status SetTempo(double tempo);

  /// Sets the update callback.
  ///
  /// @param update_callback Update callback.
  /// @return Status.
  Status SetUpdateCallback(UpdateCallback update_callback);

  /// Starts the playback.
  Status Start();

  /// Stops the playback.
  Status Stop();

 private:
  // Internal API handle.
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
ValueType& StatusOr<ValueType>::GetValue() {
  assert(std::holds_alternative<ValueType>(value_or_));
  return std::get<ValueType>(value_or_);
}

template <typename ValueType>
const ValueType& StatusOr<ValueType>::GetValue() const {
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

Status Instrument::SetAllNotesOff() {
  return static_cast<Status>(BarelyInstrument_SetAllNotesOff(capi_, id_));
}

Status Instrument::SetGain(float gain) {
  return static_cast<Status>(BarelyInstrument_SetGain(capi_, id_, gain));
}

Status Instrument::SetMuted(bool is_muted) {
  return static_cast<Status>(BarelyInstrument_SetMuted(capi_, id_, is_muted));
}

Status Instrument::SetNoteOff(float pitch) {
  return static_cast<Status>(BarelyInstrument_SetNoteOff(capi_, id_, pitch));
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

Status Instrument::SetNoteOn(float pitch, float intensity) {
  return static_cast<Status>(
      BarelyInstrument_SetNoteOn(capi_, id_, pitch, intensity));
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
