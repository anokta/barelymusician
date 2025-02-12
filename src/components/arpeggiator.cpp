#include "components/arpeggiator.h"

#include <algorithm>
#include <cassert>

#include "barelycomposer.h"
#include "barelymusician.h"
#include "engine/engine.h"
#include "engine/instrument.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(Engine& engine) noexcept
    : engine_(&engine), performer_(engine_->CreatePerformer()) {
  performer_->SetLooping(true);
  performer_->SetLoopLength(1.0);
  task_ = performer_->CreateTask(0.0, 1.0,
                                 {[](BarelyTaskState state, void* user_data) {
                                    auto& arpeggiator = *static_cast<Arpeggiator*>(user_data);
                                    if (state == BarelyTaskState_kBegin) {
                                      arpeggiator.Update();
                                      arpeggiator.SetNextNoteOn();
                                    } else if (state == BarelyTaskState_kEnd) {
                                      arpeggiator.SetNextNoteOff();
                                    }
                                  },
                                  this});
}

Arpeggiator::~Arpeggiator() noexcept { engine_->DestroyPerformer(performer_); }

bool Arpeggiator::IsNoteOn(float pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool Arpeggiator::IsPlaying() const noexcept { return performer_->IsPlaying(); }

void Arpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void Arpeggiator::SetGateRatio(float gate_ratio) noexcept {
  gate_ratio_ = std::clamp(gate_ratio, 0.0f, 1.0f);
  task_->SetDuration(gate_ratio_ * performer_->GetLoopLength());
}

void Arpeggiator::SetInstrument(Instrument* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
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
      performer_->Start();
    }
  }
}

void Arpeggiator::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_->SetLoopLength(length);
  task_->SetDuration(gate_ratio_ * performer_->GetLoopLength());
}

void Arpeggiator::SetStyle(ArpeggiatorStyle style) noexcept { style_ = style; }

void Arpeggiator::SetNextNoteOff() noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetNoteOff(pitch_);
  }
}

void Arpeggiator::SetNextNoteOn() noexcept {
  if (instrument_ != nullptr) {
    static constexpr float kNoteIntensity = 1.0f;
    instrument_->SetNoteOn(pitch_, kNoteIntensity);
  }
}

void Arpeggiator::Stop() noexcept {
  performer_->Stop();
  performer_->SetPosition(0.0);
  index_ = -1;
}

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
      index_ = random_.DrawUniform(0, size);
      break;
    default:
      assert(!"Invalid arpeggiator style");
      return;
  }
  assert(index_ >= 0 && index_ < static_cast<int>(pitches_.size()));
  pitch_ = pitches_[index_];
}

}  // namespace barely::internal
