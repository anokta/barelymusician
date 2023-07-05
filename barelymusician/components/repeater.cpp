#include "barelymusician/components/repeater.h"

#include <algorithm>
#include <optional>

#include "barelymusician/barelymusician.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Repeater::Repeater(Musician& musician, int process_order) noexcept
    : musician_(musician), performer_(musician_.CreatePerformer()) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  performer_.CreateTask(
      [this, process_order]() noexcept {
        if (pitches_.empty() || !Update()) {
          return;
        }
        const auto& [pitch_or, length] = pitches_[index_];
        if (!pitches_[index_].first.has_value()) {
          return;
        }
        const double pitch = *pitches_[index_].first + pitch_shift_;
        instrument_.SetNoteOn(pitch);
        performer_.CreateTask(
            [this, pitch]() { instrument_.SetNoteOff(pitch); },
            /*is_one_off*/ true,
            static_cast<double>(length) * *performer_.GetLoopLength(),
            process_order);
      },
      /*is_one_off=*/false, 0.0, process_order);
}

Repeater::~Repeater() {
  musician_.DestroyPerformer(performer_);
  instrument_.SetAllNotesOff();
}

void Repeater::Clear() noexcept { pitches_.clear(); }

bool Repeater::IsPlaying() const noexcept { return *performer_.IsPlaying(); }

void Repeater::Pop() noexcept {
  if (pitches_.empty()) {
    return;
  }
  if (index_ == static_cast<int>(pitches_.size()) - 1 && IsPlaying()) {
    instrument_.SetNoteOff(*pitches_.back().first + pitch_shift_);
    remaining_length_ = 0;
  }
  pitches_.pop_back();
}

void Repeater::Push(std::optional<double> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

void Repeater::SetInstrument(InstrumentRef instrument) noexcept {
  instrument_.SetAllNotesOff();
  instrument_ = instrument;
}

void Repeater::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(length);
}

void Repeater::SetStyle(RepeaterStyle style) noexcept { style_ = style; }

void Repeater::Start(double pitch_shift) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_shift_ = pitch_shift;
  performer_.Start();
}

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

bool Repeater::Update() noexcept {
  if (--remaining_length_ > 0 || pitches_.empty()) {
    return false;
  }
  const int size = static_cast<int>(pitches_.size());
  switch (style_) {
    case RepeaterStyle::kForward:
      index_ = (index_ + 1) % size;
      break;
    case RepeaterStyle::kBackward:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case RepeaterStyle::kRandom:
      index_ = random_.DrawUniform(0, size - 1);
  }
  remaining_length_ = pitches_[index_].second;
  return true;
}

}  // namespace barely
