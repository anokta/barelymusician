#include "examples/performers/metronome.h"

#include <utility>

#include "barelymusician.h"

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
Metronome::Metronome(MusicianHandle musician, int process_order) noexcept
    : musician_(std::move(musician)), performer_(musician_.AddPerformer(process_order)) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  performer_.AddTask(
      [this]() noexcept {
        if (callback_) {
          callback_(beat_);
        }
        ++beat_;
      },
      0.0);
}

Metronome::~Metronome() noexcept { musician_.RemovePerformer(performer_); }

bool Metronome::IsPlaying() const noexcept { return performer_.IsPlaying(); }

void Metronome::Reset() noexcept {
  performer_.Stop();
  performer_.SetPosition(0.0);
  beat_ = 0;
}

void Metronome::SetBeatCallback(BeatCallback callback) noexcept { callback_ = std::move(callback); }

void Metronome::Start() noexcept { performer_.Start(); }

void Metronome::Stop() noexcept { performer_.Stop(); }

}  // namespace barely::examples
