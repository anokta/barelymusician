#include "platforms/capi/barelymusician.h"

#include <stdint.h>

#include <any>
#include <optional>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/musician.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::GetStatusOrStatus;
using ::barely::GetStatusOrValue;
using ::barely::InstrumentDefinition;
using ::barely::InstrumentState;
using ::barely::IsOk;
using ::barely::Musician;
using ::barely::Note;
using ::barely::ParamDefinition;
using ::barely::ParamDefinitionMap;
using ::barely::Status;
using ::barely::StatusOr;
using ::barely::examples::SynthInstrument;

// Returns the corresponding |InstrumentDefinition| for a given |definition|.
InstrumentDefinition GetInstrumentDefinition(
    BarelyInstrumentDefinition definition) noexcept {
  InstrumentDefinition result;
  if (definition.create_fn) {
    result.create_fn = [create_fn = std::move(definition.create_fn)](
                           InstrumentState* state, int sample_rate) noexcept {
      create_fn(&state->emplace<BarelyInstrumentState>(), sample_rate);
    };
  }
  if (definition.destroy_fn) {
    result.destroy_fn = [destroy_fn = std::move(definition.destroy_fn)](
                            InstrumentState* state) noexcept {
      destroy_fn(std::any_cast<BarelyInstrumentState*>(&state));
      state->reset();
    };
  }
  if (definition.process_fn) {
    result.process_fn = [process_fn = std::move(definition.process_fn)](
                            InstrumentState* state, float* output,
                            int num_channels, int num_frames) noexcept {
      process_fn(std::any_cast<BarelyInstrumentState*>(&state), output,
                 num_channels, num_frames);
    };
  }
  if (definition.set_custom_data_fn) {
    result.set_custom_data_fn =
        [set_custom_data_fn = std::move(definition.set_custom_data_fn)](
            InstrumentState* state, std::any custom_data) {
          set_custom_data_fn(std::any_cast<BarelyInstrumentState*>(&state),
                             std::any_cast<void*>(std::move(custom_data)));
        };
  }
  if (definition.set_note_off_fn) {
    result.set_note_off_fn = [set_note_off_fn =
                                  std::move(definition.set_note_off_fn)](
                                 InstrumentState* state, float pitch) noexcept {
      set_note_off_fn(std::any_cast<BarelyInstrumentState*>(&state), pitch);
    };
  }
  if (definition.set_note_on_fn) {
    result.set_note_on_fn =
        [set_note_on_fn = std::move(definition.set_note_on_fn)](
            InstrumentState* state, float pitch, float intensity) noexcept {
          set_note_on_fn(std::any_cast<BarelyInstrumentState*>(&state), pitch,
                         intensity);
        };
  }
  if (definition.set_param_fn) {
    result.set_param_fn = [set_param_fn = std::move(definition.set_param_fn)](
                              InstrumentState* state, BarelyParamId id,
                              float value) noexcept {
      set_param_fn(std::any_cast<BarelyInstrumentState*>(&state), id, value);
    };
  }
  return result;
}

// Returns the corresponding |std::optional<float>| for a given |value_or|.
std::optional<float> GetOptionalFloat(const BarelyOptionalFloat& value_or) {
  return value_or.has_value ? std::optional<float>(value_or.value)
                            : std::nullopt;
}

// Returns the corresponding |ParamDefinition| for a given |param_definition|.
ParamDefinition GetParamDefinition(
    const BarelyParamDefinition& param_definition) noexcept {
  return ParamDefinition(param_definition.default_value,
                         GetOptionalFloat(param_definition.min_value),
                         GetOptionalFloat(param_definition.max_value));
}

// Returns the corresponding |ParamDefinitionMap| for given |param_definitions|.
ParamDefinitionMap GetParamDefinitions(
    BarelyParamDefinitionMap param_definitions) noexcept {
  ParamDefinitionMap result;
  for (int32_t i = 0; i < param_definitions.num_definitions; ++i) {
    const auto& param_definition = param_definitions.definitions[i];
    result.emplace(static_cast<int>(param_definition.id),
                   GetParamDefinition(param_definition.definition));
  }
  return result;
}

