#include "barelymusician/barelymusician.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/internal/engine.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

using ::barely::internal::Engine;
using ::barely::internal::Instrument;
using ::barely::internal::Observer;
using ::barely::internal::Performer;

// Effect.
struct BarelyEffect {
  // Internal instrument.
  Instrument* instrument;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyEffect_Create(BarelyInstrumentHandle instrument,
                                                BarelyEffectDefinition definition,
                                                int32_t process_order,
                                                BarelyEffectHandle* out_effect);
  friend BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffectHandle effect);

  // Default constructor.
  BarelyEffect() noexcept = default;

  // Default destructor.
  ~BarelyEffect() noexcept = default;

  // Non-copyable and non-movable.
  BarelyEffect(const BarelyEffect& other) noexcept = delete;
  BarelyEffect& operator=(const BarelyEffect& other) noexcept = delete;
  BarelyEffect(BarelyEffect&& other) noexcept = delete;
  BarelyEffect& operator=(BarelyEffect&& other) noexcept = delete;
};

// Instrument.
struct BarelyInstrument {
  // Internal engine.
  Engine& engine;

  // Internal instrument.
  Observer<Instrument> internal;

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyInstrument_Create(BarelyMusicianHandle musician,
                                                    BarelyInstrumentDefinition definition,
                                                    int32_t frame_rate,
                                                    BarelyInstrumentHandle* out_instrument);
  friend BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);

  // Constructs a new `BarelyInstrument` with a given `engine`, `definition`, and `frame_rate`.
  BarelyInstrument(Engine& engine, BarelyInstrumentDefinition definition,
                   int32_t frame_rate) noexcept
      : engine(engine), internal(engine.CreateInstrument(definition, frame_rate)) {}

  // Default destructor.
  ~BarelyInstrument() noexcept = default;

  // Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;
};

// Musician.
struct BarelyMusician {
  // Internal engine.
  Engine engine;

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
struct BarelyPerformer {
  // Internal engine.
  Engine& engine;

  // Internal performer.
  Observer<Performer> internal;

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyPerformer_Create(BarelyMusicianHandle musician,
                                                   BarelyPerformerHandle* out_performer);
  friend BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformerHandle performer);

  // Constructs a new `BarelyPerformer` with a given `engine`.
  explicit BarelyPerformer(Engine& engine) noexcept
      : engine(engine), internal(engine.CreatePerformer()) {}

  // Default destructor.
  ~BarelyPerformer() noexcept = default;

  // Non-copyable and non-movable.
  BarelyPerformer(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer& operator=(const BarelyPerformer& other) noexcept = delete;
  BarelyPerformer(BarelyPerformer&& other) noexcept = delete;
  BarelyPerformer& operator=(BarelyPerformer&& other) noexcept = delete;
};

// Task.
struct BarelyTask {
  // Internal performer.
  Performer* performer;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be managed via explicit API calls.
  friend BARELY_EXPORT bool BarelyTask_Create(BarelyPerformerHandle performer,
                                              BarelyTaskDefinition definition, double position,
                                              int32_t process_order, void* user_data,
                                              BarelyTaskHandle* out_task);
  friend BARELY_EXPORT bool BarelyTask_Destroy(BarelyTaskHandle task);

  // Default constructor.
  BarelyTask() noexcept = default;

  // Default destructor.
  ~BarelyTask() noexcept = default;

  // Non-copyable and non-movable.
  BarelyTask(const BarelyTask& other) noexcept = delete;
  BarelyTask& operator=(const BarelyTask& other) noexcept = delete;
  BarelyTask(BarelyTask&& other) noexcept = delete;
  BarelyTask& operator=(BarelyTask&& other) noexcept = delete;
};

bool BarelyEffect_Create(BarelyInstrumentHandle instrument, BarelyEffectDefinition definition,
                         int32_t process_order, BarelyEffectHandle* out_effect) {
  if (!instrument || !instrument->internal || !out_effect) return false;

  const auto effect_id_or = instrument->engine.CreateInstrumentEffect(instrument->internal.get(),
                                                                      definition, process_order);
  if (effect_id_or.has_value()) {
    *out_effect = new BarelyEffect();
    (*out_effect)->instrument = instrument->internal.get();
    (*out_effect)->id = *effect_id_or;
    return true;
  }
  return false;
}

bool BarelyEffect_Destroy(BarelyEffectHandle effect) {
  if (!effect || !effect->instrument) return false;

  const bool success = effect->instrument->DestroyEffect(effect->id);
  delete effect;
  return success;
}

bool BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t index, double* out_value) {
  if (!effect || !effect->instrument) return false;
  if (!out_value) return false;

  if (const auto* control = effect->instrument->GetEffectControl(effect->id, index)) {
    *out_value = control->GetValue();
    return true;
  }
  return false;
}

bool BarelyEffect_GetControlDefinition(BarelyEffectHandle effect, int32_t index,
                                       BarelyControlDefinition* out_definition) {
  if (!effect || !effect->instrument) return false;
  if (!out_definition) return false;

  if (const auto* control = effect->instrument->GetEffectControl(effect->id, index)) {
    *out_definition = control->GetDefinition();
    return true;
  }
  return false;
}

