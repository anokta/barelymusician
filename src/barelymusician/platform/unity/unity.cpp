#include "barelymusician/platform/unity/unity.h"

#include <mutex>
#include <utility>

#include "barelymusician/api/barelymusician.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/platform/unity/unity_instrument.h"

namespace barelyapi {
namespace unity {

namespace {

// BarelyMusician API.
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

void Start() {
  DCHECK(barelymusician);
  barelymusician->Start();
}

void Stop() {
  DCHECK(barelymusician);
  barelymusician->Stop();
}

void Update() {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  DCHECK(barelymusician);
  barelymusician->Update();
}

void RegisterBeatCallback(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  const auto sequencer_beat_callback =
      [beat_callback_ptr](const Transport& transport, int, int) {
        beat_callback_ptr(transport.section, transport.bar, transport.beat);
      };
  barelymusician->RegisterBeatCallback(sequencer_beat_callback);
}

void SetNumBars(int num_bars) {
  DCHECK(barelymusician);
  barelymusician->SetNumBars(num_bars);
}

void SetNumBeats(int num_beats) {
  DCHECK(barelymusician);
  barelymusician->SetNumBeats(num_beats);
}

void SetPosition(int section, int bar, int beat) {
  DCHECK(barelymusician);
  barelymusician->SetPosition(section, bar, beat);
}

void SetTempo(float tempo) {
  DCHECK(barelymusician);
  barelymusician->SetTempo(tempo);
}

int CreateInstrument(ClearCallback* clear_callback_ptr,
                     NoteOffCallback* note_off_callback_ptr,
                     NoteOnCallback* note_on_callback_ptr,
                     ProcessCallback* process_callback_ptr) {
  DCHECK(barelymusician);
  return barelymusician->CreateInstrument<UnityInstrument>(
      clear_callback_ptr, note_off_callback_ptr, note_on_callback_ptr,
      process_callback_ptr);
}

void DestroyInstrument(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->DestroyInstrument(instrument_id);
}

void ProcessInstrument(int instrument_id, float* output) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  DCHECK(barelymusician);
  barelymusician->Update();
  barelymusician->ProcessInstrument(instrument_id, output);
}

void SetInstrumentClear(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->SetInstrumentClear(instrument_id);
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
