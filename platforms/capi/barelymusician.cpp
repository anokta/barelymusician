#include "platforms/capi/barelymusician.h"

#include <cstdint>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/status.h"
#include "examples/instruments/synth_instrument.h"

namespace {

// Returns the corresponding |BarelyStatus| value for a given |status|.
BarelyStatus GetStatus(barelyapi::Status status) {
  switch (status) {
    case barelyapi::Status::kOk:
      return kBarelyOk;
    case barelyapi::Status::kInvalidArgument:
      return kBarelyInvalidArgument;
    case barelyapi::Status::kNotFound:
      return kBarelyNotFound;
    case barelyapi::Status::kAlreadyExists:
      return kBarelyAlreadyExists;
    case barelyapi::Status::kFailedPrecondition:
      return kBarelyFailedPrecondition;
    case barelyapi::Status::kUnimplemented:
      return kBarelyUnimplemented;
    case barelyapi::Status::kInternal:
      return kBarelyInternal;
    case barelyapi::Status::kUnknown:
    default:
      return kBarelyUnknown;
  }
}

}  // namespace

/// BarelyMusician C API.
struct BarelyMusician {
  /// Constructs new |BarelyMusician|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit BarelyMusician(std::int32_t sample_rate) : instance(sample_rate) {}

  /// BarelyMusician instance.
  barelyapi::BarelyMusician instance;
};

BarelyId BarelyAddInstrument(BarelyHandle handle,
                             std::int32_t instrument_type) {
  if (handle) {
    if (instrument_type == kBarelySynthInstrument) {
      return handle->instance.AddInstrument(
          barelyapi::examples::SynthInstrument::GetDefinition(),
          barelyapi::examples::SynthInstrument::GetDefaultParams());
    }
  }
  return kBarelyInvalidId;
}

BarelyId BarelyAddPerformer(BarelyHandle handle) {
  if (handle) {
    return handle->instance.AddPerformer();
  }
  return kBarelyInvalidId;
}

BarelyStatus BarelyAddPerformerInstrument(BarelyHandle handle,
                                          BarelyId performer_id,
                                          BarelyId instrument_id) {
  if (handle) {
    return GetStatus(
        handle->instance.AddPerformerInstrument(performer_id, instrument_id));
  }
  return kBarelyNotFound;
}

BarelyHandle BarelyCreate(std::int32_t sample_rate) {
  return new BarelyMusician(sample_rate);
}

BarelyStatus BarelyDestroy(BarelyHandle handle) {
  if (handle) {
    delete handle;
    return kBarelyOk;
  }
  return kBarelyNotFound;
}

double BarelyGetPlaybackPosition(BarelyHandle handle) {
  if (handle) {
    return handle->instance.GetPlaybackPosition();
  }
  return 0.0;
}

double BarelyGetPlaybackTempo(BarelyHandle handle) {
  if (handle) {
    return handle->instance.GetPlaybackTempo();
  }
  return 0.0;
}

bool BarelyIsPlaying(BarelyHandle handle) {
  if (handle) {
    return handle->instance.IsPlaying();
  }
  return false;
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
                                      BarelyId instrument_id,
                                      std::int32_t param_id,
                                      float param_value) {
  if (handle) {
    return GetStatus(handle->instance.SetInstrumentParam(
        instrument_id, param_id, param_value));
  }
  return kBarelyNotFound;
}

BarelyStatus BarelySetInstrumentParamToDefault(BarelyHandle handle,
                                               BarelyId instrument_id,
                                               std::int32_t param_id) {
  if (handle) {
    return GetStatus(
        handle->instance.SetInstrumentParamToDefault(instrument_id, param_id));
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

BarelyStatus BarelySetSampleRate(BarelyHandle handle,
                                 std::int32_t sample_rate) {
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
