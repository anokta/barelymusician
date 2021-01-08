#ifndef BARELYMUSICIAN_BASE_ID_H_
#define BARELYMUSICIAN_BASE_ID_H_

#include <cstdint>

namespace barelyapi {

// Generates next id.
//
// @return Unique id.
std::int64_t GetNextId();

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_ID_H_
