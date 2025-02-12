#include "barelycomposer.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>

#include "barelymusician.h"
#include "common/random.h"
#include "components/arpeggiator.h"
#include "components/repeater.h"
#include "engine/engine.h"

// Arpeggiator.
struct BarelyArpeggiator : public barely::internal::Arpeggiator {
  explicit BarelyArpeggiator(barely::internal::Engine& engine) noexcept
      : barely::internal::Arpeggiator(engine) {}
};

// Random.
struct BarelyRandom : public barely::internal::Random {
  explicit BarelyRandom(int seed) noexcept : barely::internal::Random(seed) {}
};

// Repeater.
struct BarelyRepeater : public barely::internal::Repeater {
  explicit BarelyRepeater(barely::internal::Engine& engine) noexcept
      : barely::internal::Repeater(engine) {}
};

bool BarelyArpeggiator_Create(BarelyEngineHandle engine, BarelyArpeggiatorHandle* out_arpeggiator) {
  if (!engine || !out_arpeggiator) return false;

  *out_arpeggiator = new BarelyArpeggiator(*engine);
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

bool BarelyRandom_Create(int32_t seed, BarelyRandomHandle* out_random) {
  if (!out_random) return false;

  *out_random = new BarelyRandom(static_cast<int>(seed));
  return true;
}

bool BarelyRandom_Destroy(BarelyRandomHandle random) {
  if (!random) return false;

  delete random;
  return true;
}

bool BarelyRandom_DrawNormal(BarelyRandomHandle random, float mean, float variance,
                             float* out_number) {
  if (!random || !out_number) return false;

  *out_number = random->DrawNormal(mean, variance);
  return true;
}

bool BarelyRandom_DrawUniformInt(BarelyRandomHandle random, int32_t min, int32_t max,
                                 int32_t* out_number) {
  if (!random || !out_number) return false;
  if (min > max) return false;

  *out_number =
      static_cast<int32_t>(random->DrawUniform(static_cast<int>(min), static_cast<int>(max)));
  return true;
}

bool BarelyRandom_DrawUniformReal(BarelyRandomHandle random, float min, float max,
                                  float* out_number) {
  if (!random || !out_number) return false;
  if (min > max) return false;

  *out_number = random->DrawUniform(min, max);
  return true;
}

bool BarelyRandom_Reset(BarelyRandomHandle random, int32_t seed) {
  if (!random) return false;

  random->Reset(static_cast<int>(seed));
  return true;
}

bool BarelyRepeater_Clear(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Clear();
  return true;
}

bool BarelyRepeater_Create(BarelyEngineHandle engine, BarelyRepeaterHandle* out_repeater) {
  if (!engine || !out_repeater) return false;

  *out_repeater = new BarelyRepeater(*engine);
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
