#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kSampleRate = 16;
constexpr int kNumChannels = 1;
constexpr int kInstrumentId = 5;

// Test instrument that produces constant output per note for testing.
class TestInstrument : public Instrument {
 public:
  TestInstrument() : sample_(0.0f) {}

  // Implements |Instrument|.
  void Control(int, float) override {}
  void NoteOff(float) override { sample_ = 0.0f; }
  void NoteOn(float pitch, float intensity) override {
    sample_ = index * intensity;
  }
  void Process(float* output, int num_channels, int num_frames) override {
    std::fill_n(output, num_channels * num_frames, sample_);
  }
  
 private:
  float sample_;
};

// Tests that the engine creates and destroy instruments as expected.
TEST(EngineTest, CreateDestroy) {
  const float kPitch = 10.0f;
  const float kNoteIntensity = 0.75f;

  Engine engine(kSampleRate);

  std::vector<float> buffer(kNumChannels * kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Create instrument and start note.
  engine.Create(kInstrumentId, std::make_unique<TestInstrument>());
  engine.NoteOn(kInstrumentId, kPitch, kNoteIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kNoteIntensity);
    }
  }

  // Destroy instrument.
  engine.Destroy(kInstrumentId);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing a single note produces the expected output.
TEST(EngineTest, ScheduleSingleNote) {
  const float kPitch = 32.0f;
  const float kNoteIntensity = 0.5f;

  Engine engine(kSampleRate);
  engine.Create(kInstrumentId, std::make_unique<TestInstrument>());

  engine.SetTempo(kSecondsFromMinutes);
  engine.Start();

  std::vector<float> buffer(kNumChannels * kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  engine.ScheduleNoteOn(kInstrumentId, 0.0, kPitch, kNoteIntensity);
  engine.Update(kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kPitch * kNoteIntensity);
    }
  }

  // Stop note.
  engine.ScheduleNoteOff(kInstrumentId, 1.0, kPitch);
  engine.Update(kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing multiple notes produces the expected output.
TEST(EngineTest, ScheduleMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  Engine engine(kSampleRate);
  engine.Create(kInstrumentId, std::make_unique<TestInstrument>());

  engine.SetTempo(kSecondsFromMinutes);
  engine.Start();

  std::vector<float> buffer(kNumChannels * kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kSampleRate; ++i) {
    engine.ScheduleNoteOn(
        kInstrumentId,
        static_cast<double>(i) / static_cast<double>(kSampleRate),
        static_cast<float>(i), kNoteIntensity);
  }
  engine.Update(kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  engine.Stop();
  engine.Update(kSampleRate);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  engine.Process(kInstrumentId, buffer.data(), kNumChannels, kSampleRate);
  for (int frame = 0; frame < kSampleRate; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playback triggers the beat callback as expected.
TEST(EngineTest, SetBeatCallback) {
  const int kStartBeat = 5;
  const int kNumBeats = 4;

  Engine engine(kSampleRate);
  engine.SetPosition(static_cast<double>(kStartBeat));
  engine.SetTempo(kSecondsFromMinutes);

  int current_beat = kStartBeat;
  engine.SetBeatCallback([&](int beat) {
    EXPECT_EQ(beat, current_beat);
    ++current_beat;
  });

  engine.Update(kSampleRate * kNumBeats);
  EXPECT_EQ(current_beat, kStartBeat);

  engine.Start();
  engine.Update(kSampleRate * kNumBeats);
  EXPECT_EQ(current_beat, kStartBeat + kNumBeats);

  engine.Stop();
  engine.Update(kSampleRate * kNumBeats);
  EXPECT_EQ(current_beat, kStartBeat + kNumBeats);
}

// Tests that playing notes triggers the corresponding callbacks as expected.
TEST(EngineTest, SetNoteCallbacks) {
  const float kPitch = 40.0f;
  const float kNoteIntensity = 0.75f;

  Engine engine(kSampleRate);
  engine.Create(kInstrumentId, std::make_unique<TestInstrument>());

  // Trigger note on.
  int note_on_instrument_id = 0;
  float note_on_index = 0.0f;
  float note_on_intensity = 0.0f;
  engine.SetNoteOnCallback(
      [&](int instrument_id, float pitch, float intensity) {
        note_on_instrument_id = instrument_id;
        note_on_index = index;
        note_on_intensity = intensity;
      });
  EXPECT_NE(note_on_instrument_id, kInstrumentId);
  EXPECT_NE(note_on_index, kPitch);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  engine.NoteOn(kInstrumentId, kPitch, kNoteIntensity);
  EXPECT_EQ(note_on_instrument_id, kInstrumentId);
  EXPECT_FLOAT_EQ(note_on_index, kPitch);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // Trigger note off.
  int note_off_instrument_id = 0;
  float note_off_index = 0.0f;
  engine.SetNoteOffCallback([&](int instrument_id, float pitch) {
    note_off_instrument_id = instrument_id;
    note_off_index = index;
  });
  EXPECT_NE(note_off_instrument_id, kInstrumentId);
  EXPECT_NE(note_off_index, kPitch);

  engine.NoteOff(kInstrumentId, kPitch);
  EXPECT_EQ(note_off_instrument_id, kInstrumentId);
  EXPECT_FLOAT_EQ(note_off_index, kPitch);
}

// Tests that the engine starts and stops playback as expected.
TEST(EngineTest, StartStop) {
  Engine engine(kSampleRate);
  engine.SetTempo(kSecondsFromMinutes);

  EXPECT_DOUBLE_EQ(engine.GetPosition(), 0.0);
  engine.Update(kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetPosition(), 0.0);

  engine.Start();
  engine.Update(kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetPosition(), 1.0);

  engine.Stop();
  engine.Update(kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetPosition(), 1.0);

  engine.SetPosition(0.0);
  EXPECT_DOUBLE_EQ(engine.GetPosition(), 0.0);
}

}  // namespace
}  // namespace barelyapi
