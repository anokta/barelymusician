#include "barelymusician/components/metronome.h"

#include <utility>

#include "barelymusician/barelymusician.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Metronome::Metronome(MusicianPtr musician, int process_order) noexcept
    : performer_(musician, process_order),
      task_(
          performer_,
          [this]() noexcept {
            if (callback_) {
              callback_(beat_);
            }
            ++beat_;
          },
          0.0) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
}

bool Metronome::IsPlaying() const noexcept { return performer_.IsPlaying(); }

void Metronome::Reset() noexcept {
  performer_.Stop();
  performer_.SetPosition(0.0);
  beat_ = 0;
}

void Metronome::SetBeatCallback(BeatCallback callback) noexcept { callback_ = std::move(callback); }

void Metronome::Start() noexcept { performer_.Start(); }

void Metronome::Stop() noexcept { performer_.Stop(); }

}  // namespace barely
