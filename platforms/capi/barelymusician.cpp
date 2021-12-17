#include "platforms/capi/barelymusician.h"

#include <stdint.h>

#include <any>
#include <optional>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/musician.h"
#include "examples/instruments/synth_instrument.h"

namespace {

using ::barely::GetStatusOrStatus;
using ::barely::GetStatusOrValue;
using ::barely::InstrumentDefinition;
using ::barely::InstrumentState;
using ::barely::IsOk;
using ::barely::Musician;
using ::barely::Note;
using ::barely::Status;
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
                              InstrumentState* state, int id,
                              float value) noexcept {
      set_param_fn(std::any_cast<BarelyInstrumentState*>(&state), id, value);
    };
  }
  return result;
}

// Returns the corresponding |BarelyStatus| value for a given |status|.
BarelyStatus GetStatus(Status status) noexcept {
  switch (status) {
    case Status::kOk:
      return kBarelyOk;
    case Status::kInvalidArgument:
      return kBarelyInvalidArgument;
    case Status::kNotFound:
      return kBarelyNotFound;
    case Status::kAlreadyExists:
      return kBarelyAlreadyExists;
    case Status::kFailedPrecondition:
      return kBarelyFailedPrecondition;
    case Status::kUnimplemented:
      return kBarelyUnimplemented;
    case Status::kInternal:
      return kBarelyInternal;
    case Status::kUnknown:
    default:
      return kBarelyUnknown;
  }
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
                                 BarelyId* instrument_id_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!instrument_id_ptr) return kBarelyInvalidArgument;
  *instrument_id_ptr = handle->instance.AddInstrument(
      GetInstrumentDefinition(std::move(definition)),
      SynthInstrument::GetParamDefinitions());
  return kBarelyOk;
}

BarelyStatus BarelyAddSynthInstrument(BarelyHandle handle,
                                      BarelyId* instrument_id_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!instrument_id_ptr) return kBarelyInvalidArgument;
  *instrument_id_ptr = handle->instance.AddInstrument(
      SynthInstrument::GetDefinition(), SynthInstrument::GetParamDefinitions());
  return kBarelyOk;
}

BarelyStatus BarelyAddPerformer(BarelyHandle handle,
                                BarelyId* performer_id_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!performer_id_ptr) return kBarelyInvalidArgument;
  *performer_id_ptr = handle->instance.AddPerformer();
  return kBarelyOk;
}

BarelyStatus BarelyAddPerformerInstrument(BarelyHandle handle,
                                          BarelyId performer_id,
                                          BarelyId instrument_id) {
  if (!handle) return kBarelyNotFound;
  return GetStatus(
      handle->instance.AddPerformerInstrument(performer_id, instrument_id));
}

BarelyStatus BarelyAddPerformerNote(BarelyHandle handle, BarelyId performer_id,
                                    double note_position, double note_duration,
                                    float note_pitch, float note_intensity,
                                    BarelyId* note_id_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!note_id_ptr) return kBarelyInvalidArgument;
  const auto note_id_or =
      handle->instance.AddPerformerNote(performer_id, note_position,
                                        Note{.pitch = note_pitch,
                                             .intensity = note_intensity,
                                             .duration = note_duration});
  if (IsOk(note_id_or)) {
    *note_id_ptr = GetStatusOrValue(note_id_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(note_id_or));
}

BarelyHandle BarelyCreate(int32_t sample_rate) {
  return new BarelyMusician(sample_rate);
}

