#ifndef EXAMPLES_COMPOSERS_DEFAULT_SECTION_COMPOSER_H_
#define EXAMPLES_COMPOSERS_DEFAULT_SECTION_COMPOSER_H_

#include "barelymusician/composition/section_composer.h"

namespace barelyapi {
namespace examples {

class DefaultSectionComposer : public SectionComposer {
 public:
  // Implements |SectionComposer|.
  void Reset() override {}
  int GetSectionType(int section) override { return section; }
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_DEFAULT_SECTION_COMPOSER_H_
