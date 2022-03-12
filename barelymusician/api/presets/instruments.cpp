#include "barelymusician/api/presets/instruments.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include "barelymusician/api/instrument.h"
#include "barelymusician/api/status.h"
// #include "presets/instruments/drumkit_instrument.h"
// #include "presets/instruments/synth_instrument.h"

extern "C" {

BarelyStatus BarelyPresets_GetInstrumentDefinition(
    BarelyPresetsInstrumentType type,
    BarelyInstrumentDefinition* out_definition) {
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  switch (type) {
    case BarelyPresetsInstrumentType_kSynth:
      // *out_definition = barely::presets::SynthInstrument::GetDefinition();
      // break;
    case BarelyPresetsInstrumentType_kDrumkit:
      // *out_definition = barely::presets::DrumkitInstrument::GetDefinition();
      // break;
    default:
      return BarelyStatus_kUnimplemented;
  }
  // return BarelyStatus_kOk;
}

}  // extern "C"
