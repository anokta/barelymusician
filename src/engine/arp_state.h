#ifndef BARELYMUSICIAN_ENGINE_ARP_STATE_H_
#define BARELYMUSICIAN_ENGINE_ARP_STATE_H_

#include <cstdint>

namespace barely {

struct ArpState {
  double phase = 0.0;

  uint32_t note_index = UINT32_MAX;

  bool is_note_on = false;
  bool should_release_note = false;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ARP_STATE_H_
