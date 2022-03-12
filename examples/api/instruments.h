#ifndef EXAMPLES_API_INSTRUMENTS_H_
#define EXAMPLES_API_INSTRUMENTS_H_

// NOLINTBEGIN
#include <stdint.h>

#include "barelymusician/api/instrument.h"
#include "barelymusician/api/status.h"
#include "barelymusician/api/visibility.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Instrument type enum alias.
typedef int32_t BarelyExamplesInstrumentType;

/// Instrument type enum values.
enum BarelyExamplesInstrumentType_Values {
  /// Synth instrument.
  BarelyExamplesInstrumentType_kSynth = 0,
  /// Drumkit instrument.
  BarelyExamplesInstrumentType_kDrumkit = 1,
};

/// Synth instrument parameter enum alias.
typedef int32_t BarelyExamplesSynthParameter;

/// Synth instrument parameter enum values.
enum BarelyExamplesSynthParameter_Values {
  /// Envelope attack.
  BarelyExamplesSynthParameter_kAttack = 0,
  /// Envelope decay.
  BarelyExamplesSynthParameter_kDecay = 1,
  /// Envelope sustain.
  BarelyExamplesSynthParameter_kSustain = 2,
  /// Envelope release.
  BarelyExamplesSynthParameter_kRelease = 3,
  /// Oscillator type.
  BarelyExamplesSynthParameter_kOscillatorType = 4,
  /// Number of voices
  BarelyExamplesSynthParameter_kNumVoices = 5,
};

/// Drumkit instrument parameter enum alias.
typedef int32_t BarelyExamplesDrumkitParameter;

/// Drumkit instrument parameter enum values.
enum BarelyExamplesDrumkitParameter_Values {
  /// Pad envelope release.
  BarelyExamplesDrumkitParameter_kPadRelease = 0,
};

/// Gets instrument definition of type.
///
/// @param type Instrument type.
/// @param out_definition Output instrument definition.
/// @return Status.
BARELY_EXPORT BarelyStatus BarelyExamples_GetInstrumentDefinition(
    BarelyExamplesInstrumentType type,
    BarelyInstrumentDefinition* out_definition);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <cassert>

namespace barely::examples {

/// Instrument type.
enum class InstrumentType : BarelyExamplesInstrumentType {
  /// Synth instrument.
  kSynth = BarelyExamplesInstrumentType_kSynth,
  /// Drumkit instrument.
  kDrumkit = BarelyExamplesInstrumentType_kDrumkit,
};

/// Synth parameter.
enum class SynthParameter : BarelyExamplesSynthParameter {
  /// Envelope attack.
  kAttack = BarelyExamplesSynthParameter_kAttack,
  /// Envelope decay.
  kDecay = BarelyExamplesSynthParameter_kDecay,
  /// Envelope sustain.
  kSustain = BarelyExamplesSynthParameter_kSustain,
  /// Envelope release.
  kRelease = BarelyExamplesSynthParameter_kRelease,
  /// Oscillator type.
  kOscillatorType = BarelyExamplesSynthParameter_kOscillatorType,
  /// Number of voices
  kNumVoices = BarelyExamplesSynthParameter_kNumVoices,
};

/// Drumkit parameter.
enum class DrumkitParameter : BarelyExamplesDrumkitParameter {
  /// Pad envelope release.
  kPadRelease = BarelyExamplesDrumkitParameter_kPadRelease,
};

/// Returns instrument definition of type.
///
/// @param type Instrument type.
/// @return Instrument definition.
inline InstrumentDefinition GetInstrumentDefinition(InstrumentType type) {
  BarelyInstrumentDefinition definition;
  const auto status = BarelyExamples_GetInstrumentDefinition(
      static_cast<BarelyExamplesInstrumentType>(type), &definition);
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

}  // namespace barely::examples
#endif  // __cplusplus

#endif  // EXAMPLES_API_INSTRUMENTS_H_
