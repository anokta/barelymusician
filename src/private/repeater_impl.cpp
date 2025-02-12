#include "private/repeater_impl.h"

#include <cassert>
#include <optional>
#include <utility>

#include "barelycomposer.h"
#include "barelymusician.h"
#include "private/engine_impl.h"
#include "private/instrument_impl.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
RepeaterImpl::RepeaterImpl(EngineImpl& engine) noexcept
    : engine_(&engine), performer_(engine_->CreatePerformer()) {
  performer_->SetBeatCallback({
      [](void* user_data) noexcept {
        auto& repeater = *static_cast<RepeaterImpl*>(user_data);
        repeater.OnBeat();
      },
      this,
  });
}

RepeaterImpl::~RepeaterImpl() noexcept {
  if (IsPlaying() && instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  engine_->DestroyPerformer(performer_);
}

void RepeaterImpl::Clear() noexcept { pitches_.clear(); }

bool RepeaterImpl::IsPlaying() const noexcept { return performer_->IsPlaying(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void RepeaterImpl::Pop() noexcept {
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
void RepeaterImpl::Push(std::optional<float> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

void RepeaterImpl::SetInstrument(InstrumentImpl* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

void RepeaterImpl::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_->SetLoopLength(length);
}

void RepeaterImpl::SetStyle(RepeaterStyle style) noexcept { style_ = style; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void RepeaterImpl::Start(float pitch_offset) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_offset_ = pitch_offset;
  performer_->Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void RepeaterImpl::Stop() noexcept {
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

void RepeaterImpl::OnBeat() noexcept {
  if (pitches_.empty() || instrument_ == nullptr) {
    return;
  }
  if (index_ != -1 && pitches_[index_].first.has_value() && remaining_length_ == 1) {
    instrument_->SetNoteOff(*pitches_[index_].first + pitch_offset_);
  }
  if (!Update()) {
    return;
  }
  if (!pitches_[index_].first.has_value()) {
    return;
  }
  static constexpr float kNoteIntensity = 1.0f;
  instrument_->SetNoteOn(*pitches_[index_].first + pitch_offset_, kNoteIntensity);
}

bool RepeaterImpl::Update() noexcept {
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
      index_ = random_.DrawUniform(0, size);
      break;
    default:
      assert(!"Invalid repeater style");
      return false;
  }
  remaining_length_ = pitches_[index_].second;
  return true;
}

}  // namespace barely
