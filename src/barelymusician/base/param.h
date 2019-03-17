#ifndef BARELYMUSICIAN_BASE_PARAM_H_
#define BARELYMUSICIAN_BASE_PARAM_H_

#include "barelymusician/base/types.h"

namespace barelyapi {

// Basic instrument parameter structure.
template <typename ParamType>
struct Param {
  // Parameter ID.
  ParamId id;

  // Parameter value.
  ParamType value;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_PARAM_H_
