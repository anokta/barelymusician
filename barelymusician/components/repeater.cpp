#include "barelymusician/components/repeater.h"

#include <optional>
#include <utility>

#include "barelymusician/barelycomposer.h"
#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"

namespace barely::internal {

namespace {

constexpr double kNoteIntensity = 1.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Repeater::Repeater(Musician& musician, int process_order) noexcept
    : musician_(musician), performer_(musician_.AddPerformer(process_order)) {
  performer_->SetLooping(true);
  performer_->SetLoopLength(1.0);
  // TODO(#126): This should not need `TaskEvent::Callback`.
  TaskEvent::Callback callback = [this]() noexcept {
    if (pitches_.empty() || !Update() || instrument_ == nullptr) {
      return;
    }
    const auto& [pitch_or, length] = pitches_[index_];
    if (!pitches_[index_].first.has_value()) {
      return;
    }
    const double pitch = *pitches_[index_].first + pitch_offset_;
    instrument_->SetNoteOn(pitch, kNoteIntensity);
    TaskEvent::Callback note_off_callback = [this, pitch]() noexcept {
      instrument_->SetNoteOff(pitch);
    };
    performer_->ScheduleOneOffTask(EventWithCallback<TaskEvent>(note_off_callback),
                                   static_cast<double>(length) * performer_->GetLoopLength());
  };
  performer_->AddTask(EventWithCallback<TaskEvent>(callback), 0.0);
}

Repeater::~Repeater() noexcept {
  if (IsPlaying() && instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  musician_.RemovePerformer(performer_);
}

void Repeater::Clear() noexcept { pitches_.clear(); }

bool Repeater::IsPlaying() const noexcept { return performer_->IsPlaying(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Pop() noexcept {
  if (pitches_.empty()) {
    return;
  }
  if (index_ == static_cast<int>(pitches_.size()) - 1 && IsPlaying()) {
    if (instrument_ != nullptr) {
      instrument_->SetNoteOff(*pitches_.back().first + pitch_offset_);
    }
    remaining_length_ = 0;
  }
  pitches_.pop_back();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Push(std::optional<double> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

void Repeater::SetInstrument(InstrumentController* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

void Repeater::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_->SetLoopLength(length);
}

void Repeater::SetStyle(RepeaterStyle style) noexcept { style_ = style; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Start(double pitch_offset) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_offset_ = pitch_offset;
  performer_->Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Stop() noexcept {
  if (!IsPlaying()) {
    return;
  }
  performer_->Stop();
  performer_->SetPosition(0.0);
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
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

}  // namespace barely::internal
