#include "private/arpeggiator.h"

#include <algorithm>
#include <cassert>

#include "barelymusician.h"
#include "private/engine.h"
#include "private/instrument.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyArpeggiator::BarelyArpeggiator(BarelyEngine& engine) noexcept
    : engine_(&engine), performer_(engine_->CreatePerformer()) {
  performer_->SetLooping(true);
  performer_->SetLoopLength(1.0);
  task_ = performer_->CreateTask(0.0, 1.0,
                                 {[](BarelyTaskState state, void* user_data) {
                                    auto& arpeggiator = *static_cast<BarelyArpeggiator*>(user_data);
                                    if (state == BarelyTaskState_kBegin) {
                                      arpeggiator.Update();
                                      arpeggiator.SetNextNoteOn();
                                    } else if (state == BarelyTaskState_kEnd) {
                                      arpeggiator.SetNextNoteOff();
                                    }
                                  },
                                  this});
}

BarelyArpeggiator::~BarelyArpeggiator() noexcept { engine_->DestroyPerformer(performer_); }

bool BarelyArpeggiator::IsNoteOn(float pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool BarelyArpeggiator::IsPlaying() const noexcept { return performer_->IsPlaying(); }

void BarelyArpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void BarelyArpeggiator::SetGateRatio(float gate_ratio) noexcept {
  gate_ratio_ = std::clamp(gate_ratio, 0.0f, 1.0f);
  task_->SetDuration(gate_ratio_ * performer_->GetLoopLength());
}

void BarelyArpeggiator::SetInstrument(BarelyInstrument* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyArpeggiator::SetNoteOff(float pitch) noexcept {
  if (const auto it = std::find(pitches_.begin(), pitches_.end(), pitch); it != pitches_.end()) {
    pitches_.erase(it);
    if (pitches_.empty() && IsPlaying()) {
      Stop();
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyArpeggiator::SetNoteOn(float pitch) noexcept {
  if (const auto it = std::lower_bound(pitches_.begin(), pitches_.end(), pitch);
      it == pitches_.end() || *it != pitch) {
    pitches_.insert(it, pitch);
    if (!pitches_.empty() && !IsPlaying()) {
      performer_->Start();
    }
  }
}

void BarelyArpeggiator::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_->SetLoopLength(length);
  task_->SetDuration(gate_ratio_ * performer_->GetLoopLength());
}

void BarelyArpeggiator::SetStyle(BarelyArpeggiatorStyle style) noexcept { style_ = style; }

void BarelyArpeggiator::SetNextNoteOff() noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetNoteOff(pitch_);
  }
}

void BarelyArpeggiator::SetNextNoteOn() noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetNoteOn(pitch_, {});
  }
}

void BarelyArpeggiator::Stop() noexcept {
  performer_->Stop();
  performer_->SetPosition(0.0);
  index_ = -1;
}

void BarelyArpeggiator::Update() noexcept {
  const int size = static_cast<int>(pitches_.size());
  switch (style_) {
    case BarelyArpeggiatorStyle_kUp:
      index_ = (index_ + 1) % size;
      break;
    case BarelyArpeggiatorStyle_kDown:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case BarelyArpeggiatorStyle_kRandom:
      index_ = engine_->main_rng().Generate(0, size);
      break;
    default:
      assert(!"Invalid arpeggiator style");
      return;
  }
  assert(index_ >= 0 && index_ < static_cast<int>(pitches_.size()));
  pitch_ = pitches_[index_];
}
