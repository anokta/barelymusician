#ifndef BARELYMUSICIAN_DSP_MIXER_H_
#define BARELYMUSICIAN_DSP_MIXER_H_

#include "barelymusician/base/buffer.h"
#include "barelymusician/base/module.h"

namespace barelyapi {

// Mixer that accumulates input buffers into an output buffer.
class Mixer : public Module {
 public:
  // Constructs new |Mixer| with the given |num_channels| and |num_frames|.
  //
  // @param num_channels Number of channels.
  // @param num_frames Number of frames.
  Mixer(int num_channels, int num_frames);

  // Implements |Module|.
  void Reset();

  // Adds the given |input| buffer to the mixer.
  //
  // @param input Input buffer.
  void AddInput(const Buffer& input);

  // Returns the mixed output buffer.
  //
  // @return Output buffer.
  const Buffer& GetOutput() const;

 private:
  // Output buffer.
  Buffer output_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_MIXER_H_