// Returns the corresponding |BarelyStatus| value for a given |status|.
BarelyStatus GetStatus(Status status) noexcept {
  switch (status) {
    case Status::kOk:
      return kBarelyStatus_Ok;
    case Status::kInvalidArgument:
      return kBarelyStatus_InvalidArgument;
    case Status::kNotFound:
      return kBarelyStatus_NotFound;
    case Status::kAlreadyExists:
      return kBarelyStatus_AlreadyExists;
    case Status::kFailedPrecondition:
      return kBarelyStatus_FailedPrecondition;
    case Status::kUnimplemented:
      return kBarelyStatus_Unimplemented;
    case Status::kInternal:
      return kBarelyStatus_Internal;
    case Status::kUnknown:
    default:
      return kBarelyStatus_Unknown;
  }
}

// Returns the corresponding |BarelyStatus| value for a given |status_or|.
template <typename ValueType>
BarelyStatus GetStatus(const StatusOr<ValueType>& status_or) noexcept {
  return GetStatus(GetStatusOrStatus(status_or));
}

}  // namespace

extern "C" {

/// BarelyMusician C API.
struct BarelyMusician {
  /// Constructs new |BarelyMusician|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit BarelyMusician(int32_t sample_rate) noexcept
      : instance(sample_rate) {}

  /// BarelyMusician instance.
  Musician instance;
};

BarelyStatus BarelyAddInstrument(BarelyHandle handle,
                                 BarelyInstrumentDefinition definition,
                                 BarelyParamDefinitionMap param_definitions,
                                 BarelyId* instrument_id_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!instrument_id_ptr) return kBarelyStatus_InvalidArgument;
  *instrument_id_ptr = handle->instance.AddInstrument(
      GetInstrumentDefinition(std::move(definition)),
      GetParamDefinitions(std::move(param_definitions)));
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyAddSynthInstrument(BarelyHandle handle,
                                      BarelyId* instrument_id_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!instrument_id_ptr) return kBarelyStatus_InvalidArgument;
  *instrument_id_ptr = handle->instance.AddInstrument(
      SynthInstrument::GetDefinition(), SynthInstrument::GetParamDefinitions());
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyAddPerformer(BarelyHandle handle,
                                BarelyId* performer_id_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!performer_id_ptr) return kBarelyStatus_InvalidArgument;
  *performer_id_ptr = handle->instance.AddPerformer();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyAddPerformerInstrument(BarelyHandle handle,
                                          BarelyId performer_id,
                                          BarelyId instrument_id) {
  if (!handle) return kBarelyStatus_NotFound;
  return GetStatus(
      handle->instance.AddPerformerInstrument(performer_id, instrument_id));
}

BarelyStatus BarelyAddPerformerNote(BarelyHandle handle, BarelyId performer_id,
                                    double note_position, double note_duration,
                                    float note_pitch, float note_intensity,
                                    BarelyId* note_id_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!note_id_ptr) return kBarelyStatus_InvalidArgument;
  const auto note_id_or =
      handle->instance.AddPerformerNote(performer_id, note_position,
                                        Note{.pitch = note_pitch,
                                             .intensity = note_intensity,
                                             .duration = note_duration});
  if (IsOk(note_id_or)) {
    *note_id_ptr = GetStatusOrValue(note_id_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(note_id_or);
}

BarelyHandle BarelyCreate(int32_t sample_rate) {
  return new BarelyMusician(sample_rate);
}

BarelyStatus BarelyDestroy(BarelyHandle handle) {
  if (handle) {
    delete handle;
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyGetPerformerBeginOffset(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* begin_offset_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!begin_offset_ptr) return kBarelyStatus_InvalidArgument;
  const auto begin_offset_or =
      handle->instance.GetPerformerBeginOffset(performer_id);
  if (IsOk(begin_offset_or)) {
    *begin_offset_ptr = GetStatusOrValue(begin_offset_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(begin_offset_or);
}

BarelyStatus BarelyGetPerformerBeginPosition(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double* begin_position_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!begin_position_ptr) return kBarelyStatus_InvalidArgument;
  const auto begin_position_or =
      handle->instance.GetPerformerBeginPosition(performer_id);
  if (IsOk(begin_position_or)) {
    *begin_position_ptr = GetStatusOrValue(begin_position_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(begin_position_or);
}

BarelyStatus BarelyGetPerformerEndPosition(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* end_position_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!end_position_ptr) return kBarelyStatus_InvalidArgument;
  const auto end_position_or =
      handle->instance.GetPerformerEndPosition(performer_id);
  if (IsOk(end_position_or)) {
    *end_position_ptr = GetStatusOrValue(end_position_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(end_position_or);
}

BarelyStatus BarelyGetPerformerLoopBeginOffset(BarelyHandle handle,
                                               BarelyId performer_id,
                                               double* loop_begin_offset_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!loop_begin_offset_ptr) return kBarelyStatus_InvalidArgument;
  const auto loop_begin_offset_or =
      handle->instance.GetPerformerLoopBeginOffset(performer_id);
  if (IsOk(loop_begin_offset_or)) {
    *loop_begin_offset_ptr = GetStatusOrValue(loop_begin_offset_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(loop_begin_offset_or);
}

BarelyStatus BarelyGetPerformerLoopLength(BarelyHandle handle,
                                          BarelyId performer_id,
                                          double* loop_length_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!loop_length_ptr) return kBarelyStatus_InvalidArgument;
  const auto loop_length_or =
      handle->instance.GetPerformerLoopLength(performer_id);
  if (IsOk(loop_length_or)) {
    *loop_length_ptr = GetStatusOrValue(loop_length_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(loop_length_or);
}

BarelyStatus BarelyGetPlaybackPosition(BarelyHandle handle,
                                       double* position_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!position_ptr) return kBarelyStatus_InvalidArgument;
  *position_ptr = handle->instance.GetPlaybackPosition();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyGetPlaybackTempo(BarelyHandle handle, double* tempo_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!tempo_ptr) return kBarelyStatus_InvalidArgument;
  *tempo_ptr = handle->instance.GetPlaybackTempo();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyIsPerformerEmpty(BarelyHandle handle, BarelyId performer_id,
                                    bool* is_empty_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!is_empty_ptr) return kBarelyStatus_InvalidArgument;
  const auto is_empty_or = handle->instance.IsPerformerEmpty(performer_id);
  if (IsOk(is_empty_or)) {
    *is_empty_ptr = GetStatusOrValue(is_empty_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(is_empty_or);
}

BarelyStatus BarelyIsPerformerLooping(BarelyHandle handle,
                                      BarelyId performer_id,
                                      bool* is_looping_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!is_looping_ptr) return kBarelyStatus_InvalidArgument;
  const auto is_looping_or = handle->instance.IsPerformerLooping(performer_id);
  if (IsOk(is_looping_or)) {
    *is_looping_ptr = GetStatusOrValue(is_looping_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(is_looping_or);
}

BarelyStatus BarelyIsPlaying(BarelyHandle handle, bool* is_playing_ptr) {
  if (!handle) return kBarelyStatus_NotFound;
  if (!is_playing_ptr) return kBarelyStatus_InvalidArgument;
  *is_playing_ptr = handle->instance.IsPlaying();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyProcessInstrument(BarelyHandle handle,
                                     BarelyId instrument_id, double timestamp,
                                     float* output, int32_t num_channels,
                                     int32_t num_frames) {
  if (handle) {
    handle->instance.ProcessInstrument(instrument_id, timestamp, output,
                                       num_channels, num_frames);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveAllPerformerInstruments(BarelyHandle handle,
                                                 BarelyId performer_id) {
  if (handle) {
    return GetStatus(
        handle->instance.RemoveAllPerformerInstruments(performer_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotes(BarelyHandle handle,
                                           BarelyId performer_id) {
  if (handle) {
    return GetStatus(handle->instance.RemoveAllPerformerNotes(performer_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotesAt(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double begin_position,
                                             double end_position) {
  if (handle) {
    return GetStatus(handle->instance.RemoveAllPerformerNotes(
        performer_id, begin_position, end_position));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveInstrument(BarelyHandle handle,
                                    BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.RemoveInstrument(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemovePerformer(BarelyHandle handle, BarelyId performer_id) {
  if (handle) {
    return GetStatus(handle->instance.RemovePerformer(performer_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemovePerformerInstrument(BarelyHandle handle,
                                             BarelyId performer_id,
                                             BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.RemovePerformerInstrument(performer_id,
                                                                instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemovePerformerNote(BarelyHandle handle,
                                       BarelyId performer_id,
                                       BarelyId note_id) {
  if (handle) {
    return GetStatus(
        handle->instance.RemovePerformerNote(performer_id, note_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetAllInstrumentNotesOff(BarelyHandle handle,
                                            BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.SetAllInstrumentNotesOff(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetAllInstrumentParamsToDefault(BarelyHandle handle,
                                                   BarelyId instrument_id) {
  if (handle) {
    return GetStatus(
        handle->instance.SetAllInstrumentParamsToDefault(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyId instrument_id,
                                        float note_pitch) {
  if (handle) {
    return GetStatus(
        handle->instance.SetInstrumentNoteOff(instrument_id, note_pitch));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback) {
  if (handle) {
    if (instrument_note_off_callback) {
      handle->instance.SetInstrumentNoteOffCallback(
          [instrument_note_off_callback](auto instrument_id, double timestamp,
                                         float note_pitch) {
            instrument_note_off_callback(instrument_id, timestamp, note_pitch);
          });
    } else {
      handle->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyId instrument_id, float note_pitch,
                                       float note_intensity) {
  if (handle) {
    return GetStatus(handle->instance.SetInstrumentNoteOn(
        instrument_id, note_pitch, note_intensity));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOnCallback instrument_note_on_callback) {
  if (handle) {
    if (instrument_note_on_callback) {
      handle->instance.SetInstrumentNoteOnCallback(
          [instrument_note_on_callback](auto instrument_id, double timestamp,
                                        float note_pitch,
                                        float note_intensity) {
            instrument_note_on_callback(instrument_id, timestamp, note_pitch,
                                        note_intensity);
          });
    } else {
      handle->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentParam(BarelyHandle handle,
                                      BarelyId instrument_id,
                                      BarelyParamId param_id,
                                      float param_value) {
  if (handle) {
    return GetStatus(handle->instance.SetInstrumentParam(
        instrument_id, param_id, param_value));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentParamToDefault(BarelyHandle handle,
                                               BarelyId instrument_id,
                                               BarelyParamId param_id) {
  if (handle) {
    return GetStatus(
        handle->instance.SetInstrumentParamToDefault(instrument_id, param_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerBeginOffset(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double begin_offset) {
  if (handle) {
    return GetStatus(
        handle->instance.SetPerformerBeginOffset(performer_id, begin_offset));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerBeginPosition(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double* begin_position) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerBeginPosition(
        performer_id, begin_position ? std::optional<double>{*begin_position}
                                     : std::nullopt));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerEndPosition(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* end_position) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerEndPosition(
        performer_id,
        end_position ? std::optional<double>{*end_position} : std::nullopt));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerLoop(BarelyHandle handle, BarelyId performer_id,
                                    bool loop) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerLoop(performer_id, loop));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerLoopBeginOffset(BarelyHandle handle,
                                               BarelyId performer_id,
                                               double loop_begin_offset) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerLoopBeginOffset(
        performer_id, loop_begin_offset));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerLoopLength(BarelyHandle handle,
                                          BarelyId performer_id,
                                          double loop_length) {
  if (handle) {
    return GetStatus(
        handle->instance.SetPerformerLoopLength(performer_id, loop_length));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyHandle handle, BarelyPlaybackBeatCallback playback_beat_callback) {
  if (handle) {
    if (playback_beat_callback) {
      handle->instance.SetPlaybackBeatCallback(
          [playback_beat_callback](double position) {
            playback_beat_callback(position);
          });
    } else {
      handle->instance.SetPlaybackBeatCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle, double position) {
  if (handle) {
    handle->instance.SetPlaybackPosition(position);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo) {
  if (handle) {
    handle->instance.SetPlaybackTempo(tempo);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyHandle handle,
    BarelyPlaybackUpdateCallback playback_update_callback) {
  if (handle) {
    if (playback_update_callback) {
      handle->instance.SetPlaybackUpdateCallback(
          [playback_update_callback](double begin_position,
                                     double end_position) {
            playback_update_callback(begin_position, end_position);
          });
    } else {
      handle->instance.SetPlaybackUpdateCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyStartPlayback(BarelyHandle handle) {
  if (handle) {
    handle->instance.StartPlayback();
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyStopPlayback(BarelyHandle handle) {
  if (handle) {
    handle->instance.StopPlayback();
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetSampleRate(BarelyHandle handle, int32_t sample_rate) {
  if (handle) {
    handle->instance.SetSampleRate(sample_rate);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyUpdate(BarelyHandle handle, double timestamp) {
  if (handle) {
    handle->instance.Update(timestamp);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

}  // extern "C"