bool BarelyEffect_GetProcessOrder(BarelyEffectHandle effect, int32_t* out_process_order) {
  if (!effect || !effect->instrument) return false;
  if (!out_process_order) return false;

  const auto process_order_or = effect->instrument->GetEffectProcessOrder(effect->id);
  if (process_order_or.has_value()) {
    *out_process_order = *process_order_or;
    return true;
  }
  return false;
}

bool BarelyEffect_ResetAllControls(BarelyEffectHandle effect) {
  if (!effect || !effect->instrument) return false;

  return effect->instrument->ResetAllEffectControls(effect->id);
}

bool BarelyEffect_ResetControl(BarelyEffectHandle effect, int32_t index) {
  if (!effect || !effect->instrument) return false;

  return effect->instrument->ResetEffectControl(effect->id, index);
}

bool BarelyEffect_SetControl(BarelyEffectHandle effect, int32_t index, double value,
                             double slope_per_beat) {
  if (!effect || !effect->instrument) return false;

  return effect->instrument->SetEffectControl(effect->id, index, value, slope_per_beat);
}

bool BarelyEffect_SetControlEvent(BarelyEffectHandle effect,
                                  BarelyControlEventDefinition definition, void* user_data) {
  if (!effect || !effect->instrument) return false;

  effect->instrument->SetEffectControlEvent(effect->id, definition, user_data);
  return true;
}

