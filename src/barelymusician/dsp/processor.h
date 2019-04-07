#ifndef BARELYMUSICIAN_DSP_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_PROCESSOR_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Unit processor interface that filters per sample PCM input.
class Processor : public Module {
 public:
  // Processes the next input sample.
  //
  // @param input Input sample.
  // @return Processed output sample.
  virtual float Next(float input) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_PROCESSOR_H_
