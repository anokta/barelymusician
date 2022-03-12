#include "examples/api/instruments.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include "barelymusician/api/instrument.h"
#include "barelymusician/api/status.h"
// #include "examples/instruments/drumkit_instrument.h"
// #include "examples/instruments/synth_instrument.h"

extern "C" {

BarelyStatus BarelyExamples_GetInstrumentDefinition(
    BarelyExamplesInstrumentType type,
    BarelyInstrumentDefinition* out_definition) {
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  switch (type) {
    case BarelyExamplesInstrumentType_kSynth:
      // *out_definition = barely::examples::SynthInstrument::GetDefinition();
      // break;
    case BarelyExamplesInstrumentType_kDrumkit:
      // *out_definition = barely::examples::DrumkitInstrument::GetDefinition();
      // break;
    default:
      return BarelyStatus_kUnimplemented;
  }
  // return BarelyStatus_kOk;
}

}  // extern "C"
