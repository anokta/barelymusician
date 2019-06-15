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

int CreateSequencer(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  const int sequencer_id = barelymusician->CreateSequencer();
  const auto sequencer_beat_callback = [beat_callback_ptr](
                                           const Transport& transport, int) {
    const int section = transport.section;
    const int bar = transport.bar;
    const int beat = transport.beat;
    main_task_runner->Add([beat_callback_ptr, section, bar, beat]() {
      beat_callback_ptr(section, bar, beat);
    });
  };
  barelymusician->RegisterSequencerBeatCallback(sequencer_id,
                                                sequencer_beat_callback);
  return sequencer_id;
}

void DestroySequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->DestroySequencer(sequencer_id);
}

void SetSequencerNumBars(int sequencer_id, int num_bars) {
  DCHECK(barelymusician);
  barelymusician->SetSequencerNumBars(sequencer_id, num_bars);
}

void SetSequencerNumBeats(int sequencer_id, int num_beats) {
  DCHECK(barelymusician);
  barelymusician->SetSequencerNumBeats(sequencer_id, num_beats);
}

void SetSequencerPosition(int sequencer_id, int section, int bar, int beat) {
  DCHECK(barelymusician);
  barelymusician->SetSequencerPosition(sequencer_id, section, bar, beat);
}

void SetSequencerTempo(int sequencer_id, float tempo) {
  DCHECK(barelymusician);
  barelymusician->SetSequencerTempo(sequencer_id, tempo);
}

void StartSequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->StartSequencer(sequencer_id);
}

void StopSequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->StopSequencer(sequencer_id);
}

int CreateInstrument(ClearFn* clear_fn_ptr, NoteOffFn* note_off_fn_ptr,
                     NoteOnFn* note_on_fn_ptr, ProcessFn* process_fn_ptr,
                     NoteOffCallback* note_off_callback_ptr,
                     NoteOnCallback* note_on_callback_ptr) {
  DCHECK(barelymusician);
  const int instrument_id = barelymusician->CreateInstrument<UnityInstrument>(
      clear_fn_ptr, note_off_fn_ptr, note_on_fn_ptr, process_fn_ptr);
  barelymusician->RegisterInstrumentNoteOffCallback(instrument_id,
                                                    note_off_callback_ptr);
  barelymusician->RegisterInstrumentNoteOnCallback(instrument_id,
                                                   note_on_callback_ptr);
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

void SetInstrumentClear(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->ClearAllInstrumentNotes(instrument_id);
}

void SetInstrumentNoteOff(int instrument_id, float index) {
  DCHECK(barelymusician);
  barelymusician->StopInstrumentNote(instrument_id, index, 0);
}

void SetInstrumentNoteOn(int instrument_id, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->StartInstrumentNote(instrument_id, index, intensity, 0);
}

}  // namespace unity
}  // namespace barelyapi
