#include "barelymusician.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>
#include <span>

#include "private/arpeggiator_impl.h"
#include "private/engine_impl.h"
#include "private/instrument_impl.h"
#include "private/performer_impl.h"
#include "private/repeater_impl.h"

using ::barely::ControlType;
using ::barely::NoteControlType;
using ::barely::Slice;

bool BarelyArpeggiator_Create(BarelyEngineHandle engine, BarelyArpeggiatorHandle* out_arpeggiator) {
  if (!engine || !out_arpeggiator) return false;

  *out_arpeggiator = static_cast<BarelyArpeggiatorHandle>(new barely::ArpeggiatorImpl(*engine));
  return true;
}

bool BarelyArpeggiator_Destroy(BarelyArpeggiatorHandle arpeggiator) {
  if (!arpeggiator) return false;

  delete arpeggiator;
  return true;
}

bool BarelyArpeggiator_IsNoteOn(BarelyArpeggiatorHandle arpeggiator, float pitch,
                                bool* out_is_note_on) {
  if (!arpeggiator || !out_is_note_on) return false;

  *out_is_note_on = arpeggiator->IsNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_IsPlaying(BarelyArpeggiatorHandle arpeggiator, bool* out_is_playing) {
  if (!arpeggiator || !out_is_playing) return false;

  *out_is_playing = arpeggiator->IsPlaying();
  return true;
}

bool BarelyArpeggiator_SetAllNotesOff(BarelyArpeggiatorHandle arpeggiator) {
  if (!arpeggiator) return false;

  arpeggiator->SetAllNotesOff();
  return true;
}

bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiatorHandle arpeggiator, float gate_ratio) {
  if (!arpeggiator) return false;

  arpeggiator->SetGateRatio(gate_ratio);
  return true;
}

bool BarelyArpeggiator_SetInstrument(BarelyArpeggiatorHandle arpeggiator,
                                     BarelyInstrumentHandle instrument) {
  if (!arpeggiator) return false;

  arpeggiator->SetInstrument(instrument);
  return true;
}

bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiatorHandle arpeggiator, float pitch) {
  if (!arpeggiator) return false;

  arpeggiator->SetNoteOff(pitch);
  return true;
}

bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiatorHandle arpeggiator, float pitch) {
  if (!arpeggiator) return false;

  arpeggiator->SetNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_SetRate(BarelyArpeggiatorHandle arpeggiator, double rate) {
  if (!arpeggiator) return false;

  arpeggiator->SetRate(rate);
  return true;
}

bool BarelyArpeggiator_SetStyle(BarelyArpeggiatorHandle arpeggiator, BarelyArpeggiatorStyle style) {
  if (!arpeggiator) return false;

  arpeggiator->SetStyle(static_cast<barely::ArpeggiatorStyle>(style));
  return true;
}

bool BarelyEngine_Create(int32_t sample_rate, BarelyEngineHandle* out_engine) {
  if (sample_rate <= 0) return false;
  if (!out_engine) return false;

  *out_engine = new BarelyEngine(sample_rate);
  return true;
}

bool BarelyEngine_Destroy(BarelyEngineHandle engine) {
  if (!engine) return false;

  delete engine;
  return true;
}

bool BarelyEngine_GenerateRandomNumber(BarelyEngineHandle engine, double* out_number) {
  if (!engine) return false;
  if (!out_number) return false;

  *out_number = engine->main_rng().Generate();
  return true;
}

bool BarelyEngine_GetReferenceFrequency(BarelyEngineHandle engine, float* out_reference_frequency) {
  if (!engine) return false;
  if (!out_reference_frequency) return false;

  *out_reference_frequency = engine->GetReferenceFrequency();
  return true;
}

bool BarelyEngine_GetSeed(BarelyEngineHandle engine, int32_t* out_seed) {
  if (!engine) return false;
  if (!out_seed) return false;

  *out_seed = engine->main_rng().GetSeed();
  return true;
}

bool BarelyEngine_GetTempo(BarelyEngineHandle engine, double* out_tempo) {
  if (!engine) return false;
  if (!out_tempo) return false;

  *out_tempo = engine->GetTempo();
  return true;
}

bool BarelyEngine_GetTimestamp(BarelyEngineHandle engine, double* out_timestamp) {
  if (!engine) return false;
  if (!out_timestamp) return false;

  *out_timestamp = engine->GetTimestamp();
  return true;
}

bool BarelyEngine_SetReferenceFrequency(BarelyEngineHandle engine, float reference_frequency) {
  if (!engine) return false;

  engine->SetReferenceFrequency(reference_frequency);
  return true;
}

bool BarelyEngine_SetSeed(BarelyEngineHandle engine, int32_t seed) {
  if (!engine) return false;

  engine->main_rng().SetSeed(seed);
  return true;
}

bool BarelyEngine_SetTempo(BarelyEngineHandle engine, double tempo) {
  if (!engine) return false;

  engine->SetTempo(tempo);
  return true;
}

bool BarelyEngine_Update(BarelyEngineHandle engine, double timestamp) {
  if (!engine) return false;

  engine->Update(timestamp);
  return true;
}

bool BarelyInstrument_Create(BarelyEngineHandle engine, BarelyInstrumentHandle* out_instrument) {
  if (!engine) return false;
  if (!out_instrument) return false;

  *out_instrument = static_cast<BarelyInstrument*>(engine->CreateInstrument());
  // TODO(#126): Temp hack to allow destroying by handle.
  (*out_instrument)->engine = engine;
  return true;
}

bool BarelyInstrument_Destroy(BarelyInstrumentHandle instrument) {
  if (!instrument) return false;

  instrument->engine->DestroyInstrument(instrument);
  return true;
}

bool BarelyInstrument_GetControl(BarelyInstrumentHandle instrument, BarelyControlType type,
                                 float* out_value) {
  if (!instrument) return false;
  if (type >= BarelyControlType_kCount) return false;
  if (!out_value) return false;

  *out_value = instrument->GetControl(static_cast<ControlType>(type));
  return true;
}

bool BarelyInstrument_GetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float* out_value) {
  if (!instrument) return false;
  if (type >= BarelyNoteControlType_kCount) return false;
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
  if (type >= BarelyControlType_kCount) return false;

  instrument->SetControl(static_cast<ControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteControl(BarelyInstrumentHandle instrument, float pitch,
                                     BarelyNoteControlType type, float value) {
  if (!instrument) return false;
  if (type >= BarelyNoteControlType_kCount) return false;

  instrument->SetNoteControl(pitch, static_cast<NoteControlType>(type), value);
  return true;
}

bool BarelyInstrument_SetNoteOff(BarelyInstrumentHandle instrument, float pitch) {
  if (!instrument) return false;

  instrument->SetNoteOff(pitch);
  return true;
}

bool BarelyInstrument_SetNoteOffCallback(BarelyInstrumentHandle instrument,
                                         BarelyInstrument_NoteOffCallback callback,
                                         void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOffCallback({callback, user_data});
  return true;
}

bool BarelyInstrument_SetNoteOn(BarelyInstrumentHandle instrument, float pitch, float intensity) {
  if (!instrument) return false;

  instrument->SetNoteOn(pitch, intensity);
  return true;
}

bool BarelyInstrument_SetNoteOnCallback(BarelyInstrumentHandle instrument,
                                        BarelyInstrument_NoteOnCallback callback, void* user_data) {
  if (!instrument) return false;

  instrument->SetNoteOnCallback({callback, user_data});
  return true;
}

bool BarelyInstrument_SetSampleData(BarelyInstrumentHandle instrument, const BarelySlice* slices,
                                    int32_t slice_count) {
  if (!instrument) return false;
  if (slice_count < 0 || (!slices && slice_count > 0)) return false;

  instrument->SetSampleData(
      std::span<const Slice>{reinterpret_cast<const Slice*>(slices),
                             reinterpret_cast<const Slice*>(slices + slice_count)});
  return true;
}

bool BarelyPerformer_Create(BarelyEngineHandle engine, BarelyPerformerHandle* out_performer) {
  if (!engine) return false;
  if (!out_performer) return false;

  *out_performer = static_cast<BarelyPerformer*>(engine->CreatePerformer());
  (*out_performer)->engine = engine;
  return true;
}

bool BarelyPerformer_Destroy(BarelyPerformerHandle performer) {
  if (!performer) return false;

  performer->engine->DestroyPerformer(performer);
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
                                     BarelyPerformer_BeatCallback callback, void* user_data) {
  if (!performer) return false;

  performer->SetBeatCallback({callback, user_data});
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

bool BarelyQuantization_GetPosition(const BarelyQuantization* quantization, double position,
                                    double* out_position) {
  if (!quantization || !out_position) return false;
  if (quantization->resolution <= 0.0) return false;
  if (quantization->amount < 0.0 || quantization->amount > 1.0) return false;

  *out_position = std::lerp(
      position, quantization->resolution * std::round(position / quantization->resolution),
      static_cast<double>(quantization->amount));
  return true;
}

bool BarelyRepeater_Clear(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Clear();
  return true;
}

bool BarelyRepeater_Create(BarelyEngineHandle engine, BarelyRepeaterHandle* out_repeater) {
  if (!engine || !out_repeater) return false;

  *out_repeater = static_cast<BarelyRepeaterHandle>(new barely::RepeaterImpl(*engine));
  return true;
}

bool BarelyRepeater_Destroy(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  delete repeater;
  return true;
}

bool BarelyRepeater_IsPlaying(BarelyRepeaterHandle repeater, bool* out_is_playing) {
  if (!repeater || !out_is_playing) return false;

  *out_is_playing = repeater->IsPlaying();
  return true;
}

bool BarelyRepeater_Pop(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Pop();
  return true;
}

bool BarelyRepeater_Push(BarelyRepeaterHandle repeater, float pitch, int32_t length) {
  if (!repeater) return false;

  repeater->Push(pitch, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_PushSilence(BarelyRepeaterHandle repeater, int32_t length) {
  if (!repeater) return false;

  repeater->Push(std::nullopt, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_SetInstrument(BarelyRepeaterHandle repeater,
                                  BarelyInstrumentHandle instrument) {
  if (!repeater) return false;

  repeater->SetInstrument(instrument);
  return true;
}

bool BarelyRepeater_SetRate(BarelyRepeaterHandle repeater, double rate) {
  if (!repeater) return false;

  repeater->SetRate(rate);
  return true;
}

bool BarelyRepeater_SetStyle(BarelyRepeaterHandle repeater, BarelyRepeaterStyle style) {
  if (!repeater) return false;

  repeater->SetStyle(static_cast<barely::RepeaterStyle>(style));
  return true;
}

bool BarelyRepeater_Start(BarelyRepeaterHandle repeater, float pitch_offset) {
  if (!repeater) return false;

  repeater->Start(pitch_offset);
  return true;
}

bool BarelyRepeater_Stop(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Stop();
  return true;
}

bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree, float* out_pitch) {
  if (scale == nullptr) return false;
  if (scale->pitches == nullptr || scale->pitch_count == 0) return false;
  if (scale->mode < 0 || scale->mode >= scale->pitch_count) return false;
  if (out_pitch == nullptr) return false;

  const int scale_degree = degree + scale->mode;
  const int pitch_count = static_cast<int>(scale->pitch_count);
  const int octave = static_cast<int>(
      std::floor(static_cast<float>(scale_degree) / static_cast<float>(pitch_count)));
  const int index = scale_degree - octave * pitch_count;
  assert(index >= 0 && index < pitch_count);
  *out_pitch = scale->root_pitch + static_cast<float>(octave) + scale->pitches[index] -
               scale->pitches[scale->mode];
  return true;
}

bool BarelyTask_Create(BarelyPerformerHandle performer, double position, double duration,
                       BarelyTask_ProcessCallback callback, void* user_data,
                       BarelyTaskHandle* out_task) {
  if (!performer) return false;
  if (duration <= 0.0) return false;
  if (!out_task) return false;

  *out_task =
      static_cast<BarelyTask*>(performer->CreateTask(position, duration, {callback, user_data}));
  // TODO(#126): Temp hack to allow destroying by handle.
  (*out_task)->performer = performer;
  return *out_task;
}

bool BarelyTask_Destroy(BarelyTaskHandle task) {
  if (!task) return false;

  task->performer->DestroyTask(task);
  return true;
}

bool BarelyTask_GetDuration(BarelyTaskHandle task, double* out_duration) {
  if (!task) return false;
  if (!out_duration) return false;

  *out_duration = task->GetDuration();
  return true;
}

bool BarelyTask_GetPosition(BarelyTaskHandle task, double* out_position) {
  if (!task) return false;
  if (!out_position) return false;

  *out_position = task->GetPosition();
  return true;
}

bool BarelyTask_IsActive(BarelyTaskHandle task, bool* out_is_active) {
  if (!task) return false;
  if (!out_is_active) return false;

  *out_is_active = task->IsActive();
  return true;
}

bool BarelyTask_SetDuration(BarelyTaskHandle task, double duration) {
  if (!task) return false;
  if (duration <= 0.0) return false;

  task->SetDuration(duration);
  return true;
}

bool BarelyTask_SetPosition(BarelyTaskHandle task, double position) {
  if (!task) return false;

  task->SetPosition(position);
  return true;
}

bool BarelyTask_SetProcessCallback(BarelyTaskHandle task, BarelyTask_ProcessCallback callback,
                                   void* user_data) {
  if (!task) return false;

  task->SetProcessCallback({callback, user_data});
  return true;
}

bool Barely_AmplitudeToDecibels(float amplitude, float* out_decibels) {
  if (!out_decibels) return false;

  *out_decibels = (amplitude > 0.0) ? 20.0f * std::log10(amplitude) : barely::kMinDecibels;
  return true;
}

bool Barely_DecibelsToAmplitude(float decibels, float* out_amplitude) {
  if (!out_amplitude) return false;

  *out_amplitude = (decibels > barely::kMinDecibels) ? std::pow(10.0f, 0.05f * decibels) : 0.0f;
  return true;
}
