#include "barelymusician/sequencers/metronome.h"

#include <utility>

namespace barely {

namespace {

// Default metronome priority.
constexpr int kPriority = -1;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Metronome::Metronome(Engine& engine) noexcept
    : sequencer_(engine.CreateSequencer(kPriority)) {
  sequencer_.SetLooping(true);
  sequencer_.SetLoopLength(1.0);
  sequencer_.AddEvent(0.0, [this]() {
    if (callback_) {
      callback_(beat_);
    }
    ++beat_;
  });
}

bool Metronome::IsPlaying() const noexcept { return sequencer_.IsPlaying(); }

void Metronome::Reset() noexcept {
  sequencer_.Stop();
  sequencer_.SetPosition(0.0);
  beat_ = 0;
}

void Metronome::SetBeatCallback(BeatCallback callback) noexcept {
  callback_ = std::move(callback);
}

void Metronome::Start() noexcept { sequencer_.Start(); }

void Metronome::Stop() noexcept { sequencer_.Stop(); }

}  // namespace barely
