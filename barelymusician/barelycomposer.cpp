#include "barelymusician/barelycomposer.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"
#include "barelymusician/components/arpeggiator.h"
#include "barelymusician/components/repeater.h"
#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"

// Arpeggiator.
struct BarelyArpeggiator : public barely::internal::Arpeggiator {
  BarelyArpeggiator(barely::internal::Musician& musician, int process_order) noexcept
      : barely::internal::Arpeggiator(musician, process_order) {}
};

// Random.
struct BarelyRandom : public barely::internal::Random {
  explicit BarelyRandom(int seed) noexcept : barely::internal::Random(seed) {}
};

// Repeater.
struct BarelyRepeater : public barely::internal::Repeater {
  BarelyRepeater(barely::internal::Musician& musician, int process_order) noexcept
      : barely::internal::Repeater(musician, process_order) {}
};

bool BarelyArpeggiator_Create(BarelyMusicianHandle musician, int32_t process_order,
                              BarelyArpeggiatorHandle* out_arpeggiator) {
  if (!musician || !out_arpeggiator) return false;

  *out_arpeggiator = new BarelyArpeggiator(*reinterpret_cast<barely::internal::Musician*>(musician),
                                           static_cast<int>(process_order));
  return true;
}

bool BarelyArpeggiator_Destroy(BarelyArpeggiatorHandle arpeggiator) {
  if (!arpeggiator) return false;

  delete arpeggiator;
  return true;
}

bool BarelyArpeggiator_IsNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch,
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

bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiatorHandle arpeggiator, double gate_ratio) {
  if (!arpeggiator) return false;

  arpeggiator->SetGateRatio(gate_ratio);
  return true;
}

bool BarelyArpeggiator_SetInstrument(BarelyArpeggiatorHandle arpeggiator,
                                     BarelyInstrumentHandle instrument) {
  if (!arpeggiator) return false;

  // TODO(#141): Can this cast be safer?
  arpeggiator->SetInstrument(reinterpret_cast<barely::internal::InstrumentController*>(instrument));
  return true;
}

bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiatorHandle arpeggiator, double pitch) {
  if (!arpeggiator) return false;

  arpeggiator->SetNoteOff(pitch);
  return true;
}

bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch) {
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
      quantization->amount);
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

bool BarelyRandom_DrawNormal(BarelyRandomHandle random, double mean, double variance,
                             double* out_number) {
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

bool BarelyRandom_DrawUniformReal(BarelyRandomHandle random, double min, double max,
                                  double* out_number) {
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

bool BarelyRepeater_Create(BarelyMusicianHandle musician, int32_t process_order,
                           BarelyRepeaterHandle* out_repeater) {
  if (!musician || !out_repeater) return false;

  // TODO(#141): Can this cast be safer?
  *out_repeater = new BarelyRepeater(*reinterpret_cast<barely::internal::Musician*>(musician),
                                     static_cast<int>(process_order));
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

bool BarelyRepeater_Push(BarelyRepeaterHandle repeater, double pitch, int32_t length) {
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

  // TODO(#141): Can this cast be safer?
  repeater->SetInstrument(reinterpret_cast<barely::internal::InstrumentController*>(instrument));
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

bool BarelyRepeater_Start(BarelyRepeaterHandle repeater, double pitch_offset) {
  if (!repeater) return false;

  repeater->Start(pitch_offset);
  return true;
}

bool BarelyRepeater_Stop(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Stop();
  return true;
}

bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree, double* out_pitch) {
  if (scale == nullptr) return false;
  if (scale->pitches == nullptr || scale->pitch_count == 0) return false;
  if (scale->mode < 0 || scale->mode >= scale->pitch_count) return false;
  if (out_pitch == nullptr) return false;

  const int scale_degree = degree + scale->mode;
  const int pitch_count = static_cast<int>(scale->pitch_count);
  const int octave = static_cast<int>(
      std::floor(static_cast<double>(scale_degree) / static_cast<double>(pitch_count)));
  const int index = scale_degree - octave * pitch_count;
  assert(index >= 0 && index < pitch_count);
  *out_pitch = scale->root_pitch + static_cast<double>(octave) + scale->pitches[index] -
               scale->pitches[scale->mode];
  return true;
}
