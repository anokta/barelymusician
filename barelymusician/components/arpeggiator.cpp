#include "barelymusician/components/arpeggiator.h"

#include <algorithm>

#include "barelymusician/barelycomposer.h"
#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"

namespace barely::internal {

namespace {

constexpr double kNoteIntensity = 1.0;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(Musician& musician, int process_order) noexcept
    : musician_(musician), performer_(musician_.AddPerformer(process_order)) {
  performer_->SetLooping(true);
  performer_->SetLoopLength(1.0);
  // TODO(#126): This should not need `TaskEvent::Callback`.
  TaskEvent::Callback callback = [this]() noexcept {
    Update();
    if (instrument_ == nullptr) {
      return;
    }
    const double pitch = pitches_[index_];
    instrument_->SetNoteOn(pitch, kNoteIntensity);
    TaskEvent::Callback note_off_callback = [this, pitch]() noexcept {
      if (instrument_ != nullptr) {
        instrument_->SetNoteOff(pitch);
      }
    };
    performer_->ScheduleOneOffTask(EventWithCallback<TaskEvent>(note_off_callback),
                                   gate_ratio_ * performer_->GetLoopLength());
  };
  performer_->AddTask(EventWithCallback<TaskEvent>(callback), 0.0);
}

Arpeggiator::~Arpeggiator() noexcept {
  if (IsPlaying() && instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  musician_.RemovePerformer(performer_);
}

bool Arpeggiator::IsNoteOn(double pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool Arpeggiator::IsPlaying() const noexcept { return performer_->IsPlaying(); }

void Arpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void Arpeggiator::SetGateRatio(double gate_ratio) noexcept {
  gate_ratio_ = std::min(std::max(gate_ratio, 0.0), 1.0);
}

void Arpeggiator::SetInstrument(InstrumentController* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Arpeggiator::SetNoteOff(double pitch) noexcept {
  if (const auto it = std::find(pitches_.begin(), pitches_.end(), pitch); it != pitches_.end()) {
    pitches_.erase(it);
    if (pitches_.empty() && IsPlaying()) {
      Stop();
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Arpeggiator::SetNoteOn(double pitch) noexcept {
  if (const auto it = std::lower_bound(pitches_.begin(), pitches_.end(), pitch);
      it == pitches_.end() || *it != pitch) {
    pitches_.insert(it, pitch);
    if (!pitches_.empty() && !IsPlaying()) {
      performer_->Start();
    }
  }
}

void Arpeggiator::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_->SetLoopLength(length);
}

void Arpeggiator::SetStyle(ArpeggiatorStyle style) noexcept { style_ = style; }

void Arpeggiator::Update() noexcept {
  const int size = static_cast<int>(pitches_.size());
  switch (style_) {
    case ArpeggiatorStyle::kUp:
      index_ = (index_ + 1) % size;
      break;
    case ArpeggiatorStyle::kDown:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case ArpeggiatorStyle::kRandom:
      index_ = random_.DrawUniform(0, size - 1);
  }
}

void Arpeggiator::Stop() noexcept {
  performer_->Stop();
  performer_->CancelAllOneOffTasks();
  performer_->SetPosition(0.0);
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  index_ = -1;
}

}  // namespace barely::internal
