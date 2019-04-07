#ifndef BARELYMUSICIAN_DSP_GENERATOR_H_
#define BARELYMUSICIAN_DSP_GENERATOR_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Unit generator interface that produces per sample PCM output.
class Generator : public Module {
 public:
  // Generates the next output sample.
  //
  // @return Next output sample.
  virtual float Next() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_GENERATOR_H_
