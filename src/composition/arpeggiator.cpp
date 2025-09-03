#include "composition/arpeggiator.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>

#include "api/engine.h"
#include "api/instrument.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(BarelyEngine& engine, BarelyInstrument& instrument) noexcept
    : engine_(engine),
      instrument_(instrument),
      performer_(engine_),
      task_(performer_, 0.0, 1.0, 0,
            {[](BarelyTaskEventType type, void* user_data) {
               auto& arpeggiator = *static_cast<Arpeggiator*>(user_data);
               if (type == BarelyTaskEventType_kBegin) {
                 arpeggiator.Update();
                 arpeggiator.SetNextNoteOn();
               } else if (type == BarelyTaskEventType_kEnd) {
                 arpeggiator.SetNextNoteOff();
               }
             },
             this}) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
}

bool Arpeggiator::IsNoteOn(float pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool Arpeggiator::IsPlaying() const noexcept { return performer_.IsPlaying(); }

void Arpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void Arpeggiator::SetGateRatio(float gate_ratio) noexcept {
  task_.SetDuration(static_cast<double>(gate_ratio) * performer_.GetLoopLength());
}

void Arpeggiator::SetMode(BarelyArpeggiatorMode mode) noexcept {
  if (mode == BarelyArpeggiatorMode_kNone && mode_ != BarelyArpeggiatorMode_kNone) {
    instrument_.StopAllNotes();
  }
  mode_ = mode;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Arpeggiator::SetNoteOff(float pitch) noexcept {
  if (const auto it = std::find(pitches_.begin(), pitches_.end(), pitch); it != pitches_.end()) {
    pitches_.erase(it);
    if (pitches_.empty() && IsPlaying()) {
      Stop();
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Arpeggiator::SetNoteOn(float pitch) noexcept {
  if (const auto it = std::lower_bound(pitches_.begin(), pitches_.end(), pitch);
      it == pitches_.end() || *it != pitch) {
    pitches_.insert(it, pitch);
    if (!pitches_.empty() && !IsPlaying()) {
      performer_.Start();
    }
  }
}

void Arpeggiator::SetRate(float gate_ratio, float rate) noexcept {
  performer_.SetLoopLength((rate > 0.0f) ? 1.0 / static_cast<double>(rate) : 0.0);
  task_.SetDuration(static_cast<double>(gate_ratio) * performer_.GetLoopLength());
}

void Arpeggiator::SetNextNoteOff() noexcept { instrument_.StopNote(pitch_); }

void Arpeggiator::SetNextNoteOn() noexcept { instrument_.StartNote(pitch_, {}); }

void Arpeggiator::Stop() noexcept {
  performer_.Stop();
  performer_.SetPosition(0.0);
  index_ = -1;
}

void Arpeggiator::Update() noexcept {
  const int size = static_cast<int>(pitches_.size());
  switch (mode_) {
    case BarelyArpeggiatorMode_kUp:
      index_ = (index_ + 1) % size;
      break;
    case BarelyArpeggiatorMode_kDown:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case BarelyArpeggiatorMode_kRandom:
      index_ = engine_.main_rng().Generate(0, size);
      break;
    default:
      assert(!"Invalid arpeggiator style");
      return;
  }
  assert(index_ >= 0 && index_ < static_cast<int>(pitches_.size()));
  pitch_ = pitches_[index_];
}

}  // namespace barely
