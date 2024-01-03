#include "barelymusician/components/arpeggiator.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <optional>

#include "barelymusician/barelymusician.h"

// Arpeggiator.
struct BarelyArpeggiator {
  // Default constructor.
  BarelyArpeggiator(BarelyMusicianHandle musician, int process_order)
      : arpeggiator(barely::Musician::Observer(musician).CreateComponent<barely::Arpeggiator>(
            process_order)) {}

  // Internal arpeggiator.
  barely::Arpeggiator arpeggiator;

  // Optional instrument.
  std::optional<barely::Instrument::Observer> instrument = std::nullopt;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT bool BarelyArpeggiator_Destroy(BarelyArpeggiatorHandle arpeggiator);
  ~BarelyArpeggiator() = default;
};

bool BarelyArpeggiator_Create(BarelyMusicianHandle musician, int32_t process_order,
                              BarelyArpeggiatorHandle* out_arpeggiator) {
  if (!musician || !out_arpeggiator) return false;

  *out_arpeggiator = new BarelyArpeggiator(musician, process_order);
  return true;
}

bool BarelyArpeggiator_Destroy(BarelyArpeggiatorHandle arpeggiator) {
  if (!arpeggiator) return false;

  delete arpeggiator;
  return true;
}

bool BarelyArpeggiator_IsNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch,
                                bool* out_is_note_on) {
  if (!arpeggiator || !out_is_note_on) return false;

  *out_is_note_on = arpeggiator->arpeggiator.IsNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_IsPlaying(BarelyArpeggiatorHandle arpeggiator, bool* out_is_playing) {
  if (!arpeggiator || !out_is_playing) return false;

  *out_is_playing = arpeggiator->arpeggiator.IsPlaying();
  return true;
}

bool BarelyArpeggiator_SetAllNotesOff(BarelyArpeggiatorHandle arpeggiator) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetAllNotesOff();
  return true;
}

bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiatorHandle arpeggiator, double gate_ratio) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetGateRatio(gate_ratio);
  return true;
}

bool BarelyArpeggiator_SetInstrument(BarelyArpeggiatorHandle arpeggiator,
                                     BarelyInstrumentHandle instrument) {
  if (!arpeggiator) return false;

  arpeggiator->instrument.emplace(instrument);
  arpeggiator->arpeggiator.SetInstrument(&arpeggiator->instrument.value());
  return true;
}

bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiatorHandle arpeggiator, double pitch) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetNoteOff(pitch);
  return true;
}

bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_SetRate(BarelyArpeggiatorHandle arpeggiator, double rate) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetRate(rate);
  return true;
}

bool BarelyArpeggiator_SetStyle(BarelyArpeggiatorHandle arpeggiator, BarelyArpeggiatorStyle style) {
  if (!arpeggiator) return false;

  arpeggiator->arpeggiator.SetStyle(static_cast<barely::ArpeggiatorStyle>(style));
  return true;
}

namespace barely {

Arpeggiator::~Arpeggiator() noexcept {
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
}

bool Arpeggiator::IsNoteOn(double pitch) const noexcept {
  return std::find(pitches_.begin(), pitches_.end(), pitch) != pitches_.end();
}

bool Arpeggiator::IsPlaying() const noexcept { return performer_.IsPlaying(); }

void Arpeggiator::SetAllNotesOff() noexcept {
  if (!pitches_.empty()) {
    pitches_.clear();
    Stop();
  }
}

void Arpeggiator::SetGateRatio(double gate_ratio) noexcept {
  gate_ratio_ = std::min(std::max(gate_ratio, 0.0), 1.0);
}

void Arpeggiator::SetInstrument(Instrument* instrument) noexcept {
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
  if (instrument_ != nullptr) {
    instrument_->SetAllNotesOff();
  }
  index_ = -1;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(Musician& musician, int process_order) noexcept
    : performer_(musician.CreatePerformer()) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
  task_ = performer_.CreateTask(
      [this, process_order]() noexcept {
        Update();
        if (instrument_ == nullptr) {
          return;
        }
        const double pitch = pitches_[index_];
        instrument_->SetNoteOn(pitch);
        performer_.ScheduleOneOffTask([this, pitch]() { instrument_->SetNoteOff(pitch); },
                                      gate_ratio_ * performer_.GetLoopLength(), process_order);
      },
      0.0, process_order);
}

}  // namespace barely