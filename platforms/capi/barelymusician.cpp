#include "platforms/capi/barelymusician.h"

#include <stdint.h>

#include <any>
#include <optional>
#include <vector>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/param_definition.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::examples::SynthInstrument;
using ::barelyapi::Engine;
using ::barelyapi::GetStatusOrStatus;
using ::barelyapi::GetStatusOrValue;
using ::barelyapi::InstrumentDefinition;
using ::barelyapi::InstrumentState;
using ::barelyapi::IsOk;
using ::barelyapi::Note;
using ::barelyapi::ParamDefinition;
using ::barelyapi::Status;
using ::barelyapi::StatusOr;

// Returns the corresponding |ParamDefinition| for a given |param_definition|.
ParamDefinition GetParamDefinition(
    const BarelyParamDefinition& param_definition) noexcept {
  return ParamDefinition(param_definition.default_value,
                         param_definition.min_value,
                         param_definition.max_value);
}

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
                              InstrumentState* state, int32_t index,
                              float value) noexcept {
      set_param_fn(std::any_cast<BarelyInstrumentState*>(&state), index, value);
    };
  }
  if (definition.num_param_definitions > 0) {
    result.param_definitions.reserve(definition.num_param_definitions);
    for (int32_t i = 0; i < definition.num_param_definitions; ++i) {
      result.param_definitions.emplace_back(
          GetParamDefinition(definition.param_definitions[i]));
    }
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

/// BarelyMusician C api.
struct BarelyMusician {
  /// Constructs new |BarelyMusician|.
  ///
  /// @param sample_rate Sampling rate in hz.
  explicit BarelyMusician(int32_t sample_rate) noexcept
      : instance(sample_rate) {}

  /// Engine instance.
  Engine instance;

 private:
  // Ensure that the instance can only be destroyed via the api call.
  friend BARELY_EXPORT BarelyStatus BarelyDestroyApi(BarelyApi);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyAddPerformer(BarelyApi api, BarelyId* out_performer_id) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_performer_id) return kBarelyStatus_InvalidArgument;
  *out_performer_id = api->instance.AddPerformer();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyAddPerformerNote(BarelyApi api, BarelyId performer_id,
                                    double note_position, double note_duration,
                                    float note_pitch, float note_intensity,
                                    BarelyId* out_note_id) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_note_id) return kBarelyStatus_InvalidArgument;
  const auto note_id_or =
      api->instance.AddPerformerNote(performer_id, note_position,
                                     Note{.pitch = note_pitch,
                                          .intensity = note_intensity,
                                          .duration = note_duration});
  if (IsOk(note_id_or)) {
    *out_note_id = GetStatusOrValue(note_id_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(note_id_or);
}

BarelyApi BarelyCreateApi(int32_t sample_rate) {
  return new BarelyMusician(sample_rate);
}

BarelyStatus BarelyCreateInstrument(BarelyApi api,
                                    BarelyInstrumentDefinition definition,
                                    BarelyId* out_instrument_id) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_instrument_id) return kBarelyStatus_InvalidArgument;
  *out_instrument_id = api->instance.AddInstrument(
      GetInstrumentDefinition(std::move(definition)));
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyCreateSynthInstrument(BarelyApi api,
                                         BarelyId* out_instrument_id) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_instrument_id) return kBarelyStatus_InvalidArgument;
  *out_instrument_id =
      api->instance.AddInstrument(SynthInstrument::GetDefinition());
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyDestroyApi(BarelyApi api) {
  if (api) {
    delete api;
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyDestroyInstrument(BarelyApi api, BarelyId instrument_id) {
  if (api) {
    return GetStatus(api->instance.RemoveInstrument(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyGetPerformerBeginOffset(BarelyApi api, BarelyId performer_id,
                                           double* out_begin_offset) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_begin_offset) return kBarelyStatus_InvalidArgument;
  const auto begin_offset_or =
      api->instance.GetPerformerBeginOffset(performer_id);
  if (IsOk(begin_offset_or)) {
    *out_begin_offset = GetStatusOrValue(begin_offset_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(begin_offset_or);
}

BarelyStatus BarelyGetPerformerBeginPosition(BarelyApi api,
                                             BarelyId performer_id,
                                             double* out_begin_position) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_begin_position) return kBarelyStatus_InvalidArgument;
  const auto begin_position_or =
      api->instance.GetPerformerBeginPosition(performer_id);
  if (IsOk(begin_position_or)) {
    *out_begin_position = GetStatusOrValue(begin_position_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(begin_position_or);
}

BarelyStatus BarelyGetPerformerEndPosition(BarelyApi api, BarelyId performer_id,
                                           double* out_end_position) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_end_position) return kBarelyStatus_InvalidArgument;
  const auto end_position_or =
      api->instance.GetPerformerEndPosition(performer_id);
  if (IsOk(end_position_or)) {
    *out_end_position = GetStatusOrValue(end_position_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(end_position_or);
}

BarelyStatus BarelyGetPerformerLoopBeginOffset(BarelyApi api,
                                               BarelyId performer_id,
                                               double* out_loop_begin_offset) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_loop_begin_offset) return kBarelyStatus_InvalidArgument;
  const auto loop_begin_offset_or =
      api->instance.GetPerformerLoopBeginOffset(performer_id);
  if (IsOk(loop_begin_offset_or)) {
    *out_loop_begin_offset = GetStatusOrValue(loop_begin_offset_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(loop_begin_offset_or);
}

BarelyStatus BarelyGetPerformerLoopLength(BarelyApi api, BarelyId performer_id,
                                          double* out_loop_length) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_loop_length) return kBarelyStatus_InvalidArgument;
  const auto loop_length_or =
      api->instance.GetPerformerLoopLength(performer_id);
  if (IsOk(loop_length_or)) {
    *out_loop_length = GetStatusOrValue(loop_length_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(loop_length_or);
}

BarelyStatus BarelyGetPlaybackPosition(BarelyApi api, double* out_position) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_position) return kBarelyStatus_InvalidArgument;
  *out_position = api->instance.GetPlaybackPosition();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyGetPlaybackTempo(BarelyApi api, double* out_tempo) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_tempo) return kBarelyStatus_InvalidArgument;
  *out_tempo = api->instance.GetPlaybackTempo();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyIsPerformerEmpty(BarelyApi api, BarelyId performer_id,
                                    bool* out_is_empty) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_is_empty) return kBarelyStatus_InvalidArgument;
  const auto is_empty_or = api->instance.IsPerformerEmpty(performer_id);
  if (IsOk(is_empty_or)) {
    *out_is_empty = GetStatusOrValue(is_empty_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(is_empty_or);
}

BarelyStatus BarelyIsPerformerLooping(BarelyApi api, BarelyId performer_id,
                                      bool* out_is_looping) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_is_looping) return kBarelyStatus_InvalidArgument;
  const auto is_looping_or = api->instance.IsPerformerLooping(performer_id);
  if (IsOk(is_looping_or)) {
    *out_is_looping = GetStatusOrValue(is_looping_or);
    return kBarelyStatus_Ok;
  }
  return GetStatus(is_looping_or);
}

BarelyStatus BarelyIsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return kBarelyStatus_NotFound;
  if (!out_is_playing) return kBarelyStatus_InvalidArgument;
  *out_is_playing = api->instance.IsPlaying();
  return kBarelyStatus_Ok;
}

BarelyStatus BarelyProcessInstrument(BarelyApi api, BarelyId instrument_id,
                                     double timestamp, float* output,
                                     int32_t num_channels, int32_t num_frames) {
  if (api) {
    api->instance.ProcessInstrument(instrument_id, timestamp, output,
                                    num_channels, num_frames);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotes(BarelyApi api,
                                           BarelyId performer_id) {
  if (api) {
    return GetStatus(api->instance.RemoveAllPerformerNotes(performer_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotesAt(BarelyApi api,
                                             BarelyId performer_id,
                                             double begin_position,
                                             double end_position) {
  if (api) {
    return GetStatus(api->instance.RemoveAllPerformerNotes(
        performer_id, begin_position, end_position));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemovePerformer(BarelyApi api, BarelyId performer_id) {
  if (api) {
    return GetStatus(api->instance.RemovePerformer(performer_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyRemovePerformerNote(BarelyApi api, BarelyId performer_id,
                                       BarelyId note_id) {
  if (api) {
    return GetStatus(api->instance.RemovePerformerNote(performer_id, note_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetAllInstrumentNotesOff(BarelyApi api,
                                            BarelyId instrument_id) {
  if (api) {
    return GetStatus(api->instance.SetAllInstrumentNotesOff(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetAllInstrumentParamsToDefault(BarelyApi api,
                                                   BarelyId instrument_id) {
  if (api) {
    return GetStatus(
        api->instance.SetAllInstrumentParamsToDefault(instrument_id));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyApi api, BarelyId instrument_id,
                                        float note_pitch) {
  if (api) {
    return GetStatus(
        api->instance.SetInstrumentNoteOff(instrument_id, note_pitch));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyApi api,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback) {
  if (api) {
    if (instrument_note_off_callback) {
      api->instance.SetInstrumentNoteOffCallback(
          [instrument_note_off_callback](auto instrument_id, float note_pitch) {
            instrument_note_off_callback(instrument_id, note_pitch);
          });
    } else {
      api->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float note_pitch, float note_intensity) {
  if (api) {
    return GetStatus(api->instance.SetInstrumentNoteOn(
        instrument_id, note_pitch, note_intensity));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyApi api, BarelyInstrumentNoteOnCallback instrument_note_on_callback) {
  if (api) {
    if (instrument_note_on_callback) {
      api->instance.SetInstrumentNoteOnCallback(
          [instrument_note_on_callback](auto instrument_id, float note_pitch,
                                        float note_intensity) {
            instrument_note_on_callback(instrument_id, note_pitch,
                                        note_intensity);
          });
    } else {
      api->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentParam(BarelyApi api, BarelyId instrument_id,
                                      int32_t param_index, float param_value) {
  if (api) {
    return GetStatus(api->instance.SetInstrumentParam(
        instrument_id, param_index, param_value));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetInstrumentParamToDefault(BarelyApi api,
                                               BarelyId instrument_id,
                                               int32_t param_index) {
  if (api) {
    return GetStatus(
        api->instance.SetInstrumentParamToDefault(instrument_id, param_index));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerBeginOffset(BarelyApi api, BarelyId performer_id,
                                           double begin_offset) {
  if (api) {
    return GetStatus(
        api->instance.SetPerformerBeginOffset(performer_id, begin_offset));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerBeginPosition(BarelyApi api,
                                             BarelyId performer_id,
                                             double* begin_position) {
  if (api) {
    return GetStatus(api->instance.SetPerformerBeginPosition(
        performer_id, begin_position ? std::optional<double>{*begin_position}
                                     : std::nullopt));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerEndPosition(BarelyApi api, BarelyId performer_id,
                                           double* end_position) {
  if (api) {
    return GetStatus(api->instance.SetPerformerEndPosition(
        performer_id,
        end_position ? std::optional<double>{*end_position} : std::nullopt));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerInstrument(BarelyApi api, BarelyId performer_id,
                                          BarelyId instrument_id) {
  if (!api) return kBarelyStatus_NotFound;
  api->instance.RemoveAllPerformerInstruments(performer_id);
  return GetStatus(
      api->instance.AddPerformerInstrument(performer_id, instrument_id));
}

BarelyStatus BarelySetPerformerLoop(BarelyApi api, BarelyId performer_id,
                                    bool loop) {
  if (api) {
    return GetStatus(api->instance.SetPerformerLoop(performer_id, loop));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerLoopBeginOffset(BarelyApi api,
                                               BarelyId performer_id,
                                               double loop_begin_offset) {
  if (api) {
    return GetStatus(api->instance.SetPerformerLoopBeginOffset(
        performer_id, loop_begin_offset));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPerformerLoopLength(BarelyApi api, BarelyId performer_id,
                                          double loop_length) {
  if (api) {
    return GetStatus(
        api->instance.SetPerformerLoopLength(performer_id, loop_length));
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackBeatCallback(
    BarelyApi api, BarelyPlaybackBeatCallback playback_beat_callback) {
  if (api) {
    if (playback_beat_callback) {
      api->instance.SetPlaybackBeatCallback(
          [playback_beat_callback](double position) {
            playback_beat_callback(position);
          });
    } else {
      api->instance.SetPlaybackBeatCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackPosition(BarelyApi api, double position) {
  if (api) {
    api->instance.SetPlaybackPosition(position);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackTempo(BarelyApi api, double tempo) {
  if (api) {
    api->instance.SetPlaybackTempo(tempo);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetPlaybackUpdateCallback(
    BarelyApi api, BarelyPlaybackUpdateCallback playback_update_callback) {
  if (api) {
    if (playback_update_callback) {
      api->instance.SetPlaybackUpdateCallback(
          [playback_update_callback](double begin_position,
                                     double end_position) {
            playback_update_callback(begin_position, end_position);
          });
    } else {
      api->instance.SetPlaybackUpdateCallback(nullptr);
    }
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyStartPlayback(BarelyApi api) {
  if (api) {
    api->instance.StartPlayback();
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyStopPlayback(BarelyApi api) {
  if (api) {
    api->instance.StopPlayback();
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelySetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (api) {
    api->instance.SetSampleRate(sample_rate);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

BarelyStatus BarelyUpdate(BarelyApi api, double timestamp) {
  if (api) {
    api->instance.Update(timestamp);
    return kBarelyStatus_Ok;
  }
  return kBarelyStatus_NotFound;
}

}  // extern "C"
