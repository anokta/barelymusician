#ifndef BARELYMUSICIAN_INSTRUMENT_POLYPHONIC_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_POLYPHONIC_INSTRUMENT_H_

#include <memory>
#include <vector>

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/instrument/voice.h"

namespace barelyapi {

// Base instrument class that allows polyphony of a desired voice.
// This class is *not* thread-safe.
// TODO(#11): Should it ensure thread-safety between NoteOn/Off vs Next/Reset?
class PolyphonicInstrument : public Instrument {
 public:
  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) override;
  void NoteOff(float index) override;

 protected:
  // List of all voices.
  std::vector<std::unique_ptr<Voice>> voices_;

 private:
  // List of currently active voices (with their corresponding note index).
  std::vector<std::pair<Voice*, float>> active_voices_;

  // List of currently free voices.
  std::vector<Voice*> free_voices_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_POLYPHONIC_INSTRUMENT_H_
