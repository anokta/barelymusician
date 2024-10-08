#include "barelymusician/components/arpeggiator.h"

#include <algorithm>
#include <cstdint>
#include <optional>

#include "barelymusician/barelymusician.h"

// Arpeggiator.
struct BarelyArpeggiator : public barely::Arpeggiator {
  // Constructs `BarelyArpeggiator` with `musician` and `process_order`.
  BarelyArpeggiator(BarelyMusician* musician, int process_order) noexcept
      : barely::Arpeggiator(barely::MusicianPtr(musician), process_order) {}

  // Destroys `BarelyArpeggiator`.
  ~BarelyArpeggiator() noexcept { SetInstrument(std::nullopt); }
};

bool BarelyArpeggiator_Create(BarelyMusician* musician, int32_t process_order,
                              BarelyArpeggiator** out_arpeggiator) {
  if (!musician || !out_arpeggiator) return false;

  *out_arpeggiator = new BarelyArpeggiator(musician, static_cast<int>(process_order));
  return true;
}

bool BarelyArpeggiator_Destroy(BarelyArpeggiator* arpeggiator) {
  if (!arpeggiator) return false;

  delete arpeggiator;
  return true;
}

bool BarelyArpeggiator_IsNoteOn(const BarelyArpeggiator* arpeggiator, double pitch,
                                bool* out_is_note_on) {
  if (!arpeggiator || !out_is_note_on) return false;

  *out_is_note_on = arpeggiator->IsNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_IsPlaying(const BarelyArpeggiator* arpeggiator, bool* out_is_playing) {
  if (!arpeggiator || !out_is_playing) return false;

  *out_is_playing = arpeggiator->IsPlaying();
  return true;
}

bool BarelyArpeggiator_SetAllNotesOff(BarelyArpeggiator* arpeggiator) {
  if (!arpeggiator) return false;

  arpeggiator->SetAllNotesOff();
  return true;
}

bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiator* arpeggiator, double gate_ratio) {
  if (!arpeggiator) return false;

  arpeggiator->SetGateRatio(gate_ratio);
  return true;
}

bool BarelyArpeggiator_SetInstrument(BarelyArpeggiator* arpeggiator, BarelyInstrument* instrument) {
  if (!arpeggiator) return false;

  arpeggiator->SetInstrument(
      instrument != nullptr ? std::optional(barely::InstrumentPtr(instrument)) : std::nullopt);
  return true;
}

bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiator* arpeggiator, double pitch) {
  if (!arpeggiator) return false;

  arpeggiator->SetNoteOff(pitch);
  return true;
}

bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiator* arpeggiator, double pitch) {
  if (!arpeggiator) return false;

  arpeggiator->SetNoteOn(pitch);
  return true;
}

bool BarelyArpeggiator_SetRate(BarelyArpeggiator* arpeggiator, double rate) {
  if (!arpeggiator) return false;

  arpeggiator->SetRate(rate);
  return true;
}

bool BarelyArpeggiator_SetStyle(BarelyArpeggiator* arpeggiator, BarelyArpeggiatorStyle style) {
  if (!arpeggiator) return false;

  arpeggiator->SetStyle(static_cast<barely::ArpeggiatorStyle>(style));
  return true;
}

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
Arpeggiator::Arpeggiator(MusicianPtr musician, int process_order) noexcept
    : performer_(musician, process_order),
      task_(
          performer_,
          [this]() noexcept {
            Update();
            if (!instrument_.has_value()) {
              return;
            }
            const double pitch = pitches_[index_];
            instrument_->SetNoteOn(pitch);
            performer_.ScheduleOneOffTask(
                [this, pitch]() {
                  if (instrument_ != nullptr) {
                    instrument_->SetNoteOff(pitch);
                  }
                },
                gate_ratio_ * performer_.GetLoopLength());
          },
          0.0) {
  performer_.SetLooping(true);
  performer_.SetLoopLength(1.0);
}

Arpeggiator::~Arpeggiator() noexcept { Stop(); }

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

void Arpeggiator::SetInstrument(std::optional<InstrumentPtr> instrument) noexcept {
  if (instrument_.has_value()) {
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
  performer_.CancelAllOneOffTasks();
  performer_.SetPosition(0.0);
  if (instrument_.has_value()) {
    instrument_->SetAllNotesOff();
  }
  index_ = -1;
}

}  // namespace barely