BarelyStatus BarelyDestroy(BarelyHandle handle) {
  if (handle) {
    delete handle;
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyGetPerformerBeginOffset(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* begin_offset_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!begin_offset_ptr) return kBarelyInvalidArgument;
  const auto begin_offset_or =
      handle->instance.GetPerformerBeginOffset(performer_id);
  if (IsOk(begin_offset_or)) {
    *begin_offset_ptr = GetStatusOrValue(begin_offset_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(begin_offset_or));
}

BarelyStatus BarelyGetPerformerBeginPosition(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double* begin_position_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!begin_position_ptr) return kBarelyInvalidArgument;
  const auto begin_position_or =
      handle->instance.GetPerformerBeginPosition(performer_id);
  if (IsOk(begin_position_or)) {
    *begin_position_ptr = GetStatusOrValue(begin_position_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(begin_position_or));
}

BarelyStatus BarelyGetPerformerEndPosition(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* end_position_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!end_position_ptr) return kBarelyInvalidArgument;
  const auto end_position_or =
      handle->instance.GetPerformerEndPosition(performer_id);
  if (IsOk(end_position_or)) {
    *end_position_ptr = GetStatusOrValue(end_position_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(end_position_or));
}

BarelyStatus BarelyGetPerformerLoopBeginOffset(BarelyHandle handle,
                                               BarelyId performer_id,
                                               double* loop_begin_offset_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!loop_begin_offset_ptr) return kBarelyInvalidArgument;
  const auto loop_begin_offset_or =
      handle->instance.GetPerformerLoopBeginOffset(performer_id);
  if (IsOk(loop_begin_offset_or)) {
    *loop_begin_offset_ptr = GetStatusOrValue(loop_begin_offset_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(loop_begin_offset_or));
}

BarelyStatus BarelyGetPerformerLoopLength(BarelyHandle handle,
                                          BarelyId performer_id,
                                          double* loop_length_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!loop_length_ptr) return kBarelyInvalidArgument;
  const auto loop_length_or =
      handle->instance.GetPerformerLoopLength(performer_id);
  if (IsOk(loop_length_or)) {
    *loop_length_ptr = GetStatusOrValue(loop_length_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(loop_length_or));
}

BarelyStatus BarelyGetPlaybackPosition(BarelyHandle handle,
                                       double* position_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!position_ptr) return kBarelyInvalidArgument;
  *position_ptr = handle->instance.GetPlaybackPosition();
  return kBarelyOk;
}

BarelyStatus BarelyGetPlaybackTempo(BarelyHandle handle, double* tempo_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!tempo_ptr) return kBarelyInvalidArgument;
  *tempo_ptr = handle->instance.GetPlaybackTempo();
  return kBarelyOk;
}

BarelyStatus BarelyIsPerformerEmpty(BarelyHandle handle, BarelyId performer_id,
                                    bool* is_empty_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!is_empty_ptr) return kBarelyInvalidArgument;
  const auto is_empty_or = handle->instance.IsPerformerEmpty(performer_id);
  if (IsOk(is_empty_or)) {
    *is_empty_ptr = GetStatusOrValue(is_empty_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(is_empty_or));
}

BarelyStatus BarelyIsPerformerLooping(BarelyHandle handle,
                                      BarelyId performer_id,
                                      bool* is_looping_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!is_looping_ptr) return kBarelyInvalidArgument;
  const auto is_looping_or = handle->instance.IsPerformerLooping(performer_id);
  if (IsOk(is_looping_or)) {
    *is_looping_ptr = GetStatusOrValue(is_looping_or);
    return kBarelyOk;
  }
  return GetStatus(GetStatusOrStatus(is_looping_or));
}

BarelyStatus BarelyIsPlaying(BarelyHandle handle, bool* is_playing_ptr) {
  if (!handle) return kBarelyNotFound;
  if (!is_playing_ptr) return kBarelyInvalidArgument;
  *is_playing_ptr = handle->instance.IsPlaying();
  return kBarelyOk;
}

