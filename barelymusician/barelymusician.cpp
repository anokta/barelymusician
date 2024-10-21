#include "barelymusician/barelymusician.h"

#include <cassert>
#include <cstdint>
#include <span>

#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/performer.h"

using ::barely::ControlType;
using ::barely::InstrumentController;
using ::barely::NoteControlType;
using ::barely::Performer;
using ::barely::SampleDataSlice;
using ::barely::internal::Musician;

// Musician.
struct BarelyMusician : public Musician {
 public:
  explicit BarelyMusician(int32_t frame_rate) noexcept : Musician(frame_rate) {}
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
struct BarelyTask : public Performer::Task {};

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, BarelyControlType type,
                                 double* out_value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = instrument->GetControl(static_cast<ControlType>(type));
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                     BarelyNoteControlType type, double* out_value) {
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

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, BarelyControlType type,
                                 double value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyControlType_kCount) return false;

  instrument->SetControl(static_cast<ControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                     BarelyNoteControlType type, double value) {
  if (!instrument) return false;
  if (type < 0 || type >= BarelyNoteControlType_kCount) return false;

  instrument->SetNoteControl(pitch, static_cast<NoteControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                      BarelyNoteOffEvent note_off_event, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOffEvent(note_off_event, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, double pitch, double intensity) {
  if (!instrument) return false;

  instrument->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                     BarelyNoteOnEvent note_on_event, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOnEvent(note_on_event, user_data);
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

bool BarelyMusician_Create(int32_t frame_rate, BarelyMusicianHandle* out_musician) {
  if (frame_rate <= 0) return false;
  if (!out_musician) return false;

  *out_musician = new BarelyMusician(frame_rate);
  return true;
}

bool BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return false;

  delete musician;
  return true;
}

bool BarelyMusician_GetReferenceFrequency(BarelyMusicianHandle musician,
                                          double* out_reference_frequency) {
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

bool BarelyMusician_SetReferenceFrequency(BarelyMusicianHandle musician,
                                          double reference_frequency) {
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

bool BarelyPerformer_AddTask(BarelyPerformerHandle performer, BarelyTaskEvent task_event,
                             double position, void* user_data, BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  *out_task = static_cast<BarelyTask*>(performer->AddTask(task_event, position, user_data));
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

bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformerHandle performer, BarelyTaskEvent task_event,
                                        double position, void* user_data) {
  if (!performer) return false;

  performer->ScheduleOneOffTask(task_event, position, user_data);
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
