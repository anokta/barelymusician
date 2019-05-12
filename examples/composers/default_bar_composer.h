#ifndef EXAMPLES_COMPOSERS_DEFAULT_BAR_COMPOSER_H_
#define EXAMPLES_COMPOSERS_DEFAULT_BAR_COMPOSER_H_

#include <vector>

#include "barelymusician/composition/bar_composer.h"

namespace barelyapi {
namespace examples {

class DefaultBarComposer : public BarComposer {
 public:
  // Implements |BarComposer|.
  void Reset() override {}
  int GetHarmonic(int section_type, int bar, int num_bars) override {
    const std::vector<int> kProgression = {0, 3, 4, 0};
    return kProgression[(section_type * num_bars + bar) % kProgression.size()];
  }
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_DEFAULT_SECTION_COMPOSER_H_
