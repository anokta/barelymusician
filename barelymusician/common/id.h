#ifndef BARELYMUSICIAN_COMMON_ID_H_
#define BARELYMUSICIAN_COMMON_ID_H_

#include <cstdint>

namespace barelyapi {

/// Id type.
using Id = std::int64_t;

/// Invalid id.
inline constexpr Id kInvalidId = -1;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMMON_ID_H_
