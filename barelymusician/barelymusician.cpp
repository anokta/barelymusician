#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/internal/engine.h"

// Effect.
struct BarelyEffect {
  // Default constructor.
  BarelyEffect() = default;

  // Internal engine.
  barely::internal::Engine* engine = nullptr;

  // Instrument identifier.
  barely::internal::Id instrument_id = barely::internal::kInvalid;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyEffect_Destroy(BarelyEffectHandle effect);
  ~BarelyEffect() = default;
};

// Instrument.
struct BarelyInstrument {
  // Default constructor.
  BarelyInstrument() = default;

  // Internal engine.
  barely::internal::Engine* engine = nullptr;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

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
  barely::internal::Engine engine;

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
  barely::internal::Engine* engine = nullptr;

  // Identifier.
  barely::internal::Id id = barely::internal::kInvalid;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyPerformer_Destroy(BarelyPerformerHandle performer);
  ~BarelyPerformer() = default;
};

// Task.
struct BarelyTask {
  // Default constructor.
  BarelyTask() = default;

  // Internal engine.
  barely::internal::Engine* engine = nullptr;

  // Performer identifier.
  barely::internal::Id performer_id = barely::internal::kInvalid;

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

  const auto effect_id_or =
      instrument->engine->CreateInstrumentEffect(instrument->id, definition, process_order);
  if (effect_id_or.has_value()) {
    *out_effect = new BarelyEffect();
    (*out_effect)->engine = instrument->engine;
    (*out_effect)->instrument_id = instrument->id;
    (*out_effect)->id = *effect_id_or;
    return true;
  }
  return false;
}

bool BarelyEffect_Destroy(BarelyEffectHandle effect) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().DestroyEffect(effect->id);
  }
  return false;
}

bool BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t index, double* out_value) {
  if (!effect) return false;
  if (!out_value) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    if (const auto* control = instrument_or->get().GetEffectControl(effect->id, index)) {
      *out_value = control->GetValue();
      return true;
    }
  }
  return false;
}

bool BarelyEffect_GetControlDefinition(BarelyEffectHandle effect, int32_t index,
                                       BarelyControlDefinition* out_definition) {
  if (!effect) return false;
  if (!out_definition) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    if (const auto* control = instrument_or->get().GetEffectControl(effect->id, index)) {
      *out_definition = control->GetDefinition();
      return true;
    }
  }
  return false;
}

bool BarelyEffect_GetProcessOrder(BarelyEffectHandle effect, int32_t* out_process_order) {
  if (!effect) return false;
  if (!out_process_order) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    const auto process_order_or = instrument_or->get().GetEffectProcessOrder(effect->id);
    if (process_order_or.has_value()) {
      *out_process_order = *process_order_or;
      return true;
    }
  }
  return false;
}

bool BarelyEffect_ResetAllControls(BarelyEffectHandle effect) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().ResetAllEffectControls(effect->id);
  }
  return false;
}

bool BarelyEffect_ResetControl(BarelyEffectHandle effect, int32_t index) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().ResetEffectControl(effect->id, index);
  }
  return false;
}

bool BarelyEffect_SetControl(BarelyEffectHandle effect, int32_t index, double value,
                             double slope_per_beat) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().SetEffectControl(effect->id, index, value, slope_per_beat);
  }
  return false;
}

bool BarelyEffect_SetControlEvent(BarelyEffectHandle effect,
                                  BarelyControlEventDefinition definition, void* user_data) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetEffectControlEvent(effect->id, definition, user_data);
    return true;
  }
  return false;
}

bool BarelyEffect_SetData(BarelyEffectHandle effect, const void* data, int32_t size) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().SetEffectData(
        effect->id,
        {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
  }
  return false;
}

bool BarelyEffect_SetProcessOrder(BarelyEffectHandle effect, int32_t process_order) {
  if (!effect) return false;

  const auto instrument_or = effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.has_value()) {
    return instrument_or->get().SetEffectProcessOrder(effect->id, process_order);
  }
  return false;
}

