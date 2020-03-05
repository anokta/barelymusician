#include "barelymusician/engine/clock.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Clock::Clock(int sample_rate)
    : num_samples_per_minute_(static_cast<double>(sample_rate) *
                              kSecondsFromMinutes),
      num_samples_per_beat_(0.0),
      position_(0.0),
      tempo_(0.0) {
  DCHECK_GE(sample_rate, 0);
}

double Clock::GetPosition() const { return position_; }

double Clock::GetTempo() const { return tempo_; }

void Clock::SetPosition(double position) {
  DCHECK_GE(position, 0.0);
  position_ = position;
}

void Clock::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  tempo_ = tempo;
  num_samples_per_beat_ =
      (tempo_ > 0.0) ? num_samples_per_minute_ / tempo_ : 0.0;
}

void Clock::UpdatePosition(int num_samples) {
  DCHECK_GE(num_samples, 0);
  if (num_samples_per_beat_ == 0) {
    return;
  }
  position_ += static_cast<double>(num_samples) / num_samples_per_beat_;
}

}  // namespace barelyapi
