#include "composition/arpeggiator.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(Engine& engine, Instrument instrument) noexcept
    : engine_(engine),
      instrument_(instrument),
      performer_(engine_.CreatePerformer()),
      task_(performer_.CreateTask(0.0, 1.0, 0, [this](TaskEventType type) noexcept {
        if (type == TaskEventType::kBegin) {
          Update();
          instrument_.SetNoteOn(pitch_);
          if (note_callback_) {
            note_callback_(pitch_);
          }
        } else if (type == TaskEventType::kEnd) {
          instrument_.SetNoteOff(pitch_);
        }
      })) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(loop_length_);
}

void Arpeggiator::SetAllNotesOff() noexcept {
  Stop();
  for (const float pitch : pitches_) {
    instrument_.SetNoteOff(pitch);
  }
  pitches_.clear();
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

void Arpeggiator::SetRate(double rate) noexcept {
  loop_length_ = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(loop_length_);
  task_.SetDuration(loop_length_ * gate_ratio_);
}

void Arpeggiator::Stop() noexcept {
  performer_.Stop();
  performer_.SetPosition(0.0);
  index_ = -1;
}

void Arpeggiator::Update() noexcept {
  const int size = static_cast<int>(pitches_.size());
  switch (mode_) {
    case Mode::kUp:
      index_ = (index_ + 1) % size;
      break;
    case Mode::kDown:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case Mode::kRandom:
      index_ = engine_.GenerateRandomNumber(0, size);
      break;
    default:
      assert(!"Invalid arpeggiator mode");
      return;
  }
  assert(index_ >= 0 && index_ < static_cast<int>(pitches_.size()));
  pitch_ = pitches_[index_];
}

}  // namespace barely::examples
