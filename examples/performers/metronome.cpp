#include "examples/performers/metronome.h"

#include <utility>

namespace barely::examples {

namespace {

// Default metronome priority.
constexpr int kPriority = -1;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Metronome::Metronome(Performer&& performer) noexcept
    : performer_(std::move(performer)) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  performer_.CreateTask(
      [this]() {
        if (callback_) {
          callback_(beat_);
        }
        ++beat_;
      },
      0.0);
}

bool Metronome::IsPlaying() const noexcept { return performer_.IsPlaying(); }

void Metronome::Reset() noexcept {
  performer_.Stop();
  performer_.SetPosition(0.0);
  beat_ = 0;
}

void Metronome::SetBeatCallback(BeatCallback callback) noexcept {
  callback_ = std::move(callback);
}

void Metronome::Start() noexcept { performer_.Start(); }

void Metronome::Stop() noexcept { performer_.Stop(); }

}  // namespace barely::examples