bool BarelyInstrument_Create(BarelyMusicianHandle musician, BarelyInstrumentDefinition definition,
                             int32_t frame_rate, BarelyInstrumentHandle* out_instrument) {
  if (!musician) return false;
  if (!out_instrument) return false;

  const auto instrument_id_or = musician->engine.CreateInstrument(definition, frame_rate);
  if (instrument_id_or.has_value()) {
    *out_instrument = new BarelyInstrument();
    (*out_instrument)->engine = &musician->engine;
    (*out_instrument)->id = *instrument_id_or;
    return true;
  }
  return false;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  return instrument->engine->DestroyInstrument(instrument->id);
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, int32_t index,
                                 double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    if (const auto* control = instrument_or->get().GetControl(index)) {
      *out_value = control->GetValue();
      return true;
    }
  }
  return false;
}

bool BarelyInstrument_GetControlDefinition(BarelyInstrumentHandle instrument, int32_t index,
                                           BarelyControlDefinition* out_definition) {
  if (!instrument) return false;
  if (!out_definition) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    if (const auto* control = instrument_or->get().GetControl(index)) {
      *out_definition = control->GetDefinition();
      return true;
    }
  }
  return false;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double* out_value) {
  if (!instrument) return false;
  if (!out_value) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    if (const auto* note_control = instrument_or->get().GetNoteControl(pitch, index)) {
      *out_value = note_control->GetValue();
      return true;
    }
  }
  return false;
}

bool BarelyInstrument_GetNoteControlDefinition(BarelyInstrumentHandle instrument, double pitch,
                                               int32_t index,
                                               BarelyControlDefinition* out_definition) {
  if (!instrument) return false;
  if (!out_definition) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    if (const auto* note_control = instrument_or->get().GetNoteControl(pitch, index)) {
      *out_definition = note_control->GetDefinition();
      return true;
    }
  }
  return false;
}

bool BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument, double pitch,
                               bool* out_is_note_on) {
  if (!instrument) return false;
  if (!out_is_note_on) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    *out_is_note_on = instrument_or->get().IsNoteOn(pitch);
    return true;
  }
  return false;
}

bool BarelyInstrument_Process(BarelyInstrumentHandle instrument, double* output_samples,
                              int32_t output_channel_count, int32_t output_frame_count,
                              double timestamp) {
  if (!instrument) return false;

  return instrument->engine->ProcessInstrument(instrument->id, output_samples, output_channel_count,
                                               output_frame_count, timestamp);
}

bool BarelyInstrument_ResetAllControls(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().ResetAllControls();
    return true;
  }
  return false;
}

bool BarelyInstrument_ResetAllNoteControls(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    return instrument_or->get().ResetAllNoteControls(pitch);
  }
  return false;
}

bool BarelyInstrument_ResetControl(BarelyInstrumentHandle instrument, int32_t index) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    return instrument_or->get().ResetControl(index);
  }
  return false;
}

bool BarelyInstrument_ResetNoteControl(BarelyInstrumentHandle instrument, double pitch,
                                       int32_t index) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    return instrument_or->get().ResetNoteControl(pitch, index);
  }
  return false;
}

bool BarelyInstrument_SetAllNotesOff(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetAllNotesOff();
    return true;
  }
  return false;
}

bool BarelyInstrument_SetControl(BarelyInstrumentHandle instrument, int32_t index, double value,
                                 double slope_per_beat) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    return instrument_or->get().SetControl(index, value, slope_per_beat);
  }
  return false;
}

bool BarelyInstrument_SetControlEvent(BarelyInstrumentHandle instrument,
                                      BarelyControlEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetControlEvent(definition, user_data);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetData(BarelyInstrumentHandle instrument, const void* data, int32_t size) {
  if (!instrument) return false;
  if (size < 0 || (!data && size > 0)) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetData(
        {static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size});
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, double pitch, int32_t index,
                                     double value, double slope_per_beat) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    return instrument_or->get().SetNoteControl(pitch, index, value, slope_per_beat);
  }
  return false;
}

bool BarelyInstrument_SetNoteControlEvent(BarelyInstrumentHandle instrument,
                                          BarelyNoteControlEventDefinition definition,
                                          void* user_data) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetNoteControlEvent(definition, user_data);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetNoteOff(pitch);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteOffEvent(BarelyInstrumentHandle instrument,
                                      BarelyNoteOffEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetNoteOffEvent(definition, user_data);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, double pitch, double intensity) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetNoteOn(pitch, intensity);
    return true;
  }
  return false;
}

