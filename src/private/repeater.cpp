#include "private/repeater.h"

#include <barelymusician.h>

#include <cassert>
#include <optional>
#include <utility>

#include "private/engine.h"
#include "private/instrument.h"

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyRepeater::BarelyRepeater(BarelyEngine& engine) noexcept
    : engine_(&engine), performer_(engine) {
  performer_.SetBeatCallback({
      [](void* user_data) noexcept {
        auto& repeater = *static_cast<BarelyRepeater*>(user_data);
        repeater.OnBeat();
      },
      this,
  });
}

BarelyRepeater::~BarelyRepeater() noexcept {
  if (IsPlaying() && instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
}

void BarelyRepeater::Clear() noexcept { pitches_.clear(); }

bool BarelyRepeater::IsPlaying() const noexcept { return performer_.IsPlaying(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyRepeater::Pop() noexcept {
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
void BarelyRepeater::Push(std::optional<float> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

void BarelyRepeater::SetInstrument(BarelyInstrument* instrument) noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

void BarelyRepeater::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(length);
}

void BarelyRepeater::SetStyle(BarelyRepeaterStyle style) noexcept { style_ = style; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyRepeater::Start(float pitch_offset) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_offset_ = pitch_offset;
  performer_.Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyRepeater::Stop() noexcept {
  if (!IsPlaying()) {
    return;
  }
  performer_.Stop();
  performer_.SetPosition(0.0);
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  index_ = -1;
  remaining_length_ = 0;
}

void BarelyRepeater::OnBeat() noexcept {
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
  instrument_->SetNoteOn(*pitches_[index_].first + pitch_offset_, {});
}

bool BarelyRepeater::Update() noexcept {
  if (--remaining_length_ > 0 || pitches_.empty()) {
    return false;
  }
  const int size = static_cast<int>(pitches_.size());
  switch (style_) {
    case BarelyRepeaterStyle_kForward:
      index_ = (index_ + 1) % size;
      break;
    case BarelyRepeaterStyle_kBackward:
      index_ = (index_ == -1) ? size - 1 : (index_ + size - 1) % size;
      break;
    case BarelyRepeaterStyle_kRandom:
      index_ = engine_->main_rng().Generate(0, size);
      break;
    default:
      assert(!"Invalid repeater style");
      return false;
  }
  remaining_length_ = pitches_[index_].second;
  return true;
}
