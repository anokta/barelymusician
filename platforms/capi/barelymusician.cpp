#include "platforms/capi/barelymusician.h"

#include <cstdint>

#include "barelymusician/barelymusician.h"

/// BarelyMusician C API.
struct BarelyMusician {
  /// Constructs new |BarelyMusician|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit BarelyMusician(std::int32_t sample_rate) : instance(sample_rate) {}

  /// BarelyMusician instance.
  barelyapi::BarelyMusician instance;
};

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

BarelyStatus BarelyUpdate(BarelyHandle handle, double timestamp) {
  if (handle) {
    handle->instance.Update(timestamp);
    return kBarelyOk;
  }
  return kBarelyNotFound;
}
