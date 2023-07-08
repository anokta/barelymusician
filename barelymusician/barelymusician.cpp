#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <cstddef>

#include "barelymusician/internal/engine.h"

extern "C" {

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
  friend BARELY_EXPORT BarelyStatus
  BarelyEffect_Destroy(BarelyEffectHandle effect);
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
  friend BARELY_EXPORT BarelyStatus
  BarelyInstrument_Destroy(BarelyInstrumentHandle instrument);
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
  friend BARELY_EXPORT BarelyStatus
  BarelyMusician_Destroy(BarelyMusicianHandle musician);
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
  friend BARELY_EXPORT BarelyStatus
  BarelyPerformer_Destroy(BarelyPerformerHandle performer);
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
  friend BARELY_EXPORT BarelyStatus BarelyTask_Destroy(BarelyTaskHandle task);
  ~BarelyTask() = default;
};

BarelyStatus BarelyEffect_Create(BarelyInstrumentHandle instrument,
                                 BarelyEffectDefinition definition,
                                 int32_t process_order,
                                 BarelyEffectHandle* out_effect) {
  if (!instrument) return BarelyStatus_kNotFound;
  if (!out_effect) return BarelyStatus_kInvalidArgument;

  const auto effect_id_or = instrument->engine->CreateInstrumentEffect(
      instrument->id, definition, process_order);
  if (effect_id_or.IsOk()) {
    *out_effect = new BarelyEffect();
    (*out_effect)->engine = instrument->engine;
    (*out_effect)->instrument_id = instrument->id;
    (*out_effect)->id = *effect_id_or;
    return BarelyStatus_kOk;
  }
  return effect_id_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_Destroy(BarelyEffectHandle effect) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().DestroyEffect(effect->id);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_GetControl(BarelyEffectHandle effect, int32_t index,
                                     double* out_value) {
  if (!effect) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    const auto value_or =
        instrument_or->get().GetEffectControl(effect->id, index);
    if (value_or.IsOk()) {
      *out_value = *value_or;
      return BarelyStatus_kOk;
    }
    return value_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_GetProcessOrder(BarelyEffectHandle effect,
                                          int32_t* out_process_order) {
  if (!effect) return BarelyStatus_kNotFound;
  if (!out_process_order) return BarelyStatus_kInvalidArgument;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    const auto process_order_or =
        instrument_or->get().GetEffectProcessOrder(effect->id);
    if (process_order_or.IsOk()) {
      *out_process_order = *process_order_or;
      return BarelyStatus_kOk;
    }
    return process_order_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_ResetAllControls(BarelyEffectHandle effect) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetAllEffectControls(effect->id);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_ResetControl(BarelyEffectHandle effect,
                                       int32_t index) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetEffectControl(effect->id, index);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_SetControl(BarelyEffectHandle effect, int32_t index,
                                     double value, double slope_per_beat) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetEffectControl(effect->id, index, value,
                                                 slope_per_beat);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_SetControlEvent(
    BarelyEffectHandle effect, BarelyControlEventDefinition definition,
    void* user_data) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetEffectControlEvent(effect->id, definition,
                                               user_data);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_SetData(BarelyEffectHandle effect, const void* data,
                                  int32_t size) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetEffectData(
        effect->id, {static_cast<const std::byte*>(data),
                     static_cast<const std::byte*>(data) + size});
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyEffect_SetProcessOrder(BarelyEffectHandle effect,
                                          int32_t process_order) {
  if (!effect) return BarelyStatus_kNotFound;

  const auto instrument_or =
      effect->engine->GetInstrument(effect->instrument_id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetEffectProcessOrder(effect->id,
                                                      process_order);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_Create(BarelyMusicianHandle musician,
                                     BarelyInstrumentDefinition definition,
                                     int32_t frame_rate,
                                     BarelyInstrumentHandle* out_instrument) {
  if (!musician) return BarelyStatus_kNotFound;
  if (!out_instrument) return BarelyStatus_kInvalidArgument;

  const auto instrument_id_or =
      musician->engine.CreateInstrument(definition, frame_rate);
  if (instrument_id_or.IsOk()) {
    *out_instrument = new BarelyInstrument();
    (*out_instrument)->engine = &musician->engine;
    (*out_instrument)->id = *instrument_id_or;
    return BarelyStatus_kOk;
  }
  return instrument_id_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return BarelyStatus_kNotFound;

  return instrument->engine->DestroyInstrument(instrument->id);
}

BarelyStatus BarelyInstrument_GetControl(BarelyInstrumentHandle instrument,
                                         int32_t index, double* out_value) {
  if (!instrument) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    const auto control_or = instrument_or->get().GetControl(index);
    if (control_or.IsOk()) {
      *out_value = *control_or;
      return BarelyStatus_kOk;
    }
    return control_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument,
                                             double pitch, int32_t index,
                                             double* out_value) {
  if (!instrument) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    const auto note_control_or =
        instrument_or->get().GetNoteControl(pitch, index);
    if (note_control_or.IsOk()) {
      *out_value = *note_control_or;
      return BarelyStatus_kOk;
    }
    return note_control_or.GetErrorStatus();
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyInstrumentHandle instrument,
                                       double pitch, bool* out_is_note_on) {
  if (!instrument) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    *out_is_note_on = instrument_or->get().IsNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_Process(BarelyInstrumentHandle instrument,
                                      double* output_samples,
                                      int32_t output_channel_count,
                                      int32_t output_frame_count,
                                      double timestamp) {
  if (!instrument) return BarelyStatus_kNotFound;

  return instrument->engine->ProcessInstrument(instrument->id, output_samples,
                                               output_channel_count,
                                               output_frame_count, timestamp);
}

BarelyStatus BarelyInstrument_ResetAllControls(
    BarelyInstrumentHandle instrument) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().ResetAllControls();
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetAllNoteControls(
    BarelyInstrumentHandle instrument, double pitch) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetAllNoteControls(pitch);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetControl(BarelyInstrumentHandle instrument,
                                           int32_t index) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetControl(index);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_ResetNoteControl(
    BarelyInstrumentHandle instrument, double pitch, int32_t index) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().ResetNoteControl(pitch, index);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetAllNotesOff(
    BarelyInstrumentHandle instrument) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetAllNotesOff();
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetControl(BarelyInstrumentHandle instrument,
                                         int32_t index, double value,
                                         double slope_per_beat) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetControl(index, value, slope_per_beat);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetControlEvent(
    BarelyInstrumentHandle instrument, BarelyControlEventDefinition definition,
    void* user_data) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetControlEvent(definition, user_data);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetData(BarelyInstrumentHandle instrument,
                                      const void* data, int32_t size) {
  if (!instrument) return BarelyStatus_kNotFound;
  if (size < 0 || (!data && size > 0)) return BarelyStatus_kInvalidArgument;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetData({static_cast<const std::byte*>(data),
                                  static_cast<const std::byte*>(data) + size});
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument,
                                             double pitch, int32_t index,
                                             double value,
                                             double slope_per_beat) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    return instrument_or->get().SetNoteControl(pitch, index, value,
                                               slope_per_beat);
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteControlEvent(
    BarelyInstrumentHandle instrument,
    BarelyNoteControlEventDefinition definition, void* user_data) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteControlEvent(definition, user_data);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument,
                                         double pitch) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOff(pitch);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOffEvent(
    BarelyInstrumentHandle instrument, BarelyNoteOffEventDefinition definition,
    void* user_data) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOffEvent(definition, user_data);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument,
                                        double pitch, double intensity) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOn(pitch, intensity);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyInstrument_SetNoteOnEvent(
    BarelyInstrumentHandle instrument, BarelyNoteOnEventDefinition definition,
    void* user_data) {
  if (!instrument) return BarelyStatus_kNotFound;

  const auto instrument_or = instrument->engine->GetInstrument(instrument->id);
  if (instrument_or.IsOk()) {
    instrument_or->get().SetNoteOnEvent(definition, user_data);
    return BarelyStatus_kOk;
  }
  return instrument_or.GetErrorStatus();
}

BarelyStatus BarelyMusician_Create(BarelyMusicianHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle musician) {
  if (!musician) return BarelyStatus_kNotFound;

  delete musician;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle musician,
                                     double* out_tempo) {
  if (!musician) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = musician->engine.GetTempo();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyMusicianHandle musician,
                                         double* out_timestamp) {
  if (!musician) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = musician->engine.GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle musician,
                                     double tempo) {
  if (!musician) return BarelyStatus_kNotFound;

  musician->engine.SetTempo(tempo);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle musician,
                                   double timestamp) {
  if (!musician) return BarelyStatus_kNotFound;

  musician->engine.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_Create(BarelyMusicianHandle musician,
                                    BarelyPerformerHandle* out_performer) {
  if (!musician) return BarelyStatus_kNotFound;
  if (!out_performer) return BarelyStatus_kInvalidArgument;

  const auto performer_id_or = musician->engine.CreatePerformer();
  if (performer_id_or.IsOk()) {
    *out_performer = new BarelyPerformer();
    (*out_performer)->engine = &musician->engine;
    (*out_performer)->id = *performer_id_or;
    return BarelyStatus_kOk;
  }
  return performer_id_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return BarelyStatus_kNotFound;

  return performer->engine->DestroyPerformer(performer->id);
}

BarelyStatus BarelyPerformer_GetLoopBeginPosition(
    BarelyPerformerHandle performer, double* out_loop_begin_position) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_loop_begin_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    *out_loop_begin_position = performer_or->get().GetLoopBeginPosition();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetLoopLength(BarelyPerformerHandle performer,
                                           double* out_loop_length) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    *out_loop_length = performer_or->get().GetLoopLength();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_GetPosition(BarelyPerformerHandle performer,
                                         double* out_position) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    *out_position = performer_or->get().GetPosition();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_IsLooping(BarelyPerformerHandle performer,
                                       bool* out_is_looping) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    *out_is_looping = performer_or->get().IsLooping();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_IsPlaying(BarelyPerformerHandle performer,
                                       bool* out_is_playing) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    *out_is_playing = performer_or->get().IsPlaying();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLoopBeginPosition(
    BarelyPerformerHandle performer, double loop_begin_position) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLoopBeginPosition(loop_begin_position);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLoopLength(BarelyPerformerHandle performer,
                                           double loop_length) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLoopLength(loop_length);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetLooping(BarelyPerformerHandle performer,
                                        bool is_looping) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().SetLooping(is_looping);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_SetPosition(BarelyPerformerHandle performer,
                                         double position) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().SetPosition(position);
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Start(BarelyPerformerHandle performer) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().Start();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyPerformer_Stop(BarelyPerformerHandle performer) {
  if (!performer) return BarelyStatus_kNotFound;

  const auto performer_or = performer->engine->GetPerformer(performer->id);
  if (performer_or.IsOk()) {
    performer_or->get().Stop();
    return BarelyStatus_kOk;
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyTask_Create(BarelyPerformerHandle performer,
                               BarelyTaskDefinition definition, bool is_one_off,
                               double position, int32_t process_order,
                               void* user_data, BarelyTaskHandle* out_task) {
  if (!performer) return BarelyStatus_kNotFound;
  if (!out_task) return BarelyStatus_kInvalidArgument;

  const auto task_id_or = performer->engine->CreatePerformerTask(
      performer->id, definition, is_one_off, position, process_order,
      user_data);
  if (task_id_or.IsOk()) {
    *out_task = new BarelyTask();
    (*out_task)->engine = performer->engine;
    (*out_task)->performer_id = performer->id;
    (*out_task)->id = *task_id_or;
    return BarelyStatus_kOk;
  }
  return task_id_or.GetErrorStatus();
}

BarelyStatus BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return BarelyStatus_kNotFound;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.IsOk()) {
    return performer_or->get().DestroyTask(task->id);
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyTask_GetPosition(BarelyTaskHandle task,
                                    double* out_position) {
  if (!task) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.IsOk()) {
    const auto position_or = performer_or->get().GetTaskPosition(task->id);
    if (position_or.IsOk()) {
      *out_position = *position_or;
      return BarelyStatus_kOk;
    }
    return position_or.GetErrorStatus();
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyTask_GetProcessOrder(BarelyTaskHandle task,
                                        int32_t* out_process_order) {
  if (!task) return BarelyStatus_kNotFound;
  if (!out_process_order) return BarelyStatus_kInvalidArgument;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.IsOk()) {
    const auto process_order_or =
        performer_or->get().GetTaskProcessOrder(task->id);
    if (process_order_or.IsOk()) {
      *out_process_order = *process_order_or;
      return BarelyStatus_kOk;
    }
    return process_order_or.GetErrorStatus();
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return BarelyStatus_kNotFound;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.IsOk()) {
    return performer_or->get().SetTaskPosition(task->id, position);
  }
  return performer_or.GetErrorStatus();
}

BarelyStatus BarelyTask_SetProcessOrder(BarelyTaskHandle task,
                                        int32_t process_order) {
  if (!task) return BarelyStatus_kNotFound;

  const auto performer_or = task->engine->GetPerformer(task->performer_id);
  if (performer_or.IsOk()) {
    return performer_or->get().SetTaskProcessOrder(task->id, process_order);
  }
  return performer_or.GetErrorStatus();
}

}  // extern "C"
