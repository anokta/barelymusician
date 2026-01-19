#include "composition/repeater.h"

#include <barelymusician.h>

#include <cassert>
#include <optional>
#include <utility>

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
Repeater::Repeater(Engine& engine, Instrument instrument) noexcept
    : engine_(engine),
      instrument_(instrument),
      performer_(engine_.CreatePerformer()),
      task_(engine_.CreateTask(performer_, 0.0, 1.0, 0, [this](TaskEventType type) noexcept {
        if (type == TaskEventType::kBegin) {
          OnBeat();
        }
      })) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
}

void Repeater::Clear() noexcept {
  if (IsPlaying()) {
    instrument_.SetAllNotesOff();
  }
  pitches_.clear();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Pop() noexcept {
  if (pitches_.empty()) {
    return;
  }
  if (index_ == static_cast<int>(pitches_.size()) - 1 && IsPlaying()) {
    instrument_.SetNoteOff(*pitches_.back().first + pitch_offset_);
    remaining_length_ = 0;
  }
  pitches_.pop_back();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Push(std::optional<float> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Start(float pitch_offset) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_offset_ = pitch_offset;
  performer_.Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Stop() noexcept {
  if (!IsPlaying()) {
    return;
  }
  performer_.Stop();
  performer_.SetPosition(0.0);
  instrument_.SetAllNotesOff();
  index_ = -1;
  remaining_length_ = 0;
}

void Repeater::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(length);
  task_.SetDuration(length);
}

void Repeater::SetStyle(RepeaterMode style) noexcept { mode_ = style; }

void Repeater::OnBeat() noexcept {
  if (pitches_.empty()) {
    return;
  }
  if (index_ != -1 && pitches_[index_].first.has_value() && remaining_length_ == 1) {
    instrument_.SetNoteOff(*pitches_[index_].first + pitch_offset_);
  }
  if (!Update()) {
    return;
  }
  if (!pitches_[index_].first.has_value()) {
    return;
  }
  instrument_.SetNoteOn(*pitches_[index_].first + pitch_offset_);
}

bool Repeater::Update() noexcept {
  if (--remaining_length_ > 0 || pitches_.empty()) {
    return false;
  }
  const int size = static_cast<int>(pitches_.size());
  switch (mode_) {
    case RepeaterMode::kForward:
      index_ = (index_ + 1) % size;
      break;
    case RepeaterMode::kBackward:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case RepeaterMode::kRandom:
      index_ = engine_.GenerateRandomNumber(0, size);
      break;
    default:
      assert(!"Invalid repeater style");
      return false;
  }
  remaining_length_ = pitches_[index_].second;
  return true;
}

}  // namespace barely::examples
