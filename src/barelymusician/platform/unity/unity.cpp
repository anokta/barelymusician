#include "barelymusician/platform/unity/unity.h"

#include <mutex>
#include <utility>

#include "barelymusician/api/barelymusician.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/platform/unity/unity_instrument.h"

namespace barelyapi {
namespace unity {

namespace {

// Maximum number of main thread tasks to be added per each update.
const int kNumMaxUnityTasks = 500;

// BarelyMusician API.
BarelyMusician* barelymusician = nullptr;

// Main thread task runner.
TaskRunner* main_task_runner = nullptr;

// Mutex to ensure thread-safe initialization and shutdown.
std::mutex init_shutdown_mutex;

}  // namespace

void Initialize(int sample_rate, int num_channels, int num_frames) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician == nullptr) {
    barelymusician = new BarelyMusician(sample_rate, num_channels, num_frames);
    main_task_runner = new TaskRunner(kNumMaxUnityTasks);
  }
}

void Shutdown() {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    delete barelymusician;
    delete main_task_runner;
  }
  barelymusician = nullptr;
}

void UpdateAudioThread() {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    barelymusician->Update();
  }
}

void UpdateMainThread() {
  DCHECK(barelymusician);
  main_task_runner->Run();
}

void ResetSequencer() {
  DCHECK(barelymusician);
  barelymusician->Reset();
}

void SetSequencerBeatCallback(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  if (beat_callback_ptr == nullptr) {
    barelymusician->SetBeatCallback(nullptr);
    return;
  }
  const auto beat_callback = [beat_callback_ptr](const Transport& transport,
                                                 int, int) {
    const int section = transport.section;
    const int bar = transport.bar;
    const int beat = transport.beat;
    main_task_runner->Add([beat_callback_ptr, section, bar, beat]() {
      beat_callback_ptr(section, bar, beat);
    });
  };
  barelymusician->SetBeatCallback(beat_callback);
}

void SetSequencerNumBars(int num_bars) {
  DCHECK(barelymusician);
  barelymusician->SetNumBars(num_bars);
}

void SetSequencerNumBeats(int num_beats) {
  DCHECK(barelymusician);
  barelymusician->SetNumBeats(num_beats);
}

void SetSequencerTempo(float tempo) {
  DCHECK(barelymusician);
  barelymusician->SetTempo(tempo);
}

void StartSequencer() {
  DCHECK(barelymusician);
  barelymusician->Start();
}

void StopSequencer() {
  DCHECK(barelymusician);
  barelymusician->Stop();
}

int CreateInstrument(AllNotesOffFn* all_notes_off_fn_ptr,
                     NoteOffFn* note_off_fn_ptr, NoteOnFn* note_on_fn_ptr,
                     ProcessFn* process_fn_ptr) {
  DCHECK(barelymusician);
  const int instrument_id = barelymusician->CreateInstrument<UnityInstrument>(
      all_notes_off_fn_ptr, note_off_fn_ptr, note_on_fn_ptr, process_fn_ptr);
  return instrument_id;
}

void DestroyInstrument(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->DestroyInstrument(instrument_id);
}

void ProcessInstrument(int instrument_id, float* output) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  if (barelymusician != nullptr) {
    barelymusician->Update();
    barelymusician->ProcessInstrument(instrument_id, output);
  }
}

void SetInstrumentAllNotesOff(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->SetInstrumentAllNotesOff(instrument_id);
}

void SetInstrumentNoteOff(int instrument_id, float index) {
  DCHECK(barelymusician);
  barelymusician->SetInstrumentNoteOff(instrument_id, index);
}

void SetInstrumentNoteOn(int instrument_id, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->SetInstrumentNoteOn(instrument_id, index, intensity);
}

}  // namespace unity
}  // namespace barelyapi
