#include "barelymusician/instrument/polyphonic_instrument.h"

namespace barelyapi {

float PolyphonicInstrument::Next() {
  float output = 0.0f;
  for (auto& voice : voices_) {
    if (voice->IsActive()) {
      output += voice->Next();
    }
  }
  return output;
}

void PolyphonicInstrument::Reset() {
  active_voices_.clear();
  free_voices_.clear();
  for (const auto& voice : voices_) {
    voice->Reset();
    free_voices_.push_back(voice.get());
  }
}

void PolyphonicInstrument::NoteOn(float index, float intensity) {
  // TODO(#12): Do we really need dynamic memory allocation here?
  Voice* voice = nullptr;
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

void PolyphonicInstrument::NoteOff(float index) {
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
