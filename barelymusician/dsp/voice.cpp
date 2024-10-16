#include "barelymusician/dsp/voice.h"

namespace barely {

Voice::Voice(int frame_rate) noexcept
    : envelope_(frame_rate), oscillator_(frame_rate), sample_player_(frame_rate) {}

bool Voice::IsActive() const noexcept { return envelope_.IsActive(); }

double Voice::Next() noexcept {
  return gain_ * envelope_.Next() * (oscillator_.Next() + sample_player_.Next());
}

void Voice::Reset() noexcept { envelope_.Reset(); }

void Voice::Start() noexcept {
  oscillator_.Reset();
  sample_player_.Reset();
  envelope_.Start();
}

void Voice::Stop() noexcept { envelope_.Stop(); }

}  // namespace barely
