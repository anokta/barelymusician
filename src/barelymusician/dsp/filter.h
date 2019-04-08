#ifndef BARELYMUSICIAN_DSP_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_PROCESSOR_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Unit filter interface that processes per sample PCM input.
class Filter : public Module {
 public:
  // Filters the next input sample.
  //
  // @param input Input sample.
  // @return Filtered output sample.
  virtual float Next(float input) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_PROCESSOR_H_
