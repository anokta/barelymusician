#include "barelymusician.h"

#include <cassert>
#include <cstdint>
#include <span>

#include "engine/instrument.h"
#include "engine/musician.h"
#include "engine/performer.h"

using ::barely::ControlType;
using ::barely::NoteControlType;
using ::barely::NoteOffEvent;
using ::barely::NoteOnEvent;
using ::barely::SampleDataSlice;

bool BarelyInstrument_Create(BarelyMusicianHandle musician,
                             BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (!out_instrument) return false;

  *out_instrument = static_cast<BarelyInstrument*>(musician->CreateInstrument());
  // TODO(#147): Temp hack to allow destroying by handle.
  (*out_instrument)->musician = musician;
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->musician->DestroyInstrument(instrument);
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, BarelyControlType type,
                                 float* out_value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = instrument->GetControl(static_cast<ControlType>(type));
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float* out_value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyNoteControlType_kCount) return false;
  if (!out_value) return false;

  if (const auto* value = instrument->GetNoteControl(pitch, static_cast<NoteControlType>(type));
      value != nullptr) {
    *out_value = *value;
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, float pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = instrument->IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_Process(BarelyInstrumentHandle instrument, float* output_samples,
                              int32_t output_sample_count, double timestamp) {
  if (!instrument) return false;

  return instrument->Process(
      {output_samples, output_samples + output_sample_count},
      static_cast<int>(static_cast<double>(instrument->GetSampleRate()) * timestamp));
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, BarelyControlType type,
                                 float value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyControlType_kCount) return false;

  instrument->SetControl(static_cast<ControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyNoteControlType_kCount) return false;

  instrument->SetNoteControl(pitch, static_cast<NoteControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, float pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                      const BarelyNoteOffEvent* note_off_event) {
  if (!instrument) return false;

  instrument->SetNoteOffEvent((note_off_event != nullptr) ? *note_off_event : NoteOffEvent{});
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, float pitch, float intensity) {
  if (!instrument) return false;

  instrument->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                     const BarelyNoteOnEvent* note_on_event) {
  if (!instrument) return false;

  instrument->SetNoteOnEvent((note_on_event != nullptr) ? *note_on_event : NoteOnEvent{});
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument,
                                    const BarelySampleDataSlice* slices, int32_t slice_count) {
  if (!instrument) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  instrument->SetSampleData(std::span<const SampleDataSlice>{
      reinterpret_cast<const SampleDataSlice*>(slices),
      reinterpret_cast<const SampleDataSlice*>(slices + slice_count)});
  return true;
}

bool BarelyMusician_Create(int32_t sample_rate, BarelyMusicianHandle* out_musician) {
  if (sample_rate <= 0) return false;
  if (!out_musician) return false;

  *out_musician = new BarelyMusician(sample_rate);
  return true;
}

bool BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_GetReferenceFrequency(BarelyMusicianHandle musician,
                                          float* out_reference_frequency) {
  if (!musician) return false;
  if (!out_reference_frequency) return false;

  *out_reference_frequency = musician->GetReferenceFrequency();
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

bool BarelyMusician_SetReferenceFrequency(BarelyMusicianHandle musician,
                                          float reference_frequency) {
  if (!musician) return false;

  musician->SetReferenceFrequency(reference_frequency);
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

bool BarelyPerformer_Create(BarelyMusicianHandle musician, int32_t process_order,
                            BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = static_cast<BarelyPerformer*>(musician->CreatePerformer(process_order));
  (*out_performer)->musician = musician;
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->musician->DestroyPerformer(performer);
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

bool BarelyPerformer_SetBeatCallback(BarelyPerformerHandle performer,
                                     BarelyBeatCallback beat_callback, void* user_data) {
  if (!performer) return false;

  performer->SetBeatCallback(beat_callback, user_data);
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

bool BarelyTask_Create(BarelyPerformerHandle performer, const BarelyTaskEvent* task_event,
                       double position, BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!task_event) return false;
  if (!out_task) return false;

  *out_task = static_cast<BarelyTask*>(performer->CreateTask(*task_event, position));
  // TODO(#147): Temp hack to allow destroying by handle.
  (*out_task)->performer = performer;
  return *out_task;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  task->performer->DestroyTask(task);
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
