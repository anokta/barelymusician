#include "barelymusician/components/repeater.h"

#include <cstdint>
#include <optional>

#include "barelymusician/barelymusician.h"

// Repeater.
struct BarelyRepeater : public barely::Repeater {
  // Constructs `BarelyRepeater` with `musician` and `process_order`.
  BarelyRepeater(BarelyMusician* musician, int process_order) noexcept
      : barely::Repeater(barely::MusicianPtr(musician), process_order) {}

  // Destroys `BarelyRepeater`.
  ~BarelyRepeater() noexcept { SetInstrument(std::nullopt); }
};

bool BarelyRepeater_Clear(BarelyRepeater* repeater) {
  if (!repeater) return false;

  repeater->Clear();
  return true;
}

bool BarelyRepeater_Create(BarelyMusician* musician, int32_t process_order,
                           BarelyRepeater** out_repeater) {
  if (!musician || !out_repeater) return false;

  *out_repeater = new BarelyRepeater(musician, static_cast<int>(process_order));
  return true;
}

bool BarelyRepeater_Destroy(BarelyRepeater* repeater) {
  if (!repeater) return false;

  delete repeater;
  return true;
}

bool BarelyRepeater_IsPlaying(const BarelyRepeater* repeater, bool* out_is_playing) {
  if (!repeater || !out_is_playing) return false;

  *out_is_playing = repeater->IsPlaying();
  return true;
}

bool BarelyRepeater_Pop(BarelyRepeater* repeater) {
  if (!repeater) return false;

  repeater->Pop();
  return true;
}

bool BarelyRepeater_Push(BarelyRepeater* repeater, double note, int32_t length) {
  if (!repeater) return false;

  repeater->Push(note, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_PushSilence(BarelyRepeater* repeater, int32_t length) {
  if (!repeater) return false;

  repeater->Push(std::nullopt, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_SetInstrument(BarelyRepeater* repeater, BarelyInstrument* instrument) {
  if (!repeater) return false;

  repeater->SetInstrument(instrument != nullptr ? std::optional(barely::InstrumentPtr(instrument))
                                                : std::nullopt);
  return true;
}

bool BarelyRepeater_SetRate(BarelyRepeater* repeater, double rate) {
  if (!repeater) return false;

  repeater->SetRate(rate);
  return true;
}

bool BarelyRepeater_SetStyle(BarelyRepeater* repeater, BarelyRepeaterStyle style) {
  if (!repeater) return false;

  repeater->SetStyle(static_cast<barely::RepeaterStyle>(style));
  return true;
}

bool BarelyRepeater_Start(BarelyRepeater* repeater, double note_multiplier) {
  if (!repeater) return false;

  repeater->Start(note_multiplier);
  return true;
}

bool BarelyRepeater_Stop(BarelyRepeater* repeater) {
  if (!repeater) return false;

  repeater->Stop();
  return true;
}

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Repeater::Repeater(MusicianPtr musician, int process_order) noexcept
    : performer_(musician, process_order),
      task_(
          performer_,
          [this]() noexcept {
            if (notes_.empty() || !Update() || !instrument_.has_value()) {
              return;
            }
            const auto& [pitch_or, length] = notes_[index_];
            if (!notes_[index_].first.has_value()) {
              return;
            }
            const double note = *notes_[index_].first * note_multiplier_;
            instrument_->SetNoteOn(note);
            performer_.ScheduleOneOffTask([this, note]() { instrument_->SetNoteOff(note); },
                                          static_cast<double>(length) * performer_.GetLoopLength());
          },
          0.0) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
}

Repeater::~Repeater() noexcept { Stop(); }

void Repeater::Clear() noexcept { notes_.clear(); }

bool Repeater::IsPlaying() const noexcept { return performer_.IsPlaying(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Pop() noexcept {
  if (notes_.empty()) {
    return;
  }
  if (index_ == static_cast<int>(notes_.size()) - 1 && IsPlaying()) {
    if (instrument_.has_value()) {
      instrument_->SetNoteOff(*notes_.back().first * note_multiplier_);
    }
    remaining_length_ = 0;
  }
  notes_.pop_back();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Push(std::optional<double> pitch_or, int length) noexcept {
  notes_.emplace_back(pitch_or, length);
}

void Repeater::SetInstrument(std::optional<InstrumentPtr> instrument) noexcept {
  if (instrument_.has_value() && IsPlaying()) {
    instrument_->SetAllNotesOff();
  }
  instrument_ = instrument;
}

void Repeater::SetRate(double rate) noexcept {
  const double length = (rate > 0.0) ? 1.0 / rate : 0.0;
  performer_.SetLoopLength(length);
}

void Repeater::SetStyle(RepeaterStyle style) noexcept { style_ = style; }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Start(double note_multiplier) noexcept {
  if (IsPlaying()) {
    return;
  }
  note_multiplier_ = note_multiplier;
  performer_.Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Stop() noexcept {
  if (!IsPlaying()) {
    return;
  }
  performer_.Stop();
  performer_.SetPosition(0.0);
  if (instrument_.has_value()) {
    instrument_->SetAllNotesOff();
  }
  index_ = -1;
  remaining_length_ = 0;
}

bool Repeater::Update() noexcept {
  if (--remaining_length_ > 0 || notes_.empty()) {
    return false;
  }
  const int size = static_cast<int>(notes_.size());
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
  remaining_length_ = notes_[index_].second;
  return true;
}

}  // namespace barely
