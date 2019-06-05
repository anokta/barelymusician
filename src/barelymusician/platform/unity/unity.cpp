#include "barelymusician/platform/unity/unity.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/platform/unity/unity_instrument.h"

namespace barelyapi {
namespace unity {

namespace {

const int kNumMaxTasks = 100;

struct BarelyMusician {
  BarelyMusician(int sample_rate, int num_channels, int num_frames)
      : sample_rate(sample_rate),
        num_channels(num_channels),
        num_frames(num_frames),
        id_counter(0),
        dsp_time(0.0),
        task_runner(kNumMaxTasks) {
    CHECK_GT(sample_rate, 0);
    CHECK_GT(num_channels, 0);
    CHECK_GT(num_frames, 0);
  }

  int sample_rate;
  int num_channels;
  int num_frames;

  std::atomic<int> id_counter;

  double dsp_time;

  TaskRunner task_runner;

  std::unordered_map<int, UnityInstrument> instruments;
  std::unordered_map<int, Sequencer> sequencers;

  int AddSequencer(BeatCallback* beat_callback_ptr) {
    const int sequencer_id = ++id_counter;
    task_runner.Add([this, beat_callback_ptr, sequencer_id]() {
      Sequencer sequencer(sample_rate);
      const auto sequencer_beat_callback = [beat_callback_ptr, this](
                                               const Transport& transport,
                                               int start_sample, int) {
        beat_callback_ptr(
            transport.section, transport.bar, transport.beat,
            dsp_time + static_cast<double>(start_sample) / sample_rate);
      };
      sequencer.RegisterBeatCallback(sequencer_beat_callback);
      sequencers.insert({sequencer_id, std::move(sequencer)});
    });
    return sequencer_id;
  }

  Sequencer* GetSequencer(int sequencer_id) {
    const auto it = sequencers.find(sequencer_id);
    if (it != sequencers.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void RemoveSequencer(int sequencer_id) {
    task_runner.Add([this, sequencer_id]() { sequencers.erase(sequencer_id); });
  }

  int AddInstrument(NoteOffCallback* note_off_callback_ptr,
                    NoteOnCallback* note_on_callback_ptr,
                    ProcessCallback* process_callback_ptr,
                    ResetCallback* reset_callback_ptr) {
    const int instrument_id = ++id_counter;
    task_runner.Add([this, note_off_callback_ptr, note_on_callback_ptr,
                     process_callback_ptr, reset_callback_ptr,
                     instrument_id]() {
      UnityInstrument instrument(note_off_callback_ptr, note_on_callback_ptr,
                                 process_callback_ptr, reset_callback_ptr);
      instruments.insert({instrument_id, std::move(instrument)});
    });
    return instrument_id;
  }

  UnityInstrument* GetInstrument(int instrument_id) {
    const auto it = instruments.find(instrument_id);
    if (it != instruments.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void RemoveInstrument(int instrument_id) {
    task_runner.Add(
        [this, instrument_id]() { instruments.erase(instrument_id); });
  }
};

BarelyMusician* barelymusician = nullptr;

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
  return barelymusician->AddSequencer(beat_callback_ptr);
}

void DestroySequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->RemoveSequencer(sequencer_id);
}

void ProcessSequencer(int sequencer_id, double dsp_time) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  barelymusician->task_runner.Run();
  DCHECK(barelymusician);
  barelymusician->dsp_time = dsp_time;
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  if (sequencer == nullptr) {
    DLOG(WARNING) << "Sequencer not found with ID: " << sequencer_id;
    return;
  }

  sequencer->Update(barelymusician->num_frames);
}

void SetSequencerNumBars(int sequencer_id, int num_bars) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  if (sequencer == nullptr) {
    DLOG(WARNING) << "Sequencer not found with ID: " << sequencer_id;
    return;
  }

  barelymusician->task_runner.Add(
      [sequencer, num_bars]() { sequencer->SetNumBars(num_bars); });
}

void SetSequencerNumBeats(int sequencer_id, int num_beats) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  if (sequencer == nullptr) {
    DLOG(WARNING) << "Sequencer not found with ID: " << sequencer_id;
    return;
  }

  barelymusician->task_runner.Add(
      [sequencer, num_beats]() { sequencer->SetNumBeats(num_beats); });
}

void SetSequencerTempo(int sequencer_id, float tempo) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  if (sequencer == nullptr) {
    DLOG(WARNING) << "Sequencer not found with ID: " << sequencer_id;
    return;
  }

  barelymusician->task_runner.Add(
      [sequencer, tempo]() { sequencer->SetTempo(tempo); });
}

int CreateInstrument(NoteOffCallback* note_off_callback_ptr,
                     NoteOnCallback* note_on_callback_ptr,
                     ProcessCallback* process_callback_ptr,
                     ResetCallback* reset_callback_ptr) {
  DCHECK(barelymusician);
  return barelymusician->AddInstrument(
      note_off_callback_ptr, note_on_callback_ptr, process_callback_ptr,
      reset_callback_ptr);
}

void DestroyInstrument(int instrument_id) {
  DCHECK(barelymusician);
  barelymusician->RemoveInstrument(instrument_id);
}

void NoteOffInstrument(int instrument_id, float index) {
  DCHECK(barelymusician);
  Instrument* instrument = barelymusician->GetInstrument(instrument_id);
  if (instrument == nullptr) {
    DLOG(WARNING) << "Instrument not found with ID: " << instrument_id;
    return;
  }

  barelymusician->task_runner.Add(
      [instrument, index]() { instrument->NoteOff(index); });
}

void NoteOnInstrument(int instrument_id, float index, float intensity) {
  DCHECK(barelymusician);
  Instrument* instrument = barelymusician->GetInstrument(instrument_id);
  if (instrument == nullptr) {
    DLOG(WARNING) << "Instrument not found with ID: " << instrument_id;
    return;
  }

  barelymusician->task_runner.Add([instrument, index, intensity]() {
    instrument->NoteOn(index, intensity);
  });
}

void ProcessInstrument(int instrument_id, float* output) {
  std::lock_guard<std::mutex> lock(init_shutdown_mutex);
  barelymusician->task_runner.Run();
  DCHECK(barelymusician);
  Instrument* instrument = barelymusician->GetInstrument(instrument_id);
  if (instrument == nullptr) {
    DLOG(WARNING) << "Instrument not found with ID: " << instrument_id;
    return;
  }

  instrument->Process(output, barelymusician->num_channels,
                      barelymusician->num_frames);
}

void ResetInstrument(int instrument_id) {
  DCHECK(barelymusician);
  Instrument* instrument = barelymusician->GetInstrument(instrument_id);
  if (instrument == nullptr) {
    DLOG(WARNING) << "Instrument not found with ID: " << instrument_id;
    return;
  }

  barelymusician->task_runner.Add([instrument]() { instrument->Reset(); });
}

}  // namespace unity
}  // namespace barelyapi
