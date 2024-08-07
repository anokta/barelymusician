#include "barelymusician/barelymusician.h"

#include <cstddef>
#include <cstdint>

#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/note.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

using ::barely::internal::Control;
using ::barely::internal::ControlMap;
using ::barely::internal::Effect;
using ::barely::internal::Instrument;
using ::barely::internal::Musician;
using ::barely::internal::Note;
using ::barely::internal::Observable;
using ::barely::internal::Observer;
using ::barely::internal::Performer;
using ::barely::internal::Task;

// Control.
struct BarelyControl : public Control {};
static_assert(sizeof(BarelyControl) == sizeof(barely::internal::Control));

// Musician.
struct BarelyMusician : public Observable<Musician> {
  explicit BarelyMusician(int32_t frame_rate) noexcept : Observable<Musician>(frame_rate) {}
};

// Effect.
struct BarelyEffect : public Effect {
 public:
  BarelyEffect(BarelyMusician* musician, BarelyEffectDefinition definition) noexcept
      : Effect(definition, musician->GetFrameRate(), musician->GetTimestamp()),
        musician_(musician->Observe()) {
    assert(musician_);
    musician_->AddEffect(this);
  }

  ~BarelyEffect() noexcept {
    if (musician_) {
      musician_->RemoveEffect(this);
    }
  }

 private:
  Observer<Musician> musician_;
};

// Instrument.
struct BarelyInstrument : public Observable<Instrument> {
 public:
  BarelyInstrument(BarelyMusician* musician, BarelyInstrumentDefinition definition) noexcept
      : Observable<Instrument>(definition, musician->GetFrameRate(), musician->GetTimestamp()),
        musician_(musician->Observe()) {
    assert(musician_);
    musician_->AddInstrument(this);
  }

  ~BarelyInstrument() noexcept {
    if (musician_) {
      musician_->RemoveInstrument(this);
    }
  }

  int GenerateNextNoteId() noexcept { return ++note_id_counter_; }
  int GetCurrentNoteId() const noexcept { return note_id_counter_; }

 private:
  Observer<Musician> musician_;
  int note_id_counter_ = 0;
};

// Note.
struct BarelyNote : public Note {
  BarelyNote(BarelyInstrument* instrument, double pitch, double intensity) noexcept
      : Note(instrument->GenerateNextNoteId(), pitch, intensity,
             instrument->BuildNoteControlMap(instrument->GetCurrentNoteId())),
        instrument_(instrument->Observe()) {
    assert(instrument_);
    instrument_->AddNote(this);
  }

  ~BarelyNote() noexcept {
    if (instrument_) {
      instrument_->RemoveNote(this);
    }
  }

 private:
  Observer<Instrument> instrument_;
};

// Performer.
struct BarelyPerformer : public Observable<Performer> {
 public:
  BarelyPerformer(BarelyMusician* musician, int process_order) noexcept
      : Observable<Performer>(process_order), musician_(musician->Observe()) {
    assert(musician_);
    musician_->AddPerformer(this);
  }

  ~BarelyPerformer() noexcept {
    if (musician_) {
      musician_->RemovePerformer(this);
    }
  }

 private:
  Observer<Musician> musician_;
};

