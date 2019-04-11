#ifndef BARELYMUSICIAN_COMPOSITION_BAR_COMPOSER_H_
#define BARELYMUSICIAN_COMPOSITION_BAR_COMPOSER_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Bar composer interface to generate harmonic progression of a bar.
class BarComposer : public Module {
 public:
  // Returns harmonic progression for the given |bar| index.
  //
  // @param section_type Section type.
  // @param bar Bar Index.
  // @param num_bars Number of bars in section.
  // @return Relative harmonic progression index.
  virtual int GetHarmonic(int section_type, int bar, int num_bars) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_BAR_COMPOSER_H_