bool BarelyInstrument_SetNoteOnEvent(BarelyInstrumentHandle instrument,
                                     BarelyNoteOnEventDefinition definition, void* user_data) {
  if (!instrument) return false;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.has_value()) {
    instrument_or->get().SetNoteOnEvent(definition, user_data);
    return true;
  }
  return false;
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

  const auto performer_id = musician->engine.CreatePerformer();
  *out_performer = new BarelyPerformer();
  (*out_performer)->engine = &musician->engine;
  (*out_performer)->id = performer_id;
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  return performer->engine->DestroyPerformer(performer->id);
}

bool BarelyPerformer_GetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double* out_loop_begin_position) {
  if (!performer) return false;
  if (!out_loop_begin_position) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    *out_loop_begin_position = performer_or->get().GetLoopBeginPosition();
    return true;
  }
  return false;
}

bool BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer, double* out_loop_length) {
  if (!performer) return false;
  if (!out_loop_length) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    *out_loop_length = performer_or->get().GetLoopLength();
    return true;
  }
  return false;
}

bool BarelyPerformer_GetPosition(BarelyPerformerHandle performer, double* out_position) {
  if (!performer) return false;
  if (!out_position) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    *out_position = performer_or->get().GetPosition();
    return true;
  }
  return false;
}

bool BarelyPerformer_IsLooping(BarelyPerformerHandle performer, bool* out_is_looping) {
  if (!performer) return false;
  if (!out_is_looping) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    *out_is_looping = performer_or->get().IsLooping();
    return true;
  }
  return false;
}

bool BarelyPerformer_IsPlaying(BarelyPerformerHandle performer, bool* out_is_playing) {
  if (!performer) return false;
  if (!out_is_playing) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    *out_is_playing = performer_or->get().IsPlaying();
    return true;
  }
  return false;
}

bool BarelyPerformer_SetLoopBeginPosition(BarelyPerformerHandle performer,
                                          double loop_begin_position) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().SetLoopBeginPosition(loop_begin_position);
    return true;
  }
  return false;
}

bool BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer, double loop_length) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().SetLoopLength(loop_length);
    return true;
  }
  return false;
}

bool BarelyPerformer_SetLooping(BarelyPerformerHandle performer, bool is_looping) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().SetLooping(is_looping);
    return true;
  }
  return false;
}

bool BarelyPerformer_SetPosition(BarelyPerformerHandle performer, double position) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().SetPosition(position);
    return true;
  }
  return false;
}

bool BarelyPerformer_Start(BarelyPerformerHandle performer) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().Start();
    return true;
  }
  return false;
}

bool BarelyPerformer_Stop(BarelyPerformerHandle performer) {
  if (!performer) return false;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.has_value()) {
    performer_or->get().Stop();
    return true;
  }
  return false;
}

bool BarelyTask_Create(BarelyPerformerHandle performer, BarelyTaskDefinition definition,
                       bool is_one_off, double position, int32_t process_order, void* user_data,
                       BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (!out_task) return false;

  const auto task_id_or = performer->engine->CreatePerformerTask(
      performer->id, definition, is_one_off, position, process_order, user_data);
  if (task_id_or.has_value()) {
    *out_task = new BarelyTask();
    (*out_task)->engine = performer->engine;
    (*out_task)->performer_id = performer->id;
    (*out_task)->id = *task_id_or;
    return true;
  }
  return false;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.has_value()) {
    return performer_or->get().DestroyTask(task->id);
  }
  return false;
}

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.has_value()) {
    const auto position_or = performer_or->get().GetTaskPosition(task->id);
    if (position_or.has_value()) {
      *out_position = *position_or;
      return true;
    }
  }
  return false;
}

bool BarelyTask_GetProcessOrder(BarelyTaskHandle task, int32_t* out_process_order) {
  if (!task) return false;
  if (!out_process_order) return false;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.has_value()) {
    const auto process_order_or = performer_or->get().GetTaskProcessOrder(task->id);
    if (process_order_or.has_value()) {
      *out_process_order = *process_order_or;
      return true;
    }
  }
  return false;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return false;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.has_value()) {
    return performer_or->get().SetTaskPosition(task->id, position);
  }
  return false;
}

bool BarelyTask_SetProcessOrder(BarelyTaskHandle task, int32_t process_order) {
  if (!task) return false;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.has_value()) {
    return performer_or->get().SetTaskProcessOrder(task->id, process_order);
  }
  return false;
}
