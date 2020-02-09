#include "barelymusician/platform/unity/unity.h"

#include <atomic>
#include <mutex>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/platform/unity/unity_instrument.h"
#include "barelymusician/util/task_runner.h"

namespace barelyapi {
namespace unity {

namespace {

// Maximum number of main thread tasks to be added per each update.
const int kNumMaxUnityTasks = 200;

struct BarelyMusician {
  BarelyMusician(int sample_rate, int num_channels, int num_frames)
      : engine(sample_rate),
        audio_runner(kNumMaxUnityTasks),
        main_runner(kNumMaxUnityTasks),
        sample_rate(sample_rate),
        num_channels(num_channels),
        num_frames(num_frames),
        id_counter(0) {}

  // Engine.
  Engine engine;

  // Audio thread task runner.
  TaskRunner audio_runner;

  // Main thread task runner.
  TaskRunner main_runner;

  // System sample rate.
  int sample_rate;

  // System number of channels.
  int num_channels;

  // System number of frames.
  int num_frames;

  // Counter to generate unique performer ids.
  int id_counter;

  // Playback position.
  std::atomic<double> position;
};

// Unity engine.
BarelyMusician* barelymusician = nullptr;

// Mutex to ensure thread-safe initialization and shutdown.
std::mutex init_shutdown_mutex;

}  // namespace

void Initialize(int sample_rate, int num_channels, int num_frames) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician == nullptr) {
    barelymusician = new BarelyMusician(sample_rate, num_channels, num_frames);
  }
}

void Shutdown() {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    delete barelymusician;
  }
  barelymusician = nullptr;
}

int Create(NoteOffFn* note_off_fn_ptr, NoteOnFn* note_on_fn_ptr,
           ProcessFn* process_fn_ptr) {
  DCHECK(barelymusician);
  const int id = ++barelymusician->id_counter;
  barelymusician->audio_runner.Add(
      [id, note_off_fn_ptr, note_on_fn_ptr, process_fn_ptr]() {
        auto instrument = std::make_unique<UnityInstrument>(
            note_off_fn_ptr, note_on_fn_ptr, process_fn_ptr);
        barelymusician->engine.Create(id, std::move(instrument));
      });
  return id;
}

void Destroy(int id) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add(
      [id]() { barelymusician->engine.Destroy(id); });
}

double GetPosition() {
  DCHECK(barelymusician);
  return barelymusician->position;
}

void NoteOff(int id, float index) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add(
      [id, index]() { barelymusician->engine.NoteOff(id, index); });
}

void NoteOn(int id, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([id, index, intensity]() {
    barelymusician->engine.NoteOn(id, index, intensity);
  });
}

void Process(int id, float* output) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    barelymusician->engine.Process(id, output, barelymusician->num_channels,
                                   barelymusician->num_frames);
  }
}

void ScheduleNoteOff(int id, double position, float index) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([id, position, index]() {
    barelymusician->engine.ScheduleNoteOff(id, position, index);
  });
}

void ScheduleNoteOn(int id, double position, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([id, position, index, intensity]() {
    barelymusician->engine.ScheduleNoteOn(id, position, index, intensity);
  });
}

void SetBeatCallback(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  if (beat_callback_ptr != nullptr) {
    const auto beat_callback = [beat_callback_ptr](int beat) {
      barelymusician->main_runner.Add(
          [beat_callback_ptr, beat]() { beat_callback_ptr(beat); });
    };
    barelymusician->audio_runner.Add([beat_callback]() {
      barelymusician->engine.SetBeatCallback(beat_callback);
    });
  } else {
    barelymusician->audio_runner.Add(
        []() { barelymusician->engine.SetBeatCallback(nullptr); });
  }
}

void SetNoteOffCallback(NoteOffCallback* note_off_callback_ptr) {
  DCHECK(barelymusician);
  if (note_off_callback_ptr != nullptr) {
    const auto note_off_callback = [note_off_callback_ptr](int id,
                                                           float index) {
      barelymusician->main_runner.Add([note_off_callback_ptr, id, index]() {
        note_off_callback_ptr(id, index);
      });
    };
    barelymusician->audio_runner.Add([note_off_callback]() {
      barelymusician->engine.SetNoteOffCallback(note_off_callback);
    });
  } else {
    barelymusician->audio_runner.Add(
        []() { barelymusician->engine.SetNoteOffCallback(nullptr); });
  }
}

void SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr) {
  DCHECK(barelymusician);
  if (note_on_callback_ptr != nullptr) {
    const auto note_on_callback = [note_on_callback_ptr](int id, float index,
                                                         float intensity) {
      barelymusician->main_runner.Add(
          [note_on_callback_ptr, id, index, intensity]() {
            note_on_callback_ptr(id, index, intensity);
          });
    };
    barelymusician->audio_runner.Add([note_on_callback]() {
      barelymusician->engine.SetNoteOnCallback(note_on_callback);
    });
  } else {
    barelymusician->audio_runner.Add(
        []() { barelymusician->engine.SetNoteOnCallback(nullptr); });
  }
}

void SetPosition(double position) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([position]() {
    if (position != barelymusician->engine.GetPosition()) {
      barelymusician->engine.SetPosition(position);
    }
  });
}

void SetTempo(double tempo) {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([tempo]() {
    if (tempo != barelymusician->engine.GetTempo()) {
      barelymusician->engine.SetTempo(tempo);
    }
  });
}

void Start() {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([]() { barelymusician->engine.Start(); });
}

void Stop() {
  DCHECK(barelymusician);
  barelymusician->audio_runner.Add([]() { barelymusician->engine.Stop(); });
}

void UpdateAudioThread() {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    barelymusician->audio_runner.Run();
    barelymusician->engine.Update(barelymusician->num_frames);
    barelymusician->position = barelymusician->engine.GetPosition();
  }
}

void UpdateMainThread() {
  DCHECK(barelymusician);
  barelymusician->main_runner.Run();
}

}  // namespace unity
}  // namespace barelyapi
