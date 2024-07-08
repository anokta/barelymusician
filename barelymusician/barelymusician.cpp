#include "barelymusician/barelymusician.h"

#include <cstddef>
#include <cstdint>

#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

// Control.
struct BarelyControl : public barely::internal::Control {};

// Effect.
struct BarelyEffect : public barely::internal::Effect {};

// Instrument.
struct BarelyInstrument : public barely::internal::Instrument {};

// Musician.
struct BarelyMusician : public barely::internal::Musician {};

// Performer.
struct BarelyPerformer : public barely::internal::Performer {};

// Task.
struct BarelyTask : public barely::internal::Task {};

bool BarelyControl_GetValue(BarelyControlHandle control, double* out_value) {
  if (!control) return false;
  if (!out_value) return false;

  *out_value = control->GetValue();
  return true;
}

bool BarelyControl_ResetValue(BarelyControlHandle control) {
  if (!control) return false;

  control->ResetValue();
  return true;
}

bool BarelyControl_SetValue(BarelyControlHandle control, double value) {
  if (!control) return false;

  control->SetValue(value);
  return true;
}

bool BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t id,
                             BarelyControlHandle* out_control) {
  if (!effect) return false;
  if (!out_control) return false;

  *out_control = static_cast<BarelyControlHandle>(effect->GetControl(id));
  return *out_control;
}

bool BarelyEffect_Process(BarelyEffectHandle effect, double* output_samples,
                          int32_t output_channel_count, int32_t output_frame_count,
                          double timestamp) {
  if (!effect) return false;

  return effect->Process(output_samples, output_channel_count, output_frame_count, timestamp);
}

bool BarelyEffect_SetData(BarelyEffectHandle effect, const void* data, int32_t size) {
  if (!effect) return false;

  effect->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, int32_t id,
                                 BarelyControlHandle* out_control) {
  if (!instrument) return false;
  if (!out_control) return false;

  *out_control = static_cast<BarelyControlHandle>(instrument->GetControl(id));
  return *out_control;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t id,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* note_control = instrument->GetNoteControl(pitch, id)) {
    *out_value = note_control->GetValue();
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

  return instrument->Process(output_samples, output_channel_count, output_frame_count, timestamp);
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetData(BarelyInstrumentHandle instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  instrument->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
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

bool BarelyMusician_Create(BarelyMusicianHandle* out_musician) {
  if (!out_musician) return false;

  *out_musician = new BarelyMusician();
  return true;
}

bool BarelyMusician_CreateEffect(BarelyMusicianHandle musician, BarelyEffectDefinition definition,
                                 int32_t frame_rate, BarelyEffectHandle* out_effect) {
  if (!musician || !out_effect) return false;

  *out_effect = static_cast<BarelyEffectHandle>(musician->CreateEffect(definition, frame_rate));
  return true;
}

bool BarelyMusician_CreateInstrument(BarelyMusicianHandle musician,
                                     BarelyInstrumentDefinition definition, int32_t frame_rate,
                                     BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (frame_rate <= 0) return false;
  if (!out_instrument) return false;

  *out_instrument =
      static_cast<BarelyInstrumentHandle>(musician->CreateInstrument(definition, frame_rate));
  return true;
}

bool BarelyMusician_CreatePerformer(BarelyMusicianHandle musician, int32_t process_order,
                                    BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = static_cast<BarelyPerformerHandle>(musician->CreatePerformer(process_order));
  return true;
}

bool BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_DestroyEffect(BarelyMusicianHandle musician, BarelyEffectHandle effect) {
  if (!musician) return false;
  if (!effect) return false;

  return musician->DestroyEffect(effect);
}

bool BarelyMusician_DestroyInstrument(BarelyMusicianHandle musician,
                                      BarelyInstrumentHandle instrument) {
  if (!musician) return false;
  if (!instrument) return false;

  return musician->DestroyInstrument(instrument);
}

bool BarelyMusician_DestroyPerformer(BarelyMusicianHandle musician,
                                     BarelyPerformerHandle performer) {
  if (!musician) return false;
  if (!performer) return false;

  return musician->DestroyPerformer(performer);
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

bool BarelyPerformer_CreateTask(BarelyPerformerHandle performer, BarelyTaskDefinition definition,
                                double position, void* user_data, BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  *out_task = static_cast<BarelyTaskHandle>(performer->CreateTask(definition, position, user_data));
  return true;
}

bool BarelyPerformer_DestroyTask(BarelyPerformerHandle performer, BarelyTaskHandle task) {
  if (!performer) return false;
  if (!task) return false;

  performer->DestroyTask(task);
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

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  *out_position = task->GetPosition();
  return true;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return false;

  task->SetPosition(position);
  return true;
}
