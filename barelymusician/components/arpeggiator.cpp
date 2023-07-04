#include "barelymusician/components/arpeggiator.h"

#include <algorithm>

#include "barelymusician/barelymusician.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(Musician& musician, int process_order) noexcept
    : musician_(musician), performer_(musician_.CreatePerformer()) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  performer_.CreateTask(
      [this, process_order]() noexcept {
        Update();
        const double pitch = pitches_[index_];
        instrument_.SetNoteOn(pitch);
        performer_.CreateTask(
            [this, pitch]() { instrument_.SetNoteOff(pitch); },
            /*is_one_off*/ true, gate_ratio_ * *performer_.GetLoopLength(),
            process_order);
      },
      /*is_one_off=*/false, 0.0, process_order);
}

Arpeggiator::~Arpeggiator() {
  musician_.DestroyPerformer(performer_);
  instrument_.SetAllNotesOff();
}

bool Arpeggiator::IsNoteOn(double pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool Arpeggiator::IsPlaying() const noexcept { return *performer_.IsPlaying(); }

void Arpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void Arpeggiator::SetGateRatio(double gate_ratio) noexcept {
  gate_ratio_ = std::min(std::max(gate_ratio, 0.0), 1.0);
}

void Arpeggiator::SetInstrument(InstrumentRef instrument) noexcept {
  instrument_.SetAllNotesOff();
  instrument_ = instrument;
}

void Arpeggiator::SetNoteOff(double pitch) noexcept {
  if (const auto it = std::find(pitches_.begin(), pitches_.end(), pitch);
      it != pitches_.end()) {
    pitches_.erase(it);
    if (pitches_.empty() && IsPlaying()) {
      Stop();
    }
  }
}

void Arpeggiator::SetNoteOn(double pitch) noexcept {
  if (const auto it = std::lower_bound(pitches_.begin(), pitches_.end(), pitch);
      it == pitches_.end() || *it != pitch) {
    pitches_.insert(it, pitch);
    if (!pitches_.empty() && !IsPlaying()) {
      performer_.Start();
    }
  }
}

void Arpeggiator::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(length);
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
  performer_.Stop();
  performer_.SetPosition(0.0);
  instrument_.SetAllNotesOff();
  index_ = -1;
}

}  // namespace barely
