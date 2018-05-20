#include "barelymusician/instrument/voice.h"

#include <algorithm>

namespace barelyapi {

Voice::Voice(float sample_interval)
    : envelope_(sample_interval), oscillator_(sample_interval), gain_(1.0f) {}

float Voice::Next() { return gain_ * envelope_.Next() * oscillator_.Next(); }

void Voice::Reset() {
  envelope_.Reset();
  oscillator_.Reset();
}

void Voice::SetGain(float gain) { gain_ = std::max(gain, 0.0f); }

void Voice::Start() {
  oscillator_.Reset();
  envelope_.Start();
}

void Voice::Stop() { envelope_.Stop(); }

}  // namespace barelyapi
