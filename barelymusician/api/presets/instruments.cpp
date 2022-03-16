#include "barelymusician/api/presets/instruments.h"

#include "barelymusician/barelymusician.h"
#include "barelymusician/presets/instruments/drumkit_instrument.h"
#include "barelymusician/presets/instruments/synth_instrument.h"

extern "C" {

BarelyStatus BarelyPresets_GetInstrumentDefinition(
    BarelyPresetsInstrumentType type,
    BarelyInstrumentDefinition* out_definition) {
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  switch (type) {
    case BarelyPresetsInstrumentType_kSynth:
      *out_definition = barelyapi::SynthInstrument::GetDefinition();
      break;
    case BarelyPresetsInstrumentType_kDrumkit:
      *out_definition = barelyapi::DrumkitInstrument::GetDefinition();
      break;
    default:
      return BarelyStatus_kUnimplemented;
  }
  return BarelyStatus_kOk;
}

}  // extern "C"
