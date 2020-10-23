#include "barelymusician/platform/unity/unity.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/platform/unity/unity_instrument.h"
#include "barelymusician/platform/unity/unity_log_writer.h"
#include "barelymusician/util/task_runner.h"
#include "instruments/basic_synth_instrument.h"

namespace barelyapi {
namespace unity {

namespace {

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

Id CreateUnityInstrument(NoteOffFn* note_off_fn_ptr, NoteOnFn* note_on_fn_ptr,
                         ProcessFn* process_fn_ptr) {
  DCHECK(barelymusician);
  auto instrument = std::make_unique<UnityInstrument>(
      note_off_fn_ptr, note_on_fn_ptr, process_fn_ptr);
  return barelymusician->engine.Create(std::move(instrument), {});
}

Id CreateBasicSynthInstrument() {
  DCHECK(barelymusician);
  auto instrument = std::make_unique<examples::BasicSynthInstrument>(
      barelymusician->sample_rate);
  return barelymusician->engine.Create(
      std::move(instrument),
      examples::BasicSynthInstrument::GetDefaultParams());
}

void Destroy(Id id) {
  DCHECK(barelymusician);
  barelymusician->engine.Destroy(id);
}

double GetPosition() {
  DCHECK(barelymusician);
  return barelymusician->engine.GetPosition();
}

double GetTempo() {
  DCHECK(barelymusician);
  return barelymusician->engine.GetTempo();
}

bool IsPlaying() {
  DCHECK(barelymusician);
  return barelymusician->engine.IsPlaying();
}

void NoteOff(Id id, float index) {
  DCHECK(barelymusician);
  barelymusician->engine.NoteOff(id, index);
}

void NoteOn(Id id, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->engine.NoteOn(id, index, intensity);
}

void Process(Id id, double timestamp, float* output, int num_channels,
             int num_frames) {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (barelymusician) {
    const double end_timestamp =
        timestamp + static_cast<double>(num_frames) /
                        static_cast<double>(barelymusician->sample_rate);
    barelymusician->engine.Process(id, timestamp, end_timestamp, output,
                                   num_channels, num_frames);
  }
}

void ScheduleNote(Id id, double position, double duration, float index,
                  float intensity) {
  DCHECK(barelymusician);
  barelymusician->engine.ScheduleNoteOn(id, position, index, intensity);
  barelymusician->engine.ScheduleNoteOff(id, position + duration, index);
}

void ScheduleNoteOff(Id id, double position, float index) {
  DCHECK(barelymusician);
  barelymusician->engine.ScheduleNoteOff(id, position, index);
}

void ScheduleNoteOn(Id id, double position, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->engine.ScheduleNoteOn(id, position, index, intensity);
}

void SetParam(Id id, int param_id, float value) {
  DCHECK(barelymusician);
  barelymusician->engine.SetParam(id, param_id, value);
}

void SetBeatCallback(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  if (beat_callback_ptr) {
    barelymusician->engine.SetBeatCallback(
        [beat_callback_ptr](double timestamp, int beat) {
          beat_callback_ptr(timestamp, beat);
        });
  } else {
    barelymusician->engine.SetBeatCallback(nullptr);
  }
}

void SetDebugCallback(DebugCallback* debug_callback_ptr) {
  DCHECK(barelymusician);
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

void SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr) {
  DCHECK(barelymusician);
  if (note_off_callback_ptr) {
    barelymusician->engine.SetNoteOffCallback(
        [note_off_callback_ptr](double timestamp, Id id, float index) {
          note_off_callback_ptr(timestamp, id, index);
        });
  } else {
    barelymusician->engine.SetNoteOffCallback(nullptr);
  }
}

void SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr) {
  DCHECK(barelymusician);
  if (note_on_callback_ptr) {
    barelymusician->engine.SetNoteOnCallback(
        [note_on_callback_ptr](double timestamp, Id id, float index,
                               float intensity) {
          note_on_callback_ptr(timestamp, id, index, intensity);
        });
  } else {
    barelymusician->engine.SetNoteOnCallback(nullptr);
  }
}

void SetPosition(double position) {
  DCHECK(barelymusician);
  barelymusician->engine.SetPosition(position);
}

void SetTempo(double tempo) {
  DCHECK(barelymusician);
  barelymusician->engine.SetTempo(tempo);
}

void Start(double timestamp) {
  DCHECK(barelymusician);
  barelymusician->engine.Start(timestamp);
}

void Stop() {
  DCHECK(barelymusician);
  barelymusician->engine.Stop();
}

void Update(double timestamp) {
  DCHECK(barelymusician);
  barelymusician->engine.Update(timestamp);
}

}  // namespace unity
}  // namespace barelyapi
