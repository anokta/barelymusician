#ifndef BARELYMUSICIAN_COMPOSITION_SECTION_COMPOSER_H_
#define BARELYMUSICIAN_COMPOSITION_SECTION_COMPOSER_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Section composer interface to generate sections of a score.
class SectionComposer : public Module {
 public:
  // Returns section type for the given |section| index.
  //
  // @param section Section index.
  // @return Section type.
  virtual int GetSectionType(int section) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SECTION_COMPOSER_H_
