#include "barelymusician/components/metronome.h"

#include <utility>

#include "barelymusician/barelymusician.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Metronome::Metronome(Musician& musician, int process_order) noexcept
    : musician_(musician), performer_(musician_.CreatePerformer()) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  performer_.CreateTask(
      [this]() noexcept {
        if (callback_) {
          callback_(beat_);
        }
        ++beat_;
      },
      /*is_one_off=*/false, 0.0, process_order);
}

Metronome::~Metronome() { musician_.DestroyPerformer(performer_); }

bool Metronome::IsPlaying() const noexcept { return *performer_.IsPlaying(); }

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

}  // namespace barely
