#ifndef BARELYMUSICIAN_ENGINE_ARP_STATE_H_
#define BARELYMUSICIAN_ENGINE_ARP_STATE_H_

#include <cassert>
#include <cmath>
#include <optional>

namespace barely {

struct ArpState {
  double rate = 0.0;

  double ratio = 0.0;

  double phase = 0.0;

  float pitch = 0.0f;

  int pitch_index = -1;

  bool is_active = false;

  std::optional<double> GetNextDuration() const noexcept {
    if (rate <= 0.0) {
      return std::nullopt;
    }
    if (!is_active) {
      return (phase < ratio) ? 0.0 : (1.0 - phase) / rate;
    }
    return (ratio - phase) / rate;
  }

  void Update(double duration) noexcept {
    assert(GetNextDuration().has_value() && duration <= *GetNextDuration());
    phase = std::fmod(phase + duration * rate, 1.0);
  }
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ARP_STATE_H_
