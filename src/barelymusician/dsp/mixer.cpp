#include "barelymusician/dsp/mixer.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

Mixer::Mixer(int num_channels, int num_frames)
    : output_(num_channels, num_frames) {
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  output_.clear();
}

void Mixer::Reset() { output_.clear(); }

void Mixer::AddInput(const Buffer& input) {
  DCHECK_EQ(output_.num_channels(), input.num_channels());
  DCHECK_EQ(output_.num_frames(), input.num_frames());
  for (int frame = 0; frame < input.num_frames(); ++frame) {
    const auto& input_frame = input[frame];
    auto& output_frame = output_[frame];
    for (int channel = 0; channel < input.num_channels(); ++channel) {
      output_frame[channel] += input_frame[channel];
    }
  }
}

const Buffer& Mixer::GetOutput() const { return output_; }

}  // namespace barelyapi
