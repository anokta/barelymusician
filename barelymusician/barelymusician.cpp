#include "barelymusician/barelymusician.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <atomic>
#include <cstddef>
#include <memory>

#include "barelymusician/internal/engine.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

// Effect.
struct BarelyEffect {
  // Default constructor.
  BarelyEffect() = default;

  // Internal instrument.
  barely::internal::Instrument* instrument;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffectHandle effect);
  ~BarelyEffect() = default;
};

// Instrument.
struct BarelyInstrument {
  // Constructs a new `BarelyInstrument` with a given `instrument`.
  BarelyInstrument(const std::shared_ptr<barely::internal::Engine>& engine,
                   barely::internal::Observer<barely::internal::Instrument> instrument) noexcept
      : engine(engine), internal(std::move(instrument)) {}

  // Internal engine.
  std::weak_ptr<barely::internal::Engine> engine;

  // Internal instrument.
  barely::internal::Observer<barely::internal::Instrument> internal;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);
  ~BarelyInstrument() = default;
};

// Musician.
struct BarelyMusician {
  // Default constructor.
  BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) noexcept = delete;
  BarelyMusician& operator=(const BarelyMusician& other) noexcept = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;

  // Internal engine.
  std::shared_ptr<barely::internal::Engine> engine = std::make_shared<barely::internal::Engine>();

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyMusician_Destroy(BarelyMusicianHandle musician);
  ~BarelyMusician() = default;
};

// Performer.
struct BarelyPerformer {
  // Default constructor.
  BarelyPerformer() = default;

  // Internal engine.
  std::weak_ptr<barely::internal::Engine> engine;

  // Internal performer.
  std::shared_ptr<barely::internal::Performer> internal = nullptr;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformerHandle performer);
  ~BarelyPerformer() = default;
};

// Task.
struct BarelyTask {
  // Default constructor.
  BarelyTask() = default;

  // Internal performer.
  barely::internal::Performer* performer;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyTask_Destroy(BarelyTaskHandle task);
  ~BarelyTask() = default;
};

bool BarelyEffect_Create(BarelyInstrumentHandle instrument, BarelyEffectDefinition definition,
                         int32_t process_order, BarelyEffectHandle* out_effect) {
  if (!instrument || !out_effect) return false;

  const auto effect_id_or = instrument->engine.lock()->CreateInstrumentEffect(
      instrument->internal.get(), definition, process_order);
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

  *out_instrument = new BarelyInstrument(
      musician->engine, musician->engine->CreateInstrument(definition, frame_rate));
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  if (auto engine = instrument->engine.lock()) {
    engine->DestroyInstrument(instrument->internal.get());
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

bool BarelyMusician_Create(BarelyMusicianHandle* out_handle) {
  if (!out_handle) return false;

  *out_handle = new BarelyMusician();
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

  *out_tempo = musician->engine->GetTempo();
  return true;
}

bool BarelyMusician_GetTimestamp(BarelyMusicianHandle musician, double* out_timestamp) {
  if (!musician) return false;
  if (!out_timestamp) return false;

  *out_timestamp = musician->engine->GetTimestamp();
  return true;
}

bool BarelyMusician_SetTempo(BarelyMusicianHandle musician, double tempo) {
  if (!musician) return false;

  musician->engine->SetTempo(tempo);
  return true;
}

bool BarelyMusician_Update(BarelyMusicianHandle musician, double timestamp) {
  if (!musician) return false;

  musician->engine->Update(timestamp);
  return true;
}

bool BarelyPerformer_Create(BarelyMusicianHandle musician, BarelyPerformerHandle* out_performer) {
  if (!musician) return false;
  if (!out_performer) return false;

  *out_performer = new BarelyPerformer();
  (*out_performer)->engine = musician->engine;
  (*out_performer)->internal = musician->engine->CreatePerformer();

  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  if (auto engine = performer->engine.lock()) {
    engine->DestroyPerformer(performer->internal.get());
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
  if (!performer) return false;
  if (performer->engine.expired()) return false;

  const auto task_id_or = performer->engine.lock()->CreatePerformerTask(
      performer->internal.get(), definition, /*is_one_off=*/true, position, process_order,
      user_data);
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
  if (!performer) return false;
  if (!out_task) return false;
  if (performer->engine.expired()) return false;

  const auto task_id_or = performer->engine.lock()->CreatePerformerTask(
      performer->internal.get(), definition, /*is_one_off=*/false, position, process_order,
      user_data);
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
