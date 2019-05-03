#ifndef EXAMPLES_COMPOSERS_DEFAULT_BAR_COMPOSER_H_
#define EXAMPLES_COMPOSERS_DEFAULT_BAR_COMPOSER_H_

#include "barelymusician/composition/bar_composer.h"

namespace barelyapi {
namespace examples {

class DefaultBarComposer : public BarComposer {
 public:
  // Implements |BarComposer|.
  void Reset() override {}
  int GetHarmonic(int section_type, int bar, int num_bars) override {
    return section_type * num_bars + bar;
  }
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_DEFAULT_SECTION_COMPOSER_H_
