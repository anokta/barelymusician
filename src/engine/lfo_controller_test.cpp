#include "engine/lfo_controller.h"

#include <cstdint>
#include <memory>

#include "engine/engine_state.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(LfoControllerTest, Evaluate) {
  constexpr int kSampleRate = 48000;

  const auto size = GetAllocSize<EngineState>(EngineConfig(kSampleRate));
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  EngineState engine(arena, EngineConfig(kSampleRate));
  LfoController controller(engine);

  const uint32_t lfo_index = controller.Acquire();
  auto& lfo = engine.GetLfo(lfo_index);

  EXPECT_DOUBLE_EQ(lfo.phase, 0.0);
  EXPECT_DOUBLE_EQ(controller.Evaluate(lfo_index), 0.0);

  controller.Update(1.0);
  EXPECT_DOUBLE_EQ(controller.Evaluate(lfo_index), 0.0);

  lfo.shape = 1.0f;  // sawtooth

  controller.Update(0.5);
  EXPECT_DOUBLE_EQ(controller.Evaluate(lfo_index), -1.0);

  controller.SetPhase(lfo_index, 0.0);
  EXPECT_DOUBLE_EQ(controller.Evaluate(lfo_index), 0.0);

  controller.SetSpeed(lfo_index, 2.0);
  controller.Update(0.25);
  EXPECT_DOUBLE_EQ(controller.Evaluate(lfo_index), -1.0);
}

}  // namespace
}  // namespace barely
