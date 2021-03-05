#include "unity/native/unity.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "barelymusician/common/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/instrument_definition.h"
#include "examples/instruments/synth_instrument.h"
#include "unity/native/unity_log_writer.h"

namespace barelyapi::unity {

namespace {

// Invalid id.
inline constexpr int kInvalidId = -1;

}  // namespace

// Unity plugin.
struct BarelyMusician {
  BarelyMusician(int sample_rate) : engine(sample_rate) {}

  // Engine.
  Engine engine;

  // Unity log writer.
  UnityLogWriter writer;
};

BarelyMusician* Initialize(int sample_rate) {
  BarelyMusician* barelymusician = new BarelyMusician(sample_rate);
  logging::SetLogWriter(&barelymusician->writer);
  return barelymusician;
}

void Shutdown(BarelyMusician* barelymusician) {
  if (barelymusician) {
    logging::SetLogWriter(nullptr);
    delete barelymusician;
  }
}

int CreateUnityInstrument(BarelyMusician* barelymusician,
                          ProcessFn* process_fn_ptr,
                          SetNoteOffFn* set_note_off_fn_ptr,
                          SetNoteOnFn* set_note_on_fn_ptr,
                          SetParamFn* set_param_fn_ptr) {
  if (barelymusician) {
    InstrumentDefinition definition = {
        .process_fn =
            [process_fn_ptr](InstrumentState*, float* output, int num_channels,
                             int num_frames) {
              process_fn_ptr(output, num_channels * num_frames, num_channels);
            },
        .set_note_off_fn = [set_note_off_fn_ptr](
                               InstrumentState*,
                               float pitch) { set_note_off_fn_ptr(pitch); },
        .set_note_on_fn =
            [set_note_on_fn_ptr](InstrumentState*, float pitch,
                                 float intensity) {
              set_note_on_fn_ptr(pitch, intensity);
            },
        .set_param_fn = [set_param_fn_ptr](
                            InstrumentState*, int id,
                            float value) { set_param_fn_ptr(id, value); }};
    return barelymusician->engine.CreateInstrument(std::move(definition), {});
  }
  return kInvalidId;
}

int CreateSynthInstrument(BarelyMusician* barelymusician) {
  if (barelymusician) {
    return barelymusician->engine.CreateInstrument(
        examples::SynthInstrument::GetDefinition(),
        examples::SynthInstrument::GetDefaultParams());
  }
  return kInvalidId;
}

void Destroy(BarelyMusician* barelymusician, int id) {
  if (barelymusician) {
    barelymusician->engine.DestroyInstrument(id);
  }
}

float GetParam(BarelyMusician* barelymusician, int id, int param_id) {
  if (barelymusician) {
    if (const float* param =
            barelymusician->engine.GetInstrumentParam(id, param_id)) {
      return *param;
    }
  }
  return 0.0f;
}

double GetPosition(BarelyMusician* barelymusician) {
  if (barelymusician) {
    return barelymusician->engine.GetPlaybackPosition();
  }
  return 0.0f;
}

double GetTempo(BarelyMusician* barelymusician) {
  if (barelymusician) {
    return 60.0 * barelymusician->engine.GetPlaybackTempo();
  }
  return 0.0f;
}

bool IsNoteOn(BarelyMusician* barelymusician, int id, float pitch) {
  if (barelymusician) {
    return barelymusician->engine.IsInstrumentNoteOn(id, pitch);
  }
  return false;
}

bool IsPlaying(BarelyMusician* barelymusician) {
  if (barelymusician) {
    return barelymusician->engine.IsPlaying();
  }
  return false;
}

void Process(BarelyMusician* barelymusician, int id, double timestamp,
             float* output, int num_channels, int num_frames) {
  if (barelymusician) {
    barelymusician->engine.ProcessInstrument(id, timestamp, output,
                                             num_channels, num_frames);
  }
}

void ResetAllParams(BarelyMusician* barelymusician, int id) {
  if (barelymusician) {
    barelymusician->engine.ResetAllInstrumentParams(id);
  }
}

void ScheduleNote(BarelyMusician* barelymusician, int id, double position,
                  double duration, float pitch, float intensity) {
  if (barelymusician) {
    barelymusician->engine.ScheduleInstrumentNote(
        id, position, position + duration, pitch, intensity);
  }
}

void SetBeatCallback(BarelyMusician* barelymusician,
                     BeatCallback* beat_callback_ptr) {
  if (barelymusician) {
    if (beat_callback_ptr) {
      barelymusician->engine.SetBeatCallback(
          [beat_callback_ptr](double timestamp, int beat) {
            beat_callback_ptr(timestamp, beat);
          });
    } else {
      barelymusician->engine.SetBeatCallback(nullptr);
    }
  }
}

void SetDebugCallback(BarelyMusician* barelymusician,
                      DebugCallback* debug_callback_ptr) {
  if (barelymusician) {
    if (debug_callback_ptr) {
      const auto debug_callback = [debug_callback_ptr](int severity,
                                                       const char* message) {
        debug_callback_ptr(severity, message);
      };
      barelymusician->writer.SetDebugCallback(debug_callback);
    } else {
      barelymusician->writer.SetDebugCallback(nullptr);
    }
  }
}

void SetNoteOffCallback(BarelyMusician* barelymusician,
                        NoteOffCallback* note_off_callback_ptr) {
  if (barelymusician) {
    if (note_off_callback_ptr) {
      barelymusician->engine.SetNoteOffCallback(
          [note_off_callback_ptr](int id, double timestamp, float pitch) {
            note_off_callback_ptr(timestamp, id, pitch);
          });
    } else {
      barelymusician->engine.SetNoteOffCallback(nullptr);
    }
  }
}

void SetNoteOnCallback(BarelyMusician* barelymusician,
                       NoteOnCallback* note_on_callback_ptr) {
  if (barelymusician) {
    if (note_on_callback_ptr) {
      barelymusician->engine.SetNoteOnCallback(
          [note_on_callback_ptr](int id, double timestamp, float pitch,
                                 float intensity) {
            note_on_callback_ptr(timestamp, id, pitch, intensity);
          });
    } else {
      barelymusician->engine.SetNoteOnCallback(nullptr);
    }
  }
}

void SetAllNotesOff(BarelyMusician* barelymusician, int id) {
  if (barelymusician) {
    barelymusician->engine.SetAllInstrumentNotesOff(id);
  }
}

void SetNoteOff(BarelyMusician* barelymusician, int id, float pitch) {
  if (barelymusician) {
    barelymusician->engine.SetInstrumentNoteOff(id, pitch);
  }
}

void SetNoteOn(BarelyMusician* barelymusician, int id, float pitch,
               float intensity) {
  if (barelymusician) {
    barelymusician->engine.SetInstrumentNoteOn(id, pitch, intensity);
  }
}

void SetParam(BarelyMusician* barelymusician, int id, int param_id,
              float value) {
  if (barelymusician) {
    barelymusician->engine.SetInstrumentParam(id, param_id, value);
  }
}

void SetPosition(BarelyMusician* barelymusician, double position) {
  if (barelymusician) {
    barelymusician->engine.SetPlaybackPosition(position);
  }
}

void SetTempo(BarelyMusician* barelymusician, double tempo) {
  if (barelymusician) {
    barelymusician->engine.SetPlaybackTempo(tempo / 60.0);
  }
}

void Start(BarelyMusician* barelymusician) {
  if (barelymusician) {
    barelymusician->engine.StartPlayback();
  }
}

void Pause(BarelyMusician* barelymusician) {
  if (barelymusician) {
    barelymusician->engine.StopPlayback();
  }
}

void Stop(BarelyMusician* barelymusician) {
  if (barelymusician) {
    barelymusician->engine.StopPlayback();
    barelymusician->engine.RemoveAllScheduledInstrumentNotes();
    barelymusician->engine.SetPlaybackPosition(0.0);
  }
}

void Update(BarelyMusician* barelymusician, double timestamp) {
  if (barelymusician) {
    barelymusician->engine.Update(timestamp);
  }
}

}  // namespace barelyapi::unity