BarelyStatus BarelyProcessInstrument(BarelyHandle handle,
                                     BarelyId instrument_id, double timestamp,
                                     float* output, int32_t num_channels,
                                     int32_t num_frames) {
  if (handle) {
    handle->instance.ProcessInstrument(instrument_id, timestamp, output,
                                       num_channels, num_frames);
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemoveAllPerformerInstruments(BarelyHandle handle,
                                                 BarelyId performer_id) {
  if (handle) {
    return GetStatus(
        handle->instance.RemoveAllPerformerInstruments(performer_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotes(BarelyHandle handle,
                                           BarelyId performer_id) {
  if (handle) {
    return GetStatus(handle->instance.RemoveAllPerformerNotes(performer_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemoveAllPerformerNotesAt(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double begin_position,
                                             double end_position) {
  if (handle) {
    return GetStatus(handle->instance.RemoveAllPerformerNotes(
        performer_id, begin_position, end_position));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemoveInstrument(BarelyHandle handle,
                                    BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.RemoveInstrument(instrument_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemovePerformer(BarelyHandle handle, BarelyId performer_id) {
  if (handle) {
    return GetStatus(handle->instance.RemovePerformer(performer_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemovePerformerInstrument(BarelyHandle handle,
                                             BarelyId performer_id,
                                             BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.RemovePerformerInstrument(performer_id,
                                                                instrument_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyRemovePerformerNote(BarelyHandle handle,
                                       BarelyId performer_id,
                                       BarelyId note_id) {
  if (handle) {
    return GetStatus(
        handle->instance.RemovePerformerNote(performer_id, note_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetAllInstrumentNotesOff(BarelyHandle handle,
                                            BarelyId instrument_id) {
  if (handle) {
    return GetStatus(handle->instance.SetAllInstrumentNotesOff(instrument_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetAllInstrumentParamsToDefault(BarelyHandle handle,
                                                   BarelyId instrument_id) {
  if (handle) {
    return GetStatus(
        handle->instance.SetAllInstrumentParamsToDefault(instrument_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyId instrument_id,
                                        float note_pitch) {
  if (handle) {
    return GetStatus(
        handle->instance.SetInstrumentNoteOff(instrument_id, note_pitch));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentNoteOffCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOffCallback instrument_note_off_callback) {
  if (handle) {
    if (instrument_note_off_callback) {
      handle->instance.SetInstrumentNoteOffCallback(
          [instrument_note_off_callback](auto instrument_id, float note_pitch) {
            instrument_note_off_callback(instrument_id, note_pitch);
          });
    } else {
      handle->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyId instrument_id, float note_pitch,
                                       float note_intensity) {
  if (handle) {
    return GetStatus(handle->instance.SetInstrumentNoteOn(
        instrument_id, note_pitch, note_intensity));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentNoteOnCallback(
    BarelyHandle handle,
    BarelyInstrumentNoteOnCallback instrument_note_on_callback) {
  if (handle) {
    if (instrument_note_on_callback) {
      handle->instance.SetInstrumentNoteOnCallback(
          [instrument_note_on_callback](auto instrument_id, float note_pitch,
                                        float note_intensity) {
            instrument_note_on_callback(instrument_id, note_pitch,
                                        note_intensity);
          });
    } else {
      handle->instance.SetInstrumentNoteOffCallback(nullptr);
    }
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentParam(BarelyHandle handle,
                                      BarelyId instrument_id, int32_t param_id,
                                      float param_value) {
  if (handle) {
    return GetStatus(handle->instance.SetInstrumentParam(
        instrument_id, param_id, param_value));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentParamToDefault(BarelyHandle handle,
                                               BarelyId instrument_id,
                                               int32_t param_id) {
  if (handle) {
    return GetStatus(
        handle->instance.SetInstrumentParamToDefault(instrument_id, param_id));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerBeginOffset(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double begin_offset) {
  if (handle) {
    return GetStatus(
        handle->instance.SetPerformerBeginOffset(performer_id, begin_offset));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerBeginPosition(BarelyHandle handle,
                                             BarelyId performer_id,
                                             double* begin_position) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerBeginPosition(
        performer_id, begin_position ? std::optional<double>{*begin_position}
                                     : std::nullopt));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerEndPosition(BarelyHandle handle,
                                           BarelyId performer_id,
                                           double* end_position) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerEndPosition(
        performer_id,
        end_position ? std::optional<double>{*end_position} : std::nullopt));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerLoop(BarelyHandle handle, BarelyId performer_id,
                                    bool loop) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerLoop(performer_id, loop));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerLoopBeginOffset(BarelyHandle handle,
                                               BarelyId performer_id,
                                               double loop_begin_offset) {
  if (handle) {
    return GetStatus(handle->instance.SetPerformerLoopBeginOffset(
        performer_id, loop_begin_offset));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPerformerLoopLength(BarelyHandle handle,
                                          BarelyId performer_id,
                                          double loop_length) {
  if (handle) {
    return GetStatus(
        handle->instance.SetPerformerLoopLength(performer_id, loop_length));
  }
  return kBarelyNotFound;
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
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle, double position) {
  if (handle) {
    handle->instance.SetPlaybackPosition(position);
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo) {
  if (handle) {
    handle->instance.SetPlaybackTempo(tempo);
    return kBarelyOk;
  }
  return kBarelyNotFound;
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
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyStartPlayback(BarelyHandle handle) {
  if (handle) {
    handle->instance.StartPlayback();
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyStopPlayback(BarelyHandle handle) {
  if (handle) {
    handle->instance.StopPlayback();
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetSampleRate(BarelyHandle handle, int32_t sample_rate) {
  if (handle) {
    handle->instance.SetSampleRate(sample_rate);
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

BarelyStatus BarelyUpdate(BarelyHandle handle, double timestamp) {
  if (handle) {
    handle->instance.Update(timestamp);
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

}  // extern "C"
