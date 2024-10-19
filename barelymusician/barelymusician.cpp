#include "barelymusician/barelymusician.h"

#include <cassert>
#include <cstdint>

#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

using ::barely::InstrumentController;
using ::barely::InstrumentControlType;
using ::barely::internal::Musician;
using ::barely::internal::Performer;
using ::barely::internal::Task;

// Musician.
struct BarelyMusician : public Musician {
 public:
  BarelyMusician(int32_t frame_rate, double reference_frequency) noexcept
      : Musician(frame_rate, reference_frequency) {}
  ~BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) noexcept = delete;
  BarelyMusician& operator=(const BarelyMusician& other) noexcept = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;
};

// Instrument.
struct BarelyInstrument : public InstrumentController {};

// Performer.
struct BarelyPerformer : public Performer {};

// Task.
struct BarelyTask : public Task {};

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                 BarelyInstrumentControlType type, double* out_value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyInstrumentControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = instrument->GetControl(static_cast<InstrumentControlType>(type));
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                     BarelyInstrumentControlType type, double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* value =
          instrument->GetNoteControl(pitch, static_cast<InstrumentControlType>(type));
      value != nullptr) {
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

  return instrument->Process(
      output_samples, output_channel_count, output_frame_count,
      static_cast<int>(static_cast<double>(instrument->GetFrameRate()) * timestamp));
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument,
                                 BarelyInstrumentControlType type, double value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyInstrumentControlType_kCount) return false;

  instrument->SetControl(static_cast<InstrumentControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                     BarelyInstrumentControlType type, double value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyInstrumentControlType_kCount) return false;

  instrument->SetNoteControl(pitch, static_cast<InstrumentControlType>(type), value);
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

bool BarelyMusician_AddInstrument(BarelyMusicianHandle musician,
                                  BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (!out_instrument) return false;

  *out_instrument = static_cast<BarelyInstrument*>(musician->AddInstrument());
  return true;
}

bool BarelyMusician_AddPerformer(BarelyMusicianHandle musician, int32_t process_order,
                                 BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = static_cast<BarelyPerformer*>(musician->AddPerformer(process_order));
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

bool BarelyMusician_RemoveInstrument(BarelyMusicianHandle musician,
                                     BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  musician->RemoveInstrument(instrument);
  return true;
}

bool BarelyMusician_RemovePerformer(BarelyMusicianHandle musician,
                                    BarelyPerformerHandle performer) {
  if (!performer) return false;

  musician->RemovePerformer(performer);
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

bool BarelyPerformer_AddTask(BarelyPerformerHandle performer, BarelyTaskDefinition definition,
                             double position, void* user_data, BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  *out_task = static_cast<BarelyTask*>(performer->AddTask(definition, position, user_data));
  return *out_task;
}

bool BarelyPerformer_CancelAllOneOffTasks(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->CancelAllOneOffTasks();
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

bool BarelyPerformer_RemoveTask(BarelyPerformerHandle performer, BarelyTaskHandle task) {
  if (!performer) return false;
  if (!task) return false;

  performer->RemoveTask(task);
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

bool BarelyTask_GetPosition(const BarelyTaskHandle task, double* out_position) {
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