bool BarelyEffect_SetData(BarelyEffectHandle effect, const void* data, int32_t size) {
  if (!effect || !effect->instrument) return false;

  return effect->instrument->SetEffectData(
      effect->id,
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
}

bool BarelyEffect_SetProcessOrder(BarelyEffectHandle effect, int32_t process_order) {
  if (!effect || !effect->instrument) return false;

  effect->instrument->SetEffectProcessOrder(effect->id, process_order);
  return true;
}

bool BarelyInstrument_Create(BarelyMusicianHandle musician, BarelyInstrumentDefinition definition,
                             int32_t frame_rate, BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (frame_rate <= 0) return false;
  if (!out_instrument) return false;

  *out_instrument = new BarelyInstrument(musician->engine, definition, frame_rate);
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  if (instrument->internal) {
    instrument->engine.DestroyInstrument(instrument->internal);
  }
  delete instrument;
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, int32_t index,
                                 double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* control = instrument->internal->GetControl(index)) {
    *out_value = control->GetValue();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetControlDefinition(BarelyInstrumentHandle instrument, int32_t index,
                                           BarelyControlDefinition* out_definition) {
  if (!instrument) return false;
  if (!out_definition) return false;

  if (const auto* control = instrument->internal->GetControl(index)) {
    *out_definition = control->GetDefinition();
    return true;
  }
  return false;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  if (const auto* note_control = instrument->internal->GetNoteControl(pitch, index)) {
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

  if (const auto* note_control = instrument->internal->GetNoteControl(pitch, index)) {
    *out_definition = note_control->GetDefinition();
    return true;
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, double pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  *out_is_note_on = instrument->internal->IsNoteOn(pitch);
  return true;
}

bool BarelyInstrument_Process(BarelyInstrumentHandle instrument, double* output_samples,
                              int32_t output_channel_count, int32_t output_frame_count,
                              double timestamp) {
  if (!instrument) return false;

  return instrument->internal->Process(output_samples, output_channel_count, output_frame_count,
                                       timestamp);
}

bool BarelyInstrument_ResetAllControls(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->internal->ResetAllControls();
  return true;
}

bool BarelyInstrument_ResetAllNoteControls(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  return instrument->internal->ResetAllNoteControls(pitch);
}

bool BarelyInstrument_ResetControl(BarelyInstrumentHandle instrument, int32_t index) {
  if (!instrument) return false;

  return instrument->internal->ResetControl(index);
}

bool BarelyInstrument_ResetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                       int32_t index) {
  if (!instrument) return false;

  return instrument->internal->ResetNoteControl(pitch, index);
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->internal->SetAllNotesOff();
  return true;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, int32_t index, double value,
                                 double slope_per_beat) {
  if (!instrument) return false;

  return instrument->internal->SetControl(index, value, slope_per_beat);
}

bool BarelyInstrument_SetControlEvent(BarelyInstrumentHandle instrument,
                                      BarelyControlEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->internal->SetControlEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetData(BarelyInstrumentHandle instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  instrument->internal->SetData(
      {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double value, double slope_per_beat) {
  if (!instrument) return false;

  return instrument->internal->SetNoteControl(pitch, index, value, slope_per_beat);
}

bool BarelyInstrument_SetNoteControlEvent(BarelyInstrumentHandle instrument,
                                          BarelyNoteControlEventDefinition definition,
                                          void* user_data) {
  if (!instrument) return false;

  instrument->internal->SetNoteControlEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  instrument->internal->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                      BarelyNoteOffEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  instrument->internal->SetNoteOffEvent(definition, user_data);
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, double pitch, double intensity) {
  if (!instrument) return false;

  instrument->internal->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                     BarelyNoteOnEventDefinition definition, void* user_data) {
  if (!instrument) return false;
  instrument->internal->SetNoteOnEvent(definition, user_data);
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

  *out_tempo = musician->engine.GetTempo();
  return true;
}

bool BarelyMusician_GetTimestamp(BarelyMusicianHandle musician, double* out_timestamp) {
  if (!musician) return false;
  if (!out_timestamp) return false;

  *out_timestamp = musician->engine.GetTimestamp();
  return true;
}

bool BarelyMusician_SetTempo(BarelyMusicianHandle musician, double tempo) {
  if (!musician) return false;

  musician->engine.SetTempo(tempo);
  return true;
}

bool BarelyMusician_Update(BarelyMusicianHandle musician, double timestamp) {
  if (!musician) return false;

  musician->engine.Update(timestamp);
  return true;
}

bool BarelyPerformer_Create(BarelyMusicianHandle musician, BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer(musician->engine);
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  if (!performer->internal) {
    performer->engine.DestroyPerformer(performer->internal);
  }
  delete performer;
  return true;
}

bool BarelyPerformer_GetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double* out_loop_begin_position) {
  if (!performer) return false;
  if (!out_loop_begin_position) return false;

  *out_loop_begin_position = performer->internal->GetLoopBeginPosition();
  return true;
}

bool BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer, double* out_loop_length) {
  if (!performer) return false;
  if (!out_loop_length) return false;

  *out_loop_length = performer->internal->GetLoopLength();
  return true;
}

bool BarelyPerformer_GetPosition(BarelyPerformerHandle performer, double* out_position) {
  if (!performer) return false;
  if (!out_position) return false;

  *out_position = performer->internal->GetPosition();
  return true;
}

bool BarelyPerformer_IsLooping(BarelyPerformerHandle performer, bool* out_is_looping) {
  if (!performer) return false;
  if (!out_is_looping) return false;

  *out_is_looping = performer->internal->IsLooping();
  return true;
}

bool BarelyPerformer_IsPlaying(BarelyPerformerHandle performer, bool* out_is_playing) {
  if (!performer) return false;
  if (!out_is_playing) return false;

  *out_is_playing = performer->internal->IsPlaying();
  return true;
}

bool BarelyPerformer_ScheduleOneOffTask(BarelyPerformerHandle performer,
                                        BarelyTaskDefinition definition, double position,
                                        int32_t process_order, void* user_data) {
  if (!performer || !performer->internal) return false;

  const auto task_id_or = performer->engine.CreatePerformerTask(performer->internal.get(),
                                                                definition, /*is_one_off=*/true,
                                                                position, process_order, user_data);
  return task_id_or.has_value();
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double loop_begin_position) {
  if (!performer) return false;

  performer->internal->SetLoopBeginPosition(loop_begin_position);
  return true;
}

bool BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer, double loop_length) {
  if (!performer) return false;

  performer->internal->SetLoopLength(loop_length);
  return true;
}

bool BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping) {
  if (!performer) return false;

  performer->internal->SetLooping(is_looping);
  return true;
}

bool BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position) {
  if (!performer) return false;

  performer->internal->SetPosition(position);
  return true;
}

bool BarelyPerformer_Start(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->internal->Start();
  return true;
}

bool BarelyPerformer_Stop(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->internal->Stop();
  return true;
}

bool BarelyTask_Create(BarelyPerformerHandle performer, BarelyTaskDefinition definition,
                       double position, int32_t process_order, void* user_data,
                       BarelyTaskHandle* out_task) {
  if (!performer || !performer->internal) return false;
  if (!out_task) return false;

  const auto task_id_or = performer->engine.CreatePerformerTask(performer->internal.get(),
                                                                definition, /*is_one_off=*/false,
                                                                position, process_order, user_data);
  if (task_id_or.has_value()) {
    *out_task = new BarelyTask();
    (*out_task)->performer = performer->internal.get();
    (*out_task)->id = *task_id_or;
    return true;
  }
  return false;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  const bool success = task->performer->DestroyTask(task->id);
  delete task;
  return success;
}

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  const auto position_or = task->performer->GetTaskPosition(task->id);
  if (position_or.has_value()) {
    *out_position = *position_or;
    return true;
  }
  return false;
}

bool BarelyTask_GetProcessOrder(BarelyTaskHandle task, int32_t* out_process_order) {
  if (!task) return false;
  if (!out_process_order) return false;

  const auto process_order_or = task->performer->GetTaskProcessOrder(task->id);
  if (process_order_or.has_value()) {
    *out_process_order = *process_order_or;
    return true;
  }
  return false;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return false;

  return task->performer->SetTaskPosition(task->id, position);
}

bool BarelyTask_SetProcessOrder(BarelyTaskHandle task, int32_t process_order) {
  if (!task) return false;

  return task->performer->SetTaskProcessOrder(task->id, process_order);
}
