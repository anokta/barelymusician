#include "barelymusician/components/repeater.h"

#include <cstdint>
#include <optional>

#include "barelymusician/barelymusician.h"

// Repeater.
struct BarelyRepeater : public barely::Repeater {
  // Constructs `BarelyRepeater` with `musician` and `process_order`.
  BarelyRepeater(BarelyMusicianHandle musician, int process_order) noexcept
      : barely::Repeater(
            barely::Musician::Observer(musician).CreateComponent<barely::Repeater>(process_order)) {
  }

  // Destroys `BarelyRepeater`.
  ~BarelyRepeater() noexcept { SetInstrument(nullptr); }

  // Optional instrument.
  std::optional<barely::Instrument::Observer> instrument = std::nullopt;
};

bool BarelyRepeater_Clear(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Clear();
  return true;
}

bool BarelyRepeater_Create(BarelyMusicianHandle musician, int32_t process_order,
                           BarelyRepeaterHandle* out_repeater) {
  if (!musician || !out_repeater) return false;

  *out_repeater = new BarelyRepeater(musician, static_cast<int>(process_order));
  return true;
}

bool BarelyRepeater_Destroy(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  delete repeater;
  return true;
}

bool BarelyRepeater_IsPlaying(BarelyRepeaterHandle repeater, bool* out_is_playing) {
  if (!repeater || !out_is_playing) return false;

  *out_is_playing = repeater->IsPlaying();
  return true;
}

bool BarelyRepeater_Pop(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Pop();
  return true;
}

bool BarelyRepeater_Push(BarelyRepeaterHandle repeater, double pitch, int32_t length) {
  if (!repeater) return false;

  repeater->Push(pitch, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_PushSilence(BarelyRepeaterHandle repeater, int32_t length) {
  if (!repeater) return false;

  repeater->Push(std::nullopt, static_cast<int>(length));
  return true;
}

bool BarelyRepeater_SetInstrument(BarelyRepeaterHandle repeater,
                                  BarelyInstrumentHandle instrument) {
  if (!repeater) return false;

  if (repeater->instrument) {
    repeater->SetInstrument(nullptr);
  }
  if (instrument) {
    repeater->instrument.emplace(instrument);
    repeater->SetInstrument(&repeater->instrument.value());
  } else {
    repeater->instrument.reset();
  }
  return true;
}

bool BarelyRepeater_SetRate(BarelyRepeaterHandle repeater, double rate) {
  if (!repeater) return false;

  repeater->SetRate(rate);
  return true;
}

bool BarelyRepeater_SetStyle(BarelyRepeaterHandle repeater, BarelyRepeaterStyle style) {
  if (!repeater) return false;

  repeater->SetStyle(static_cast<barely::RepeaterStyle>(style));
  return true;
}

bool BarelyRepeater_Start(BarelyRepeaterHandle repeater, double pitch_shift) {
  if (!repeater) return false;

  repeater->Start(pitch_shift);
  return true;
}

bool BarelyRepeater_Stop(BarelyRepeaterHandle repeater) {
  if (!repeater) return false;

  repeater->Stop();
  return true;
}

namespace barely {

Repeater::~Repeater() noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
}

void Repeater::Clear() noexcept { pitches_.clear(); }

bool Repeater::IsPlaying() const noexcept { return performer_.IsPlaying(); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Pop() noexcept {
  if (pitches_.empty()) {
    return;
  }
  if (index_ == static_cast<int>(pitches_.size()) - 1 && IsPlaying()) {
    if (instrument_ != nullptr) {
      instrument_->SetNoteOff(*pitches_.back().first + pitch_shift_);
    }
    remaining_length_ = 0;
  }
  pitches_.pop_back();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Push(std::optional<double> pitch_or, int length) noexcept {
  pitches_.emplace_back(pitch_or, length);
}

void Repeater::SetInstrument(Instrument* instrument) noexcept {
  if (instrument_ != nullptr) {
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
void Repeater::Start(double pitch_shift) noexcept {
  if (IsPlaying()) {
    return;
  }
  pitch_shift_ = pitch_shift;
  performer_.Start();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Repeater::Stop() noexcept {
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

// NOLINTNEXTLINE(bugprone-exception-escape)
Repeater::Repeater(Musician& musician, int process_order) noexcept
    : performer_(musician.CreatePerformer()) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  task_ = performer_.CreateTask(
      [this, process_order]() noexcept {
        if (pitches_.empty() || !Update() || instrument_ == nullptr) {
          return;
        }
        const auto& [pitch_or, length] = pitches_[index_];
        if (!pitches_[index_].first.has_value()) {
          return;
        }
        const double pitch = *pitches_[index_].first + pitch_shift_;
        instrument_->SetNoteOn(pitch);
        performer_.ScheduleOneOffTask([this, pitch]() { instrument_->SetNoteOff(pitch); },
                                      static_cast<double>(length) * performer_.GetLoopLength(),
                                      process_order);
      },
      0.0, process_order);
}

}  // namespace barely
