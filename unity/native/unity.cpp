#include "unity/native/unity.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>

#include "barelymusician/common/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "examples/instruments/synth_instrument.h"
#include "unity/native/unity_log_writer.h"

namespace barelyapi::unity {

namespace {

// Invalid id.
inline constexpr int kInvalidId = -1;

// Unity plugin.
struct BarelyMusician {
  BarelyMusician(int sample_rate) : sample_rate(sample_rate) {}

  // Sampling rate.
  int sample_rate;

  // Engine.
  Engine engine;

  // Unity log writer.
  UnityLogWriter writer;
};

// Unity plugin instance.
BarelyMusician* barelymusician = nullptr;

// Mutex to ensure thread-safe initialization and shutdown.
std::mutex initialize_shutdown_mutex;

}  // namespace

void Initialize(int sample_rate) {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (!barelymusician) {
    barelymusician = new BarelyMusician(sample_rate);
    logging::SetLogWriter(&barelymusician->writer);
  }
}

void Shutdown() {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (barelymusician) {
    logging::SetLogWriter(nullptr);
    delete barelymusician;
  }
  barelymusician = nullptr;
}

int CreateUnityInstrument(ProcessFn* process_fn_ptr,
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
    return barelymusician->engine.CreateInstrument(std::move(definition));
  }
  return kInvalidId;
}

int CreateSynthInstrument() {
  if (barelymusician) {
    return barelymusician->engine.CreateInstrument(
        examples::SynthInstrument::GetDefinition(barelymusician->sample_rate),
        examples::SynthInstrument::GetDefaultParams());
  }
  return kInvalidId;
}

void Destroy(int id) {
  if (barelymusician) {
    barelymusician->engine.DestroyInstrument(id);
  }
}

float GetParam(int id, int param_id) {
  if (barelymusician) {
    const auto param_or = barelymusician->engine.GetParam(id, param_id);
    if (IsOk(param_or)) {
      return GetValue(param_or);
    }
  }
  return 0.0f;
}

double GetPosition() {
  if (barelymusician) {
    return barelymusician->engine.GetPosition();
  }
  return 0.0f;
}

double GetTempo() {
  if (barelymusician) {
    return barelymusician->engine.GetTempo();
  }
  return 0.0f;
}

bool IsNoteOn(int id, float pitch) {
  if (barelymusician) {
    const auto is_note_on_or = barelymusician->engine.IsNoteOn(id, pitch);
    if (IsOk(is_note_on_or)) {
      return GetValue(is_note_on_or);
    }
  }
  return false;
}

bool IsPlaying() {
  if (barelymusician) {
    return barelymusician->engine.IsPlaying();
  }
  return false;
}

void Process(int id, double timestamp, float* output, int num_channels,
             int num_frames) {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (barelymusician) {
    barelymusician->engine.Process(
        id, SamplesFromSeconds(barelymusician->sample_rate, timestamp), output,
        num_channels, num_frames);
  }
}

void ResetAllParams(int id) {
  if (barelymusician) {
    barelymusician->engine.ResetAllParams(id);
  }
}

void ScheduleNote(int id, double position, double duration, float pitch,
                  float intensity) {
  if (barelymusician) {
    barelymusician->engine.ScheduleNote(id, position, duration, pitch,
                                        intensity);
  }
}

void SetBeatCallback(BeatCallback* beat_callback_ptr) {
  if (barelymusician) {
    if (beat_callback_ptr) {
      barelymusician->engine.SetBeatCallback(
          [beat_callback_ptr, sample_rate = barelymusician->sample_rate](
              std::int64_t timestamp, int beat) {
            beat_callback_ptr(SecondsFromSamples(sample_rate, timestamp), beat);
          });
    } else {
      barelymusician->engine.SetBeatCallback(nullptr);
    }
  }
}

void SetDebugCallback(DebugCallback* debug_callback_ptr) {
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

void SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr) {
  if (barelymusician) {
    if (note_off_callback_ptr) {
      barelymusician->engine.SetNoteOffCallback(
          [note_off_callback_ptr, sample_rate = barelymusician->sample_rate](
              int id, std::int64_t timestamp, float pitch) {
            note_off_callback_ptr(SecondsFromSamples(sample_rate, timestamp),
                                  id, pitch);
          });
    } else {
      barelymusician->engine.SetNoteOffCallback(nullptr);
    }
  }
}

void SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr) {
  if (barelymusician) {
    if (note_on_callback_ptr) {
      barelymusician->engine.SetNoteOnCallback(
          [note_on_callback_ptr, sample_rate = barelymusician->sample_rate](
              int id, std::int64_t timestamp, float pitch, float intensity) {
            note_on_callback_ptr(SecondsFromSamples(sample_rate, timestamp), id,
                                 pitch, intensity);
          });
    } else {
      barelymusician->engine.SetNoteOnCallback(nullptr);
    }
  }
}

void SetAllNotesOff(int id) {
  if (barelymusician) {
    barelymusician->engine.SetAllNotesOff(id);
  }
}

void SetNoteOff(int id, float pitch) {
  if (barelymusician) {
    barelymusician->engine.SetNoteOff(id, pitch);
  }
}

void SetNoteOn(int id, float pitch, float intensity) {
  if (barelymusician) {
    barelymusician->engine.SetNoteOn(id, pitch, intensity);
  }
}

void SetParam(int id, int param_id, float value) {
  if (barelymusician) {
    barelymusician->engine.SetParam(id, param_id, value);
  }
}

void SetPosition(double position) {
  if (barelymusician) {
    barelymusician->engine.SetPosition(position);
  }
}

void SetTempo(double tempo) {
  if (barelymusician) {
    barelymusician->engine.SetTempo(tempo);
  }
}

void Start(double timestamp) {
  if (barelymusician) {
    barelymusician->engine.Start(
        SamplesFromSeconds(barelymusician->sample_rate, timestamp));
  }
}

void Pause() {
  if (barelymusician) {
    barelymusician->engine.Stop();
  }
}

void Stop() {
  if (barelymusician) {
    barelymusician->engine.Stop();
    barelymusician->engine.RemoveAllScheduledNotes();
    barelymusician->engine.SetPosition(0.0);
  }
}

void Update(double timestamp) {
  if (barelymusician) {
    barelymusician->engine.Update(
        barelymusician->sample_rate,
        SamplesFromSeconds(barelymusician->sample_rate, timestamp));
  }
}

}  // namespace barelyapi::unity
