#include "barelymusician/barelymusician.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"
#include "barelymusician/internal/task.h"

using ::barely::internal::Effect;
using ::barely::internal::Instrument;
using ::barely::internal::Musician;
using ::barely::internal::Observable;
using ::barely::internal::Observer;
using ::barely::internal::Performer;
using ::barely::internal::Task;

// Effect.
struct BarelyEffect : public Effect {
 public:
  // Returns the instrument.
  Instrument* instrument() const noexcept { return instrument_.get(); }

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyEffect_Create(BarelyInstrumentHandle instrument,
                                                BarelyEffectDefinition definition,
                                                int32_t process_order,
                                                BarelyEffectHandle* out_effect);
  friend BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffectHandle effect);

  // Constructs `BarelyEffect` with `definition`, `instrument`, and `process_order`.
  BarelyEffect(const Observable<Instrument>& instrument, BarelyEffectDefinition definition,
               int process_order) noexcept
      : Effect(definition, instrument.GetFrameRate(), process_order),
        instrument_(instrument.Observe()) {
    instrument_->AddEffect(*this);
  }

  // Destroys `BarelyEffect`.
  ~BarelyEffect() noexcept {
    if (instrument_) {
      instrument_->RemoveEffect(*this);
    }
  }

  // Non-copyable and non-movable.
  BarelyEffect(const BarelyEffect& other) noexcept = delete;
  BarelyEffect& operator=(const BarelyEffect& other) noexcept = delete;
  BarelyEffect(BarelyEffect&& other) noexcept = delete;
  BarelyEffect& operator=(BarelyEffect&& other) noexcept = delete;

  // Internal instrument.
  Observer<Instrument> instrument_;
};

// Instrument.
struct BarelyInstrument : public Observable<Instrument> {
 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusicianHandle musician,
                                                    BarelyInstrumentDefinition definition,
                                                    int32_t frame_rate,
                                                    BarelyInstrumentHandle* out_instrument);
  friend BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);

  // Constructs `BarelyInstrument` with `musician`, `definition`, and `frame_rate`.
  BarelyInstrument(const Observable<Musician>& musician, BarelyInstrumentDefinition definition,
                   int32_t frame_rate) noexcept
      : Observable<Instrument>(definition, frame_rate, musician.GetTempo(),
                               musician.GetTimestamp()),
        musician_(musician.Observe()) {
    musician_->AddInstrument(*this);
  }

  // Destroys `BarelyInstrument`.
  ~BarelyInstrument() noexcept {
    if (musician_) {
      musician_->RemoveInstrument(*this);
    }
  }

  // Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

  // Internal musician.
  Observer<Musician> musician_;
};

// Musician.
struct BarelyMusician : public Observable<Musician> {
 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyMusician_Create(BarelyMusicianHandle* out_musician);
  friend BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusicianHandle musician);

  // Default constructor.
  BarelyMusician() noexcept = default;

  // Default destructor.
  ~BarelyMusician() noexcept = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) noexcept = delete;
  BarelyMusician& operator=(const BarelyMusician& other) noexcept = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;
};

// Performer.
struct BarelyPerformer : public Observable<Performer> {
 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyPerformer_Create(BarelyMusicianHandle musician,
                                                   BarelyPerformerHandle* out_performer);
  friend BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformerHandle performer);

  // Constructs `BarelyPerformer` with `musician`.
  explicit BarelyPerformer(const Observable<Musician>& musician) noexcept
      : Observable<Performer>(), musician_(musician.Observe()) {
    musician_->AddPerformer(*this);
  }

  // Destroys `BarelyPerformer`.
  ~BarelyPerformer() noexcept {
    if (musician_) {
      musician_->RemovePerformer(*this);
    }
  }

  // Non-copyable and non-movable.
  BarelyPerformer(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer& operator=(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer(BarelyPerformer&& other) noexcept = delete;
  BarelyPerformer& operator=(BarelyPerformer&& other) noexcept = delete;

  // Internal musician.
  Observer<Musician> musician_;
};

// Task.
struct BarelyTask : public Task {
 public:
  // Returns the performer.
  Performer* performer() const noexcept { return performer_.get(); }

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyTask_Create(BarelyPerformerHandle performer,
                                              BarelyTaskDefinition definition, double position,
                                              int32_t process_order, void* user_data,
                                              BarelyTaskHandle* out_task);
  friend BARELY_EXPORT bool BarelyTask_Destroy(BarelyTaskHandle task);

  // Constructs `BarelyTask` with `performer`, `definition`, `position`, `process_order`, and
  // `user_data`.
  BarelyTask(const Observable<Performer>& performer, BarelyTaskDefinition definition,
             double position, int process_order, void* user_data) noexcept
      : Task(definition, position, process_order, user_data), performer_(performer.Observe()) {
    performer_->AddTask(*this);
  }

  // Destroys `BarelyTask`.
  ~BarelyTask() noexcept {
    if (performer_) {
      performer_->RemoveTask(*this);
    }
  }

  // Non-copyable and non-movable.
  BarelyTask(const BarelyTask& other) noexcept = delete;
  BarelyTask& operator=(const BarelyTask& other) noexcept = delete;
  BarelyTask(BarelyTask&& other) noexcept = delete;
  BarelyTask& operator=(BarelyTask&& other) noexcept = delete;

  // Internal performer.
  Observer<Performer> performer_;
};

bool BarelyEffect_Create(BarelyInstrumentHandle instrument, BarelyEffectDefinition definition,
                         int32_t process_order, BarelyEffectHandle* out_effect) {
  if (!instrument || !out_effect) return false;

  *out_effect = new BarelyEffect(*instrument, definition, process_order);
  return true;
}

bool BarelyEffect_Destroy(BarelyEffectHandle effect) {
  if (!effect) return false;

  delete effect;
  return true;
}

bool BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t index, double* out_value) {
  if (!effect) return false;
  if (!out_value) return false;

  if (const auto* control = effect->GetControl(index)) {
    *out_value = control->GetValue();
    return true;
  }
  return false;
}

