#ifndef BARELYMUSICIAN_ENGINE_LFO_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_LFO_CONTROLLER_H_

#include <algorithm>
#include <cmath>

#include "dsp/sample_generators.h"
#include "engine/engine_state.h"

namespace barely {

class LfoController {
 public:
  explicit LfoController(EngineState& engine) noexcept : engine_(engine) {}

  [[nodiscard]] uint32_t Acquire() noexcept {
    const uint32_t lfo_index = engine_.lfo_pool.Acquire();
    if (lfo_index != kInvalidIndex) {
      auto& lfo = engine_.GetLfo(lfo_index);
      lfo = {};
    }
    return lfo_index;
  }

  void Release(uint32_t lfo_index) noexcept { engine_.lfo_pool.Release(lfo_index); }

  void Reset(uint32_t lfo_index) noexcept {
    LfoState& lfo = engine_.lfo_pool.Get(lfo_index);
    lfo.phase = 0.0;
    UpdateValue(lfo);
  }

  void SetControl(uint32_t lfo_index, BarelyLfoControlType type, float value) noexcept {
    assert(type <= BarelyLfoControlType_kCount);
    LfoState& lfo = engine_.lfo_pool.Get(lfo_index);
    switch (type) {
      case BarelyLfoControlType_kNoiseMix:
        lfo.noise_mix = kInstrumentControls[type].Clamp(value);
        break;
      case BarelyLfoControlType_kShape:
        lfo.shape = kInstrumentControls[type].Clamp(value);
        break;
      case BarelyLfoControlType_kSkew:
        lfo.skew = kInstrumentControls[type].Clamp(value);
        break;
      case BarelyLfoControlType_kDepth:
        lfo.depth = kInstrumentControls[type].Clamp(value);
        break;
      case BarelyLfoControlType_kPhaseOffset:
        lfo.phase_offset = kInstrumentControls[type].Clamp(value);
        break;
      default:
        assert(!"Invalid lfo control type");
        return;
    }
  }

  void SetSpeed(uint32_t lfo_index, double speed) noexcept {
    engine_.lfo_pool.Get(lfo_index).speed = std::max(speed, 0.0);
  }

  void Update(double duration) noexcept {
    for (uint32_t i = 0; i < engine_.lfo_pool.ActiveCount(); ++i) {
      LfoState& lfo = engine_.GetLfo(engine_.lfo_pool.GetActive(i));
      if (lfo.speed > 0.0) {
        lfo.phase = std::fmod(lfo.phase + lfo.speed * duration, 1.0);
        UpdateValue(lfo);
      }
    }
  }

 private:
  void UpdateValue(LfoState& lfo) noexcept {
    const float phase = static_cast<float>(lfo.phase) + lfo.phase_offset;
    lfo.value = lfo.depth * std::lerp(GenerateOscSample(lfo.shape, phase, 0.0f),
                                      GenerateNoiseSample(engine_.main_rng), lfo.noise_mix);
  }

  EngineState& engine_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_LFO_CONTROLLER_H_