// Task.
struct BarelyTask : public Task {
 public:
  BarelyTask(BarelyPerformer* performer, BarelyTaskDefinition definition, double position,
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

 private:
  Observer<Performer> performer_;
};

bool BarelyControl_GetValue(const BarelyControl* control, double* out_value) {
  if (!control) return false;
  if (!out_value) return false;

  *out_value = control->GetValue();
  return true;
}

bool BarelyControl_ResetValue(BarelyControl* control) {
  if (!control) return false;

  control->ResetValue();
  return true;
}

bool BarelyControl_SetValue(BarelyControl* control, double value) {
  if (!control) return false;

  control->SetValue(value);
  return true;
}

bool BarelyEffect_Create(BarelyMusician* musician, BarelyEffectDefinition definition,
                         BarelyEffect** out_effect) {
  if (!musician || !out_effect) return false;

  *out_effect = new BarelyEffect(musician, definition);
  return true;
}

bool BarelyEffect_Destroy(BarelyEffect* effect) {
  if (!effect) return false;

  delete effect;
  return true;
}

bool BarelyEffect_GetControl(BarelyEffect* effect, int32_t control_id,
                             BarelyControl** out_control) {
  if (!effect) return false;
  if (!out_control) return false;

  *out_control = static_cast<BarelyControl*>(effect->GetControl(control_id));
  return *out_control;
}

bool BarelyEffect_Process(BarelyEffect* effect, double* output_samples,
                          int32_t output_channel_count, int32_t output_frame_count,
                          double timestamp) {
  if (!effect) return false;

  return effect->Process(output_samples, output_channel_count, output_frame_count, timestamp);
}

bool BarelyEffect_SetData(BarelyEffect* effect, const void* data, int32_t size) {
  if (!effect) return false;

  effect->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyInstrument_Create(BarelyMusician* musician, BarelyInstrumentDefinition definition,
                             BarelyInstrument** out_instrument) {
  if (!musician) return false;
  if (!out_instrument) return false;

  *out_instrument = new BarelyInstrument(musician, definition);
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrument* instrument) {
  if (!instrument) return false;

  delete instrument;
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrument* instrument, int32_t control_id,
                                 BarelyControl** out_control) {
  if (!instrument) return false;
  if (!out_control) return false;

  *out_control = static_cast<BarelyControl*>(instrument->GetControl(control_id));
  return *out_control;
}

bool BarelyInstrument_Process(BarelyInstrument* instrument, double* output_samples,
                              int32_t output_channel_count, int32_t output_frame_count,
                              double timestamp) {
  if (!instrument) return false;

  return instrument->Process(output_samples, output_channel_count, output_frame_count, timestamp);
}

bool BarelyInstrument_SetData(BarelyInstrument* instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  instrument->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyMusician_Create(int32_t frame_rate, BarelyMusician** out_musician) {
  if (frame_rate <= 0) return false;
  if (!out_musician) return false;

  *out_musician = new BarelyMusician(frame_rate);
  return true;
}

bool BarelyMusician_Destroy(BarelyMusician* musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_GetBeatsFromSeconds(const BarelyMusician* musician, double seconds,
                                        double* out_beats) {
  if (!musician) return false;
  if (!out_beats) return false;

  *out_beats = musician->GetBeatsFromSeconds(seconds);
  return true;
}

bool BarelyMusician_GetSecondsFromBeats(const BarelyMusician* musician, double beats,
                                        double* out_seconds) {
  if (!musician) return false;
  if (!out_seconds) return false;

  *out_seconds = musician->GetSecondsFromBeats(beats);
  return true;
}

bool BarelyMusician_GetTempo(const BarelyMusician* musician, double* out_tempo) {
  if (!musician) return false;
  if (!out_tempo) return false;

  *out_tempo = musician->GetTempo();
  return true;
}

bool BarelyMusician_GetTimestamp(const BarelyMusician* musician, double* out_timestamp) {
  if (!musician) return false;
  if (!out_timestamp) return false;

  *out_timestamp = musician->GetTimestamp();
  return true;
}

bool BarelyMusician_SetTempo(BarelyMusician* musician, double tempo) {
  if (!musician) return false;

  musician->SetTempo(tempo);
  return true;
}

bool BarelyMusician_Update(BarelyMusician* musician, double timestamp) {
  if (!musician) return false;

  musician->Update(timestamp);
  return true;
}

bool BarelyNote_Create(BarelyInstrument* instrument, double pitch, double intensity,
                       BarelyNote** out_note) {
  if (!instrument) return false;
  if (!out_note) return false;

  *out_note = new BarelyNote(instrument, pitch, intensity);
  return *out_note;
}

bool BarelyNote_Destroy(BarelyNote* note) {
  if (!note) return false;

  delete note;
  return true;
}

bool BarelyNote_GetControl(BarelyNote* note, int32_t control_id, BarelyControl** out_control) {
  if (!note) return false;
  if (!out_control) return false;

  *out_control = static_cast<BarelyControl*>(note->GetControl(control_id));
  return *out_control;
}

bool BarelyNote_GetIntensity(const BarelyNote* note, double* out_intensity) {
  if (!note) return false;
  if (!out_intensity) return false;

  *out_intensity = note->GetIntensity();
  return true;
}

bool BarelyNote_GetPitch(const BarelyNote* note, double* out_pitch) {
  if (!note) return false;
  if (!out_pitch) return false;

  *out_pitch = note->GetPitch();
  return true;
}

bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformer* performer) {
  if (!performer) return false;

  performer->CancelAllOneOffTasks();
  return true;
}

bool BarelyPerformer_Create(BarelyMusician* musician, int32_t process_order,
                            BarelyPerformer** out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer(musician, process_order);
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformer* performer) {
  if (!performer) return false;

  delete performer;
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(const BarelyPerformer* performer,
                                          double* out_loop_begin_position) {
  if (!performer) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = performer->GetLoopBeginPosition();
  return true;
}

bool BarelyPerformer_GetLoopLength(const BarelyPerformer* performer, double* out_loop_length) {
  if (!performer) return false;
  if (!out_loop_length) return false;

  *out_loop_length = performer->GetLoopLength();
  return true;
}

bool BarelyPerformer_GetPosition(const BarelyPerformer* performer, double* out_position) {
  if (!performer) return false;
  if (!out_position) return false;

  *out_position = performer->GetPosition();
  return true;
}

bool BarelyPerformer_IsLooping(const BarelyPerformer* performer, bool* out_is_looping) {
  if (!performer) return false;
  if (!out_is_looping) return false;

  *out_is_looping = performer->IsLooping();
  return true;
}

bool BarelyPerformer_IsPlaying(const BarelyPerformer* performer, bool* out_is_playing) {
  if (!performer) return false;
  if (!out_is_playing) return false;

  *out_is_playing = performer->IsPlaying();
  return true;
}

bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformer* performer, BarelyTaskDefinition definition,
                                        double position, void* user_data) {
  if (!performer) return false;

  performer->ScheduleOneOffTask(definition, position, user_data);
  return true;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformer* performer, double loop_begin_position) {
  if (!performer) return false;

  performer->SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyPerformer* performer, double loop_length) {
  if (!performer) return false;

  performer->SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyPerformer* performer, bool is_looping) {
  if (!performer) return false;

  performer->SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyPerformer* performer, double position) {
  if (!performer) return false;

  performer->SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyPerformer* performer) {
  if (!performer) return false;

  performer->Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyPerformer* performer) {
  if (!performer) return false;

  performer->Stop();
  return true;
}

bool BarelyTask_Create(BarelyPerformer* performer, BarelyTaskDefinition definition, double position,
                       void* user_data, BarelyTask** out_task) {
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