bool BarelyEffect_GetControlDefinition(BarelyEffectHandle effect, int32_t index,
                                       BarelyControlDefinition* out_definition) {
  if (!effect) return false;
  if (!out_definition) return false;

  if (const auto* control = effect->GetControl(index)) {
    *out_definition = control->GetDefinition();
    return true;
  }
  return false;
}

bool BarelyEffect_GetProcessOrder(BarelyEffectHandle effect, int32_t* out_process_order) {
  if (!effect) return false;
  if (!out_process_order) return false;

  *out_process_order = effect->GetProcessOrder();
  return true;
}

bool BarelyEffect_ResetAllControls(BarelyEffectHandle effect) {
  if (!effect || !effect->instrument()) return false;

  effect->instrument()->ResetAllEffectControls(*effect);
  return true;
}

bool BarelyEffect_ResetControl(BarelyEffectHandle effect, int32_t index) {
  if (!effect || !effect->instrument()) return false;

  return effect->instrument()->ResetEffectControl(*effect, index);
}

bool BarelyEffect_SetControl(BarelyEffectHandle effect, int32_t index, double value,
                             double slope_per_beat) {
  if (!effect || !effect->instrument()) return false;

  return effect->instrument()->SetEffectControl(*effect, index, value, slope_per_beat);
}

bool BarelyEffect_SetControlEvent(BarelyEffectHandle effect,
                                  BarelyControlEventDefinition definition, void* user_data) {
  if (!effect) return false;

  effect->SetControlEvent(definition, user_data);
  return true;
}

bool BarelyEffect_SetData(BarelyEffectHandle effect, const void* data, int32_t size) {
  if (!effect || !effect->instrument()) return false;

  effect->instrument()->SetEffectData(
      *effect, {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyEffect_SetProcessOrder(BarelyEffectHandle effect, int32_t process_order) {
  if (!effect || !effect->instrument()) return false;

  effect->instrument()->SetEffectProcessOrder(*effect, process_order);
  return true;
}

bool BarelyInstrument_Create(BarelyMusicianHandle musician, BarelyInstrumentDefinition definition,
                             int32_t frame_rate, BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (frame_rate <= 0) return false;
  if (!out_instrument) return false;

  *out_instrument = new BarelyInstrument(*musician, definition, frame_rate);
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

  if (const auto* control = instrument->GetControl(index)) {
    *out_value = control->GetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetControlDefinition(BarelyInstrumentHandle instrument, int32_t index,
                                           BarelyControlDefinition* out_definition) {
  if (!instrument) return false;
  if (!out_definition) return false;

  if (const auto* control = instrument->GetControl(index)) {
    *out_definition = control->GetDefinition();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* note_control = instrument->GetNoteControl(pitch, index)) {
    *out_value = note_control->GetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetNoteControlDefinition(BarelyInstrumentHandle instrument, double pitch,
                                               int32_t index,
                                               BarelyControlDefinition* out_definition) {
  if (!instrument) return false;
  if (!out_definition) return false;

  if (const auto* note_control = instrument->GetNoteControl(pitch, index)) {
    *out_definition = note_control->GetDefinition();
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

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, int32_t index, double value,
                                 double slope_per_beat) {
  if (!instrument) return false;

  return instrument->SetControl(index, value, slope_per_beat);
}

bool BarelyInstrument_SetControlEvent(BarelyInstrumentHandle instrument,
                                      BarelyControlEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->SetControlEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetData(BarelyInstrumentHandle instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  instrument->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double value, double slope_per_beat) {
  if (!instrument) return false;

  return instrument->SetNoteControl(pitch, index, value, slope_per_beat);
}

bool BarelyInstrument_SetNoteControlEvent(BarelyInstrumentHandle instrument,
                                          BarelyNoteControlEventDefinition definition,
                                          void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteControlEvent(definition, user_data);
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

bool BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return false;

  delete musician;
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

bool BarelyPerformer_Create(BarelyMusicianHandle musician, BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer(*musician);
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
                                        int32_t process_order, void* user_data) {
  if (!performer) return false;

  performer->ScheduleOneOffTask(definition, position, process_order, user_data);
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
                       double position, int32_t process_order, void* user_data,
                       BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  (*out_task) = new BarelyTask(*performer, definition, position, process_order, user_data);
  return true;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  delete task;
  return true;
}

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  *out_position = task->GetPosition();
  return true;
}

bool BarelyTask_GetProcessOrder(BarelyTaskHandle task, int32_t* out_process_order) {
  if (!task) return false;
  if (!out_process_order) return false;

  *out_process_order = task->GetProcessOrder();
  return true;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task || !task->performer()) return false;

  task->performer()->SetTaskPosition(*task, position);
  return true;
}

bool BarelyTask_SetProcessOrder(BarelyTaskHandle task, int32_t process_order) {
  if (!task || !task->performer()) return false;

  task->performer()->SetTaskProcessOrder(*task, process_order);
  return true;
}
