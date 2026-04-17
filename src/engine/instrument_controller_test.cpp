#include "engine/instrument_controller.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <memory>
#include <utility>

#include "core/arena.h"
#include "engine/engine_state.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kSampleRate = 48000;

[[nodiscard]] size_t GetEngineSize() noexcept {
  Arena arena;  // sizing arena
  EngineState().Init(arena, EngineConfig(kSampleRate));
  return AlignUp(arena.offset(), alignof(std::max_align_t)) + alignof(std::max_align_t);
}

TEST(InstrumentControllerTest, SetControl) {
  const size_t size = GetEngineSize();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  EngineState engine;
  engine.Init(arena, EngineConfig(kSampleRate));
  InstrumentController controller(engine);

  const uint32_t instrument_index = controller.Acquire();
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 1.0f);

  controller.SetControl(instrument_index, BarelyInstrumentControlType_kGain, 0.5f);
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 0.5f);

  // Verify that the control value is clamped at the minimum value.
  controller.SetControl(instrument_index, BarelyInstrumentControlType_kGain, -1.0f);
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 0.0f);

  controller.SetControl(instrument_index, BarelyInstrumentControlType_kGain, 0.0f);
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 0.0f);

  // Verify that the control value is clamped at the maximum value.
  controller.SetControl(instrument_index, BarelyInstrumentControlType_kGain, 10.0f);
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 1.0f);

  controller.SetControl(instrument_index, BarelyInstrumentControlType_kGain, 1.0f);
  EXPECT_FLOAT_EQ(controller.GetControl(instrument_index, BarelyInstrumentControlType_kGain), 1.0f);
}

TEST(InstrumentControllerTest, SetNoteCallbacks) {
  constexpr float kPitch = 3.3f;

  const size_t size = GetEngineSize();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  EngineState engine;
  engine.Init(arena, EngineConfig(kSampleRate));
  InstrumentController controller(engine);

  const uint32_t instrument_index = controller.Acquire();

  // Trigger the note on callback.
  std::pair<float, float> note_pitch = {0.0f, 0.0f};
  controller.SetNoteEventCallback(
      instrument_index,
      [](BarelyEventType type, float pitch, void* user_data) {
        (type == BarelyEventType_kBegin
             ? static_cast<std::pair<float, float>*>(user_data)->first
             : static_cast<std::pair<float, float>*>(user_data)->second) = pitch;
      },
      &note_pitch);
  EXPECT_FLOAT_EQ(note_pitch.first, 0.0f);

  controller.SetNoteOn(instrument_index, kPitch);
  EXPECT_FLOAT_EQ(note_pitch.first, kPitch);

  // This should not trigger the callback since the note is already on.
  note_pitch.first = 0.0f;
  controller.SetNoteOn(instrument_index, kPitch);
  EXPECT_FLOAT_EQ(note_pitch.first, 0.0f);

  // Trigger the note on callback again with another note.
  note_pitch.first = 0.0f;
  controller.SetNoteOn(instrument_index, kPitch + 2.0f);
  EXPECT_FLOAT_EQ(note_pitch.first, kPitch + 2.0f);

  // Trigger the note off callback.
  controller.SetNoteOff(instrument_index, kPitch);
  EXPECT_FLOAT_EQ(note_pitch.second, kPitch);

  // This should not trigger the callback since the note is already off.
  note_pitch.second = 0.0;
  controller.SetNoteOff(instrument_index, kPitch);
  EXPECT_FLOAT_EQ(note_pitch.second, 0.0f);

  // Finally, trigger the note off callback with the remaining note.
  controller.SetAllNotesOff(instrument_index);
  EXPECT_FLOAT_EQ(note_pitch.second, kPitch + 2.0f);
}

TEST(InstrumentControllerTest, SetAllNotesOff) {
  constexpr std::array<float, 3> kPitches = {1.0f, 2.0f, 3.0f};

  const size_t size = GetEngineSize();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  EngineState engine;
  engine.Init(arena, EngineConfig(kSampleRate));
  InstrumentController controller(engine);

  const uint32_t instrument_index = controller.Acquire();
  for (const float pitch : kPitches) {
    EXPECT_FALSE(controller.IsNoteOn(instrument_index, pitch));
  }

  // Start multiple notes.
  for (const float pitch : kPitches) {
    controller.SetNoteOn(instrument_index, pitch);
    EXPECT_TRUE(controller.IsNoteOn(instrument_index, pitch));
  }

  // Stop all notes.
  controller.SetAllNotesOff(instrument_index);
  for (const float pitch : kPitches) {
    EXPECT_FALSE(controller.IsNoteOn(instrument_index, pitch));
  }
}

}  // namespace
}  // namespace barely
