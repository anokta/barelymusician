#ifndef BARELYMUSICIAN_INTERNAL_ID_H_
#define BARELYMUSICIAN_INTERNAL_ID_H_

#include <cstdint>

namespace barely::internal {

/// Identifier alias.
using Id = std::int64_t;

/// Invalid identifier.
inline constexpr Id kInvalid = 0;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_ID_H_
