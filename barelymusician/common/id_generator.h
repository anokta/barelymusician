#ifndef BARELYMUSICIAN_COMMON_ID_GENERATOR_H_
#define BARELYMUSICIAN_COMMON_ID_GENERATOR_H_

#include "barelymusician/common/id.h"

namespace barely {

// Unique numerical id generator.
class IdGenerator {
 public:
  /// Constructs new |IdGenerator|.
  IdGenerator();

  /// Generates next id.
  ///
  /// @return Unique id.
  Id Generate();

 private:
  // Id counter.
  Id counter_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_ID_GENERATOR_H_
