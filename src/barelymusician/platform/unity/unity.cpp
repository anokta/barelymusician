#include "barelymusician/platform/unity/unity.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/instrument_manager.h"
#include "barelymusician/engine/sequencer.h"
#include "barelymusician/platform/unity/unity_instrument.h"
#include "barelymusician/platform/unity/unity_log_writer.h"
#include "barelymusician/util/task_runner.h"
#include "instruments/basic_synth_instrument.h"

namespace barelyapi {
namespace unity {

namespace {

// Unity plugin.
struct BarelyMusician {
  BarelyMusician(int sample_rate)
      : sample_rate(sample_rate),
        manager(sample_rate),
        sequencer(&manager),
        start_timestamp(0.0) {}

  // System sample rate.
  const int sample_rate;

  // Engine.
  InstrumentManager manager;
  Sequencer sequencer;

  double start_timestamp;

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
  if (barelymusician == nullptr) {
    barelymusician = new BarelyMusician(sample_rate);
    logging::SetLogWriter(&barelymusician->writer);
  }
}

void Shutdown() {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (barelymusician != nullptr) {
    logging::SetLogWriter(nullptr);
    delete barelymusician;
  }
  barelymusician = nullptr;
}

int CreateUnityInstrument(NoteOffFn* note_off_fn_ptr, NoteOnFn* note_on_fn_ptr,
                          ProcessFn* process_fn_ptr) {
  DCHECK(barelymusician);
  InstrumentDefinition definition;
  definition.get_instrument_fn =
      [note_off_fn_ptr, note_on_fn_ptr,
       process_fn_ptr](int) -> std::unique_ptr<Instrument> {
    return std::make_unique<UnityInstrument>(note_off_fn_ptr, note_on_fn_ptr,
                                             process_fn_ptr);
  };
  const int id = barelymusician->manager.Create(definition);
  barelymusician->sequencer.Create(id);
  return id;
}

int CreateBasicSynthInstrument(int num_voices, int oscillator_type) {
  DCHECK(barelymusician);
  InstrumentDefinition definition;
  definition.get_instrument_fn =
      [num_voices,
       oscillator_type](int sample_rate) -> std::unique_ptr<Instrument> {
    auto instrument = std::make_unique<examples::BasicSynthInstrument>(
        sample_rate, num_voices);
    instrument->Control(examples::BasicSynthInstrumentParam::kOscillatorType,
                        static_cast<float>(oscillator_type));
    instrument->Control(examples::BasicSynthInstrumentParam::kEnvelopeAttack,
                        0.0f);
    return instrument;
  };
  const int id = barelymusician->manager.Create(definition);
  barelymusician->sequencer.Create(id);
  return id;
}

void Destroy(int id) {
  DCHECK(barelymusician);
  barelymusician->sequencer.Destroy(id);
  barelymusician->manager.Destroy(id);
}

double GetPosition() {
  DCHECK(barelymusician);
  return barelymusician->sequencer.GetPosition();
}

double GetTempo() {
  DCHECK(barelymusician);
  return barelymusician->sequencer.GetTempo();
}

bool IsPlaying() {
  DCHECK(barelymusician);
  return barelymusician->sequencer.IsPlaying();
}

void NoteOff(int id, float index) {
  DCHECK(barelymusician);
  barelymusician->manager.NoteOff(id, index);
}

void NoteOn(int id, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->manager.NoteOn(id, index, intensity);
}

void Process(int id, double timestamp, float* output, int num_channels,
             int num_frames) {
  std::lock_guard<std::mutex> lock(initialize_shutdown_mutex);
  if (barelymusician != nullptr) {
    const double end_timestamp =
        timestamp + static_cast<double>(num_frames) /
                        static_cast<double>(barelymusician->sample_rate);
    barelymusician->manager.Process(id, timestamp, end_timestamp, output,
                                    num_channels, num_frames);
  }
}

void ScheduleNote(int id, double position, double duration, float index,
                  float intensity) {
  DCHECK(barelymusician);
  barelymusician->sequencer.ScheduleNote(id, position, duration, index,
                                         intensity);
}

void ScheduleNoteOff(int id, double position, float index) {
  DCHECK(barelymusician);
  barelymusician->sequencer.ScheduleNoteOff(id, position, index);
}

void ScheduleNoteOn(int id, double position, float index, float intensity) {
  DCHECK(barelymusician);
  barelymusician->sequencer.ScheduleNoteOn(id, position, index, intensity);
}

void SetBeatCallback(BeatCallback* beat_callback_ptr) {
  DCHECK(barelymusician);
  if (beat_callback_ptr != nullptr) {
    barelymusician->sequencer.SetBeatCallback(
        [beat_callback_ptr](int beat) { beat_callback_ptr(beat); });
  } else {
    barelymusician->sequencer.SetBeatCallback(nullptr);
  }
}

void SetDebugCallback(DebugCallback* debug_callback_ptr) {
  DCHECK(barelymusician);
  if (debug_callback_ptr != nullptr) {
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
  if (note_off_callback_ptr != nullptr) {
    barelymusician->manager.SetNoteOffCallback(
        [note_off_callback_ptr](int id, float index) {
          note_off_callback_ptr(id, index);
        });
  } else {
    barelymusician->manager.SetNoteOffCallback(nullptr);
  }
}

void SetNoteOnCallback(NoteOnCallback* note_on_callback_ptr) {
  DCHECK(barelymusician);
  if (note_on_callback_ptr != nullptr) {
    barelymusician->manager.SetNoteOnCallback(
        [note_on_callback_ptr](int id, float index, float intensity) {
          note_on_callback_ptr(id, index, intensity);
        });
  } else {
    barelymusician->manager.SetNoteOnCallback(nullptr);
  }
}

void SetPosition(double position) {
  DCHECK(barelymusician);
  barelymusician->sequencer.SetPosition(position);
}

void SetTempo(double tempo) {
  DCHECK(barelymusician);
  barelymusician->sequencer.SetTempo(tempo);
}

void Start(double timestamp) {
  DCHECK(barelymusician);
  barelymusician->sequencer.Start(timestamp);
}

void Stop() {
  DCHECK(barelymusician);
  barelymusician->sequencer.Stop();
  barelymusician->start_timestamp = std::numeric_limits<double>::max();
}

void UpdateMainThread(double timestamp, double lookahead) {
  DCHECK(barelymusician);
  barelymusician->sequencer.Update(timestamp, lookahead);
}

}  // namespace unity
}  // namespace barelyapi
