#include "barelymusician/barelymusician.h"

#include <cassert>
#include <cstdint>

#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

using ::barely::InstrumentController;
using ::barely::internal::Musician;
using ::barely::internal::Observable;
using ::barely::internal::Observer;
using ::barely::internal::Performer;
using ::barely::internal::Task;

// Musician.
struct BarelyMusician : public Observable<Musician> {
 public:
  BarelyMusician(int32_t frame_rate, double reference_frequency) noexcept
      : Observable<Musician>(frame_rate, reference_frequency) {}
  ~BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) noexcept = delete;
  BarelyMusician& operator=(const BarelyMusician& other) noexcept = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;
};

// Instrument.
struct BarelyInstrument : public Observable<InstrumentController> {
 public:
  explicit BarelyInstrument(BarelyMusicianHandle musician) noexcept
      : Observable<InstrumentController>(musician->GetFrameRate(),
                                         musician->GetReferenceFrequency(),
                                         musician->GetUpdateFrame()),
        musician_(musician->Observe()) {
    assert(musician_);
    musician_->AddInstrument(this);
  }

  ~BarelyInstrument() noexcept {
    if (musician_) {
      musician_->RemoveInstrument(this);
    }
  }

  /// Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

  Musician& musician() const noexcept {
    assert(musician_);
    return *musician_;
  }

 private:
  Observer<Musician> musician_;
};

// Performer.
struct BarelyPerformer : public Observable<Performer> {
 public:
  BarelyPerformer(BarelyMusicianHandle musician, int process_order) noexcept
      : Observable<Performer>(process_order), musician_(musician->Observe()) {
    assert(musician_);
    musician_->AddPerformer(this);
  }

  ~BarelyPerformer() noexcept {
    if (musician_) {
      musician_->RemovePerformer(this);
    }
  }

  /// Non-copyable and non-movable.
  BarelyPerformer(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer& operator=(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer(BarelyPerformer&& other) noexcept = delete;
  BarelyPerformer& operator=(BarelyPerformer&& other) noexcept = delete;

 private:
  Observer<Musician> musician_;
};

// Task.
struct BarelyTask : public Task {
 public:
  BarelyTask(BarelyPerformerHandle performer, BarelyTaskDefinition definition, double position,
             void* user_data) noexcept
      : Task(definition, position, user_data,
             [this](Task* task, double position) { performer_->SetTaskPosition(task, position); }),
        performer_(performer->Observe()) {
    assert(performer_);
    performer_->AddTask(this);
  }

  ~BarelyTask() noexcept {
    if (performer_) {
      performer_->RemoveTask(this);
    }
  }

  /// Non-copyable and non-movable.
  BarelyTask(const BarelyTask& other) noexcept = delete;
  BarelyTask& operator=(const BarelyTask& other) noexcept = delete;
  BarelyTask(BarelyTask&& other) noexcept = delete;
  BarelyTask& operator=(BarelyTask&& other) noexcept = delete;

 private:
  Observer<Performer> performer_;
};

bool BarelyInstrument_Create(BarelyMusicianHandle musician,
                             BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (!out_instrument) return false;

  *out_instrument = new BarelyInstrument(musician);
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  delete instrument;
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, int32_t index,
                                 double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const double* value = instrument->GetControl(index); value != nullptr) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* value = instrument->GetNoteControl(pitch, index); value != nullptr) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, double pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = instrument->IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_Process(BarelyInstrumentHandle instrument, double* output_samples,
                              int32_t output_channel_count, int32_t output_frame_count,
                              double timestamp) {
  if (!instrument) return false;

  return instrument->Process(output_samples, output_channel_count, output_frame_count,
                             instrument->musician().GetFramesFromSeconds(timestamp));
}

bool BarelyInstrument_ResetAllControls(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->ResetAllControls();
  return true;
}

bool BarelyInstrument_ResetAllNoteControls(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  return instrument->ResetAllNoteControls(pitch);
}

bool BarelyInstrument_ResetControl(BarelyInstrumentHandle instrument, int32_t index) {
  if (!instrument) return false;

  return instrument->ResetControl(index);
}

bool BarelyInstrument_ResetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                       int32_t index) {
  if (!instrument) return false;

  return instrument->ResetNoteControl(pitch, index);
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, int32_t index, double value) {
  if (!instrument) return false;

  return instrument->SetControl(index, value);
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double value) {
  if (!instrument) return false;

  return instrument->SetNoteControl(pitch, index, value);
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                      BarelyNoteOffEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOffEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, double pitch, double intensity) {
  if (!instrument) return false;

  instrument->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                     BarelyNoteOnEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOnEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument,
                                    const BarelySampleDataDefinition* definitions,
                                    int32_t definition_count) {
  if (!instrument) return false;
  if (definition_count < 0 || (!definitions && definition_count > 0)) return false;

  instrument->SetSampleData(std::span<const barely::SampleDataDefinition>{
      reinterpret_cast<const barely::SampleDataDefinition*>(definitions),
      reinterpret_cast<const barely::SampleDataDefinition*>(definitions + definition_count)});
  return true;
}

