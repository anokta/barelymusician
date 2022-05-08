#include "platforms/unity/unity.h"

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "barelymusician/instruments/synth_instrument.h"

extern "C" {

BarelyStatus BarelyUnityInstrument_Create(BarelyMusicianHandle handle,
                                          BarelyUnityInstrumentType type,
                                          int32_t frame_rate,
                                          BarelyId* out_instrument_id) {
  switch (type) {
    case BarelyUnityInstrumentType_kPercussion:
      return BarelyInstrument_Create(
          handle, barely::PercussionInstrument::GetDefinition(), frame_rate,
          out_instrument_id);
    case BarelyUnityInstrumentType_kSynth:
      return BarelyInstrument_Create(handle,
                                     barely::SynthInstrument::GetDefinition(),
                                     frame_rate, out_instrument_id);
  }

  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
