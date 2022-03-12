#ifndef BARELYMUSICIAN_API_MODULES_INSTRUMENTS_H_
#define BARELYMUSICIAN_API_MODULES_INSTRUMENTS_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/api/instrument.h"
#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Oscillator type enum alias.
typedef int32_t BarelyPresetsOscillatorType;

/// Oscillator type enum values.
enum BarelyPresetsOscillatorType_Values {
  /// Sine wave.
  BarelyPresetsOscillatorType_kSine = 0,
  /// Sawtooth wave.
  BarelyPresetsOscillatorType_kSaw = 1,
  /// Square wave.
  BarelyPresetsOscillatorType_kSquare = 2,
  /// White noise.
  BarelyPresetsOscillatorType_kNoise = 3,
};

/// Instrument type enum alias.
typedef int32_t BarelyPresetsInstrumentType;

/// Instrument type enum values.
enum BarelyPresetsInstrumentType_Values {
  /// Synth instrument.
  BarelyPresetsInstrumentType_kSynth = 0,
  /// Drumkit instrument.
  BarelyPresetsInstrumentType_kDrumkit = 1,
};

/// Synth instrument parameter enum alias.
typedef int32_t BarelyPresetsSynthParameter;

/// Synth instrument parameter enum values.
enum BarelyPresetsSynthParameter_Values {
  /// Envelope attack.
  BarelyPresetsSynthParameter_kAttack = 0,
  /// Envelope decay.
  BarelyPresetsSynthParameter_kDecay = 1,
  /// Envelope sustain.
  BarelyPresetsSynthParameter_kSustain = 2,
  /// Envelope release.
  BarelyPresetsSynthParameter_kRelease = 3,
  /// Oscillator type.
  BarelyPresetsSynthParameter_kOscillatorType = 4,
  /// Number of voices
  BarelyPresetsSynthParameter_kNumVoices = 5,
};

/// Drumkit instrument parameter enum alias.
typedef int32_t BarelyPresetsDrumkitParameter;

/// Drumkit instrument parameter enum values.
enum BarelyPresetsDrumkitParameter_Values {
  /// Pad envelope release.
  BarelyPresetsDrumkitParameter_kPadRelease = 0,
};

/// Gets instrument definition of type.
///
/// @param type Instrument type.
/// @param out_definition Output instrument definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyPresets_GetInstrumentDefinition(
    BarelyPresetsInstrumentType type,
    BarelyInstrumentDefinition* out_definition);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>

namespace barely::presets {

/// Oscillator type.
enum class OscillatorType : BarelyPresetsOscillatorType {
  /// Sine wave.
  kSine = BarelyPresetsOscillatorType_kSine,
  /// Sawtooth wave.
  kSaw = BarelyPresetsOscillatorType_kSaw,
  /// Square wave.
  kSquare = BarelyPresetsOscillatorType_kSquare,
  /// White noise.
  kNoise = BarelyPresetsOscillatorType_kNoise,
};

/// Instrument type.
enum class InstrumentType : BarelyPresetsInstrumentType {
  /// Synth instrument.
  kSynth = BarelyPresetsInstrumentType_kSynth,
  /// Drumkit instrument.
  kDrumkit = BarelyPresetsInstrumentType_kDrumkit,
};

/// Synth parameter.
enum SynthParameter : BarelyPresetsSynthParameter {
  /// Envelope attack.
  kAttack = BarelyPresetsSynthParameter_kAttack,
  /// Envelope decay.
  kDecay = BarelyPresetsSynthParameter_kDecay,
  /// Envelope sustain.
  kSustain = BarelyPresetsSynthParameter_kSustain,
  /// Envelope release.
  kRelease = BarelyPresetsSynthParameter_kRelease,
  /// Oscillator type.
  kOscillatorType = BarelyPresetsSynthParameter_kOscillatorType,
  /// Number of voices
  kNumVoices = BarelyPresetsSynthParameter_kNumVoices,
};

/// Drumkit parameter.
enum DrumkitParameter : BarelyPresetsDrumkitParameter {
  /// Pad envelope release.
  kPadRelease = BarelyPresetsDrumkitParameter_kPadRelease,
};

/// Returns instrument definition of type.
///
/// @param type Instrument type.
/// @return Instrument definition.
inline InstrumentDefinition GetInstrumentDefinition(InstrumentType type) {
  BarelyInstrumentDefinition definition;
  const auto status = BarelyPresets_GetInstrumentDefinition(
      static_cast<BarelyPresetsInstrumentType>(type), &definition);
  assert(IsOk(static_cast<Status>(status)));
  return InstrumentDefinition{definition};
}

/// Returns instrument of type.
///
/// @param type Instrument type.
/// @param frame_rate Frame rate in hz.
/// @return Instrument.
inline Instrument CreateInstrument(InstrumentType type, int frame_rate) {
  return Instrument{GetInstrumentDefinition(type), frame_rate};
}

}  // namespace barely::presets
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_MODULES_INSTRUMENTS_H_