bool BarelyMusician_Create(int32_t frame_rate, double reference_frequency,
                           BarelyMusicianHandle* out_musician) {
  if (frame_rate <= 0) return false;
  if (!out_musician) return false;

  *out_musician = new BarelyMusician(frame_rate, reference_frequency);
  return true;
}

bool BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_GetBeatsFromSeconds(BarelyMusicianHandle musician, double seconds,
                                        double* out_beats) {
  if (!musician) return false;
  if (!out_beats) return false;

  *out_beats = musician->GetBeatsFromSeconds(seconds);
  return true;
}

bool BarelyMusician_GetSecondsFromBeats(BarelyMusicianHandle musician, double beats,
                                        double* out_seconds) {
  if (!musician) return false;
  if (!out_seconds) return false;

  *out_seconds = musician->GetSecondsFromBeats(beats);
  return true;
}

bool BarelyMusician_GetTempo(BarelyMusicianHandle musician, double* out_tempo) {
  if (!musician) return false;
  if (!out_tempo) return false;

  *out_tempo = musician->GetTempo();
  return true;
}

bool BarelyMusician_GetTimestamp(BarelyMusicianHandle musician, double* out_timestamp) {
  if (!musician) return false;
  if (!out_timestamp) return false;

  *out_timestamp = musician->GetTimestamp();
  return true;
}

bool BarelyMusician_SetTempo(BarelyMusicianHandle musician, double tempo) {
  if (!musician) return false;

  musician->SetTempo(tempo);
  return true;
}

bool BarelyMusician_Update(BarelyMusicianHandle musician, double timestamp) {
  if (!musician) return false;

  musician->Update(timestamp);
  return true;
}

bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->CancelAllOneOffTasks();
  return true;
}

bool BarelyPerformer_Create(BarelyMusicianHandle musician, int32_t process_order,
                            BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer(musician, process_order);
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  delete performer;
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double* out_loop_begin_position) {
  if (!performer) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = performer->GetLoopBeginPosition();
  return true;
}

bool BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer, double* out_loop_length) {
  if (!performer) return false;
  if (!out_loop_length) return false;

  *out_loop_length = performer->GetLoopLength();
  return true;
}

bool BarelyPerformer_GetPosition(BarelyPerformerHandle performer, double* out_position) {
  if (!performer) return false;
  if (!out_position) return false;

  *out_position = performer->GetPosition();
  return true;
}

bool BarelyPerformer_IsLooping(BarelyPerformerHandle performer, bool* out_is_looping) {
  if (!performer) return false;
  if (!out_is_looping) return false;

  *out_is_looping = performer->IsLooping();
  return true;
}

bool BarelyPerformer_IsPlaying(BarelyPerformerHandle performer, bool* out_is_playing) {
  if (!performer) return false;
  if (!out_is_playing) return false;

  *out_is_playing = performer->IsPlaying();
  return true;
}

bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformerHandle performer,
                                        BarelyTaskDefinition definition, double position,
                                        void* user_data) {
  if (!performer) return false;

  performer->ScheduleOneOffTask(definition, position, user_data);
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double loop_begin_position) {
  if (!performer) return false;

  performer->SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer, double loop_length) {
  if (!performer) return false;

  performer->SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping) {
  if (!performer) return false;

  performer->SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position) {
  if (!performer) return false;

  performer->SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->Stop();
  return true;
}

bool BarelyTask_Create(BarelyPerformerHandle performer, BarelyTaskDefinition definition,
                       double position, void* user_data, BarelyTask** out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  *out_task = new BarelyTask(performer, definition, position, user_data);
  return *out_task;
}

bool BarelyTask_Destroy(BarelyTask* task) {
  if (!task) return false;

  delete task;
  return true;
}

bool BarelyTask_GetPosition(const BarelyTask* task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  *out_position = task->GetPosition();
  return true;
}

bool BarelyTask_SetPosition(BarelyTask* task, double position) {
  if (!task) return false;

  task->SetPosition(position);
  return true;
}
