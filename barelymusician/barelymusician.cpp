#include "barelymusician/barelymusician.h"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

using ::barely::internal::Instrument;
using ::barely::internal::Musician;
using ::barely::internal::Observable;
using ::barely::internal::Observer;
using ::barely::internal::Performer;
using ::barely::internal::Task;

// Musician.
struct BarelyMusician : public Observable<Musician> {
 public:
  explicit BarelyMusician(int32_t frame_rate) noexcept : Observable<Musician>(frame_rate) {}
  ~BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) noexcept = delete;
  BarelyMusician& operator=(const BarelyMusician& other) noexcept = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;
};

// Instrument.
struct BarelyInstrument : public Observable<Instrument> {
 public:
  BarelyInstrument(BarelyMusician* musician, BarelyInstrumentDefinition definition) noexcept
      : Observable<Instrument>(definition, musician->GetFrameRate(), musician->GetUpdateFrame()),
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

  /// Non-copyable and non-movable.
  BarelyTask(const BarelyTask& other) noexcept = delete;
  BarelyTask& operator=(const BarelyTask& other) noexcept = delete;
  BarelyTask(BarelyTask&& other) noexcept = delete;
  BarelyTask& operator=(BarelyTask&& other) noexcept = delete;

 private:
  Observer<Performer> performer_;
};

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

bool BarelyInstrument_GetControl(const BarelyInstrument* instrument, int32_t id,
                                 double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* control = instrument->GetControl(id); control != nullptr) {
    *out_value = control->GetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetNoteControl(const BarelyInstrument* instrument, double pitch, int32_t id,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* note_control = instrument->GetNoteControl(pitch, id); note_control != nullptr) {
    *out_value = note_control->GetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(const BarelyInstrument* instrument, double pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = instrument->IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_Process(BarelyInstrument* instrument, double* output_samples,
                              int32_t output_channel_count, int32_t output_frame_count,
                              double timestamp) {
  if (!instrument) return false;

  return instrument->Process(output_samples, output_channel_count, output_frame_count,
                             instrument->musician().GetFramesFromSeconds(timestamp));
}

bool BarelyInstrument_ResetAllControls(BarelyInstrument* instrument) {
  if (!instrument) return false;

  instrument->ResetAllControls();
  return true;
}

bool BarelyInstrument_ResetAllNoteControls(BarelyInstrument* instrument, double pitch) {
  if (!instrument) return false;

  return instrument->ResetAllNoteControls(pitch);
}

bool BarelyInstrument_ResetControl(BarelyInstrument* instrument, int32_t id) {
  if (!instrument) return false;

  if (auto* control = instrument->GetControl(id); control != nullptr) {
    control->ResetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_ResetNoteControl(BarelyInstrument* instrument, double pitch, int32_t id) {
  if (!instrument) return false;

  if (auto* note_control = instrument->GetNoteControl(pitch, id); note_control != nullptr) {
    note_control->ResetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrument* instrument) {
  if (!instrument) return false;

  instrument->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrument* instrument, int32_t id, double value) {
  if (!instrument) return false;

  if (auto* control = instrument->GetControl(id); control != nullptr) {
    control->SetValue(value);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetControlEvent(BarelyInstrument* instrument,
                                      BarelyControlEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetControlEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetData(BarelyInstrument* instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  instrument->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrument* instrument, double pitch, int32_t id,
                                     double value) {
  if (!instrument) return false;

  if (auto* note_control = instrument->GetNoteControl(pitch, id); note_control != nullptr) {
    note_control->SetValue(value);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteControlEvent(BarelyInstrument* instrument,
                                          BarelyNoteControlEventDefinition definition,
                                          void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteControlEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrument* instrument, double pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrument* instrument,
                                      BarelyNoteOffEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOffEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrument* instrument, double pitch, double intensity) {
  if (!instrument) return false;

  instrument->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrument* instrument,
                                     BarelyNoteOnEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOnEvent(definition, user_data);
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
