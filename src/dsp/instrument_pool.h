#ifndef BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_
#define BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_

#include <array>
#include <utility>

#include "dsp/instrument_params.h"

namespace barely {

/// Maximum number of instruments.
inline constexpr int kMaxInstrumentCount = BARELYMUSICIAN_MAX_INSTRUMENT_COUNT;

class InstrumentPool {
 public:
  InstrumentPool() noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < kMaxInstrumentCount; ++i) {
      active_instruments_[i] = i;
    }
  }

  [[nodiscard]] InstrumentIndex Acquire() noexcept {
    if (active_instrument_count_ < kMaxInstrumentCount) {
      return active_instrument_count_++;
    }
    return -1;  // TODO(#126): Handle failure properly.
  }

  void Destroy(InstrumentIndex index) noexcept {
    // TODO(#126): This can be avoided by switching to intrusive lists.
    for (int i = 0; i < active_instrument_count_; ++i) {
      if (active_instruments_[i] == index) {
        std::swap(active_instruments_[i], active_instruments_[active_instrument_count_ - 1]);
        --active_instrument_count_;
        break;
      }
    }
  }

  [[nodiscard]] InstrumentParams& Get(InstrumentIndex index) noexcept {
    assert(index >= 0);
    assert(index < kMaxInstrumentCount);
    return instruments_[index];
  }

 private:
  std::array<InstrumentIndex, kMaxInstrumentCount> active_instruments_;
  std::array<InstrumentParams, kMaxInstrumentCount> instruments_;

  int active_instrument_count_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_INSTRUMENT_POOL_H_
