#include "platforms/capi/barelymusician.h"

#include <stdint.h>

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
  std::vector<ParamDefinition> param_definitions;
  param_definitions.reserve(definition.num_param_definitions);
  for (int i = 0; i < definition.num_param_definitions; ++i) {
    const auto& param_definition = definition.param_definitions[i];
    param_definitions.emplace_back(param_definition.default_value,
                                   param_definition.min_value,
                                   param_definition.max_value);
  }
  return InstrumentDefinition{std::move(definition.create_fn),
                              std::move(definition.destroy_fn),
                              std::move(definition.process_fn),
                              std::move(definition.set_data_fn),
                              std::move(definition.set_note_off_fn),
                              std::move(definition.set_note_on_fn),
                              std::move(definition.set_param_fn),
                              std::move(param_definitions)};
}

// Returns the corresponding |BarelyStatus| value for a given |status|.
BarelyStatus GetStatus(Status status) noexcept {
  switch (status) {
    case Status::kOk:
      return BarelyStatus_kOk;
    case Status::kInvalidArgument:
      return BarelyStatus_kInvalidArgument;
    case Status::kNotFound:
      return BarelyStatus_kNotFound;
    case Status::kAlreadyExists:
      return BarelyStatus_kAlreadyExists;
    case Status::kFailedPrecondition:
      return BarelyStatus_kFailedPrecondition;
    case Status::kUnimplemented:
      return BarelyStatus_kUnimplemented;
    case Status::kInternal:
      return BarelyStatus_kInternal;
    case Status::kUnknown:
    default:
      return BarelyStatus_kUnknown;
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
  if (!api) return BarelyStatus_kNotFound;
  if (!out_performer_id) return BarelyStatus_kInvalidArgument;
  *out_performer_id = api->instance.AddPerformer();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyAddPerformerNote(BarelyApi api, BarelyId performer_id,
                                    double note_position, double note_duration,
                                    float note_pitch, float note_intensity,
                                    BarelyId* out_note_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;
  const auto note_id_or =
      api->instance.AddPerformerNote(performer_id, note_position,
                                     Note{.pitch = note_pitch,
                                          .intensity = note_intensity,
                                          .duration = note_duration});
  if (IsOk(note_id_or)) {
    *out_note_id = GetStatusOrValue(note_id_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(note_id_or);
}

BarelyApi BarelyCreateApi(int32_t sample_rate) {
  return new BarelyMusician(sample_rate);
}

BarelyStatus BarelyCreateInstrument(BarelyApi api,
                                    BarelyInstrumentDefinition definition,
                                    BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;
  *out_instrument_id = api->instance.AddInstrument(
      GetInstrumentDefinition(std::move(definition)));
  return BarelyStatus_kOk;
}

BarelyStatus BarelyCreateSynthInstrument(BarelyApi api,
                                         BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;
  *out_instrument_id =
      api->instance.AddInstrument(SynthInstrument::GetDefinition());
  return BarelyStatus_kOk;
}

BarelyStatus BarelyDestroyApi(BarelyApi api) {
  if (api) {
    delete api;
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyDestroyInstrument(BarelyApi api, BarelyId instrument_id) {
  if (api) {
    return GetStatus(api->instance.RemoveInstrument(instrument_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyGetPerformerBeginOffset(BarelyApi api, BarelyId performer_id,
                                           double* out_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;
  const auto begin_offset_or =
      api->instance.GetPerformerBeginOffset(performer_id);
  if (IsOk(begin_offset_or)) {
    *out_begin_offset = GetStatusOrValue(begin_offset_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(begin_offset_or);
}

BarelyStatus BarelyGetPerformerBeginPosition(BarelyApi api,
                                             BarelyId performer_id,
                                             double* out_begin_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;
  const auto begin_position_or =
      api->instance.GetPerformerBeginPosition(performer_id);
  if (IsOk(begin_position_or)) {
    *out_begin_position = GetStatusOrValue(begin_position_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(begin_position_or);
}

BarelyStatus BarelyGetPerformerEndPosition(BarelyApi api, BarelyId performer_id,
                                           double* out_end_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;
  const auto end_position_or =
      api->instance.GetPerformerEndPosition(performer_id);
  if (IsOk(end_position_or)) {
    *out_end_position = GetStatusOrValue(end_position_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(end_position_or);
}

BarelyStatus BarelyGetPerformerLoopBeginOffset(BarelyApi api,
                                               BarelyId performer_id,
                                               double* out_loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;
  const auto loop_begin_offset_or =
      api->instance.GetPerformerLoopBeginOffset(performer_id);
  if (IsOk(loop_begin_offset_or)) {
    *out_loop_begin_offset = GetStatusOrValue(loop_begin_offset_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(loop_begin_offset_or);
}

BarelyStatus BarelyGetPerformerLoopLength(BarelyApi api, BarelyId performer_id,
                                          double* out_loop_length) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;
  const auto loop_length_or =
      api->instance.GetPerformerLoopLength(performer_id);
  if (IsOk(loop_length_or)) {
    *out_loop_length = GetStatusOrValue(loop_length_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(loop_length_or);
}

BarelyStatus BarelyGetPlaybackPosition(BarelyApi api, double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;
  *out_position = api->instance.GetPlaybackPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyGetPlaybackTempo(BarelyApi api, double* out_tempo) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;
  *out_tempo = api->instance.GetPlaybackTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyIsPerformerEmpty(BarelyApi api, BarelyId performer_id,
                                    bool* out_is_empty) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;
  const auto is_empty_or = api->instance.IsPerformerEmpty(performer_id);
  if (IsOk(is_empty_or)) {
    *out_is_empty = GetStatusOrValue(is_empty_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_empty_or);
}

BarelyStatus BarelyIsPerformerLooping(BarelyApi api, BarelyId performer_id,
                                      bool* out_is_looping) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;
  const auto is_looping_or = api->instance.IsPerformerLooping(performer_id);
  if (IsOk(is_looping_or)) {
    *out_is_looping = GetStatusOrValue(is_looping_or);
    return BarelyStatus_kOk;
  }
  return GetStatus(is_looping_or);
}

BarelyStatus BarelyIsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;
  *out_is_playing = api->instance.IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyProcessInstrument(BarelyApi api, BarelyId instrument_id,
                                     double timestamp, float* output,
                                     int32_t num_channels, int32_t num_frames) {
  if (api) {
    api->instance.ProcessInstrument(instrument_id, timestamp, output,
                                    num_channels, num_frames);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotes(BarelyApi api,
                                           BarelyId performer_id) {
  if (api) {
    return GetStatus(api->instance.RemoveAllPerformerNotes(performer_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotesAt(BarelyApi api,
                                             BarelyId performer_id,
                                             double begin_position,
                                             double end_position) {
  if (api) {
    return GetStatus(api->instance.RemoveAllPerformerNotes(
        performer_id, begin_position, end_position));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyRemovePerformer(BarelyApi api, BarelyId performer_id) {
  if (api) {
    return GetStatus(api->instance.RemovePerformer(performer_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyRemovePerformerNote(BarelyApi api, BarelyId performer_id,
                                       BarelyId note_id) {
  if (api) {
    return GetStatus(api->instance.RemovePerformerNote(performer_id, note_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetAllInstrumentNotesOff(BarelyApi api,
                                            BarelyId instrument_id) {
  if (api) {
    return GetStatus(api->instance.SetAllInstrumentNotesOff(instrument_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetAllInstrumentParamsToDefault(BarelyApi api,
                                                   BarelyId instrument_id) {
  if (api) {
    return GetStatus(
        api->instance.SetAllInstrumentParamsToDefault(instrument_id));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyApi api, BarelyId instrument_id,
                                        float note_pitch) {
  if (api) {
    return GetStatus(
        api->instance.SetInstrumentNoteOff(instrument_id, note_pitch));
  }
  return BarelyStatus_kNotFound;
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
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float note_pitch, float note_intensity) {
  if (api) {
    return GetStatus(api->instance.SetInstrumentNoteOn(
        instrument_id, note_pitch, note_intensity));
  }
  return BarelyStatus_kNotFound;
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
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetInstrumentParam(BarelyApi api, BarelyId instrument_id,
                                      int32_t param_index, float param_value) {
  if (api) {
    return GetStatus(api->instance.SetInstrumentParam(
        instrument_id, param_index, param_value));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetInstrumentParamToDefault(BarelyApi api,
                                               BarelyId instrument_id,
                                               int32_t param_index) {
  if (api) {
    return GetStatus(
        api->instance.SetInstrumentParamToDefault(instrument_id, param_index));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerBeginOffset(BarelyApi api, BarelyId performer_id,
                                           double begin_offset) {
  if (api) {
    return GetStatus(
        api->instance.SetPerformerBeginOffset(performer_id, begin_offset));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerBeginPosition(BarelyApi api,
                                             BarelyId performer_id,
                                             double* begin_position) {
  if (api) {
    return GetStatus(api->instance.SetPerformerBeginPosition(
        performer_id, begin_position ? std::optional<double>{*begin_position}
                                     : std::nullopt));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerEndPosition(BarelyApi api, BarelyId performer_id,
                                           double* end_position) {
  if (api) {
    return GetStatus(api->instance.SetPerformerEndPosition(
        performer_id,
        end_position ? std::optional<double>{*end_position} : std::nullopt));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerInstrument(BarelyApi api, BarelyId performer_id,
                                          BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  api->instance.RemoveAllPerformerInstruments(performer_id);
  return GetStatus(
      api->instance.AddPerformerInstrument(performer_id, instrument_id));
}

BarelyStatus BarelySetPerformerLoop(BarelyApi api, BarelyId performer_id,
                                    bool loop) {
  if (api) {
    return GetStatus(api->instance.SetPerformerLoop(performer_id, loop));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerLoopBeginOffset(BarelyApi api,
                                               BarelyId performer_id,
                                               double loop_begin_offset) {
  if (api) {
    return GetStatus(api->instance.SetPerformerLoopBeginOffset(
        performer_id, loop_begin_offset));
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPerformerLoopLength(BarelyApi api, BarelyId performer_id,
                                          double loop_length) {
  if (api) {
    return GetStatus(
        api->instance.SetPerformerLoopLength(performer_id, loop_length));
  }
  return BarelyStatus_kNotFound;
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
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPlaybackPosition(BarelyApi api, double position) {
  if (api) {
    api->instance.SetPlaybackPosition(position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetPlaybackTempo(BarelyApi api, double tempo) {
  if (api) {
    api->instance.SetPlaybackTempo(tempo);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
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
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyStartPlayback(BarelyApi api) {
  if (api) {
    api->instance.StartPlayback();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyStopPlayback(BarelyApi api) {
  if (api) {
    api->instance.StopPlayback();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySetSampleRate(BarelyApi api, int32_t sample_rate) {
  if (api) {
    api->instance.SetSampleRate(sample_rate);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyUpdate(BarelyApi api, double timestamp) {
  if (api) {
    api->instance.Update(timestamp);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

}  // extern "C"
