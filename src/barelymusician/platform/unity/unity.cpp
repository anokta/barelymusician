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

BarelyMusician* barelymusician = nullptr;

double global_clock = 0.0;

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

int CreateSequencer(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  const int sequencer_id = barelymusician->CreateSequencer();
  const int sample_rate = barelymusician->GetSampleRate();
  const auto sequencer_beat_callback = [beat_callback_ptr, sample_rate](
                                           const Transport& transport,
                                           int start_sample, int) {
    beat_callback_ptr(
        transport.section, transport.bar, transport.beat,
        global_clock + static_cast<double>(start_sample) / sample_rate);
  };
  barelymusician->RegisterSequencerBeatCallback(sequencer_id,
                                                sequencer_beat_callback);
  return sequencer_id;
}

void DestroySequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->DestroySequencer(sequencer_id);
}

void ProcessSequencer(int sequencer_id, double dsp_time) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  DCHECK(barelymusician);
  global_clock = dsp_time;
  barelymusician->Update();
  barelymusician->ProcessSequencer(sequencer_id);
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
