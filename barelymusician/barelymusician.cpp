#include "barelymusician/barelymusician.h"

#include <cstddef>
#include <cstdint>

#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/note.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

// Control.
struct BarelyControl : public barely::internal::Control {};
static_assert(sizeof(BarelyControl) == sizeof(barely::internal::Control));

// Effect.
struct BarelyEffect : public barely::internal::Effect {};
static_assert(sizeof(BarelyEffect) == sizeof(barely::internal::Effect));

// Instrument.
struct BarelyInstrument : public barely::internal::Instrument {};
static_assert(sizeof(BarelyInstrument) == sizeof(barely::internal::Instrument));

// Musician.
struct BarelyMusician : public barely::internal::Musician {};
static_assert(sizeof(BarelyMusician) == sizeof(barely::internal::Musician));

// Note.
struct BarelyNote : public barely::internal::Note {};
static_assert(sizeof(BarelyNote) == sizeof(barely::internal::Note));

// Performer.
struct BarelyPerformer : public barely::internal::Performer {};
static_assert(sizeof(BarelyPerformer) == sizeof(barely::internal::Performer));

// Task.
struct BarelyTask : public barely::internal::Task {};
static_assert(sizeof(BarelyTask) == sizeof(barely::internal::Task));

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

bool BarelyInstrument_CreateNote(BarelyInstrument* instrument, double pitch, double intensity,
                                 BarelyNote** out_note) {
  if (!instrument) return false;
  if (!out_note) return false;

  *out_note = static_cast<BarelyNote*>(instrument->CreateNote(pitch, intensity));
  return *out_note;
}

bool BarelyInstrument_DestroyNote(BarelyInstrument* instrument, BarelyNote* note) {
  if (!instrument) return false;

  instrument->DestroyNote(note);
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

bool BarelyMusician_Create(BarelyMusician** out_musician) {
  if (!out_musician) return false;

  *out_musician = new BarelyMusician();
  return true;
}

bool BarelyMusician_CreateEffect(BarelyMusician* musician, BarelyEffectDefinition definition,
                                 int32_t frame_rate, BarelyEffect** out_effect) {
  if (!musician || !out_effect) return false;

  *out_effect = static_cast<BarelyEffect*>(musician->CreateEffect(definition, frame_rate));
  return true;
}

bool BarelyMusician_CreateInstrument(BarelyMusician* musician,
                                     BarelyInstrumentDefinition definition, int32_t frame_rate,
                                     BarelyInstrument** out_instrument) {
  if (!musician) return false;
  if (frame_rate <= 0) return false;
  if (!out_instrument) return false;

  *out_instrument =
      static_cast<BarelyInstrument*>(musician->CreateInstrument(definition, frame_rate));
  return true;
}

bool BarelyMusician_CreatePerformer(BarelyMusician* musician, int32_t process_order,
                                    BarelyPerformer** out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = static_cast<BarelyPerformer*>(musician->CreatePerformer(process_order));
  return true;
}

bool BarelyMusician_Destroy(BarelyMusician* musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_DestroyEffect(BarelyMusician* musician, BarelyEffect* effect) {
  if (!musician) return false;
  if (!effect) return false;

  return musician->DestroyEffect(effect);
}

bool BarelyMusician_DestroyInstrument(BarelyMusician* musician, BarelyInstrument* instrument) {
  if (!musician) return false;
  if (!instrument) return false;

  return musician->DestroyInstrument(instrument);
}

bool BarelyMusician_DestroyPerformer(BarelyMusician* musician, BarelyPerformer* performer) {
  if (!musician) return false;
  if (!performer) return false;

  return musician->DestroyPerformer(performer);
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

bool BarelyPerformer_CreateTask(BarelyPerformer* performer, BarelyTaskDefinition definition,
                                double position, void* user_data, BarelyTask** out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  *out_task = static_cast<BarelyTask*>(performer->CreateTask(definition, position, user_data));
  return *out_task;
}

bool BarelyPerformer_DestroyTask(BarelyPerformer* performer, BarelyTask* task) {
  if (!performer) return false;
  if (!task) return false;

  performer->DestroyTask(task);
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
