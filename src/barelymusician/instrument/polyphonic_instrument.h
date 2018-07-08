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
template <class VoiceType>
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
  std::vector<std::unique_ptr<VoiceType>> voices_;

 private:
  // List of currently active voices (with their corresponding note index).
  std::vector<std::pair<VoiceType*, float>> active_voices_;

  // List of currently free voices.
  std::vector<VoiceType*> free_voices_;
};

template <class VoiceType>
float PolyphonicInstrument<VoiceType>::Next() {
  float output = 0.0f;
  for (auto& voice : voices_) {
    if (voice->IsActive()) {
      output += voice->Next();
    }
  }
  return output;
}

template <class VoiceType>
void PolyphonicInstrument<VoiceType>::Reset() {
  active_voices_.clear();
  free_voices_.clear();
  for (const auto& voice : voices_) {
    voice->Reset();
    free_voices_.push_back(voice.get());
  }
}

template <class VoiceType>
void PolyphonicInstrument<VoiceType>::NoteOn(float index, float intensity) {
  // TODO(#12): Do we really need dynamic memory allocation here?
  VoiceType* voice = nullptr;
  if (!free_voices_.empty()) {
    // Acquire a free voice.
    voice = free_voices_.front();
    free_voices_.erase(free_voices_.begin());
    active_voices_.emplace_back(voice, index);
  } else {
    // If no free voices available, steal the last used active voice.
    voice = active_voices_.front().first;
    active_voices_.front().second = index;
  }
  voice->Start(index, intensity);
}

template <class VoiceType>
void PolyphonicInstrument<VoiceType>::NoteOff(float index) {
  for (auto it = active_voices_.begin(); it != active_voices_.end(); ++it) {
    if (it->second == index) {
      it->first->Stop();
      free_voices_.push_back(it->first);
      active_voices_.erase(it);
      break;
    }
  }
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_POLYPHONIC_INSTRUMENT_H_
