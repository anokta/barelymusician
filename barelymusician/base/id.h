#ifndef BARELYMUSICIAN_BASE_ID_H_
#define BARELYMUSICIAN_BASE_ID_H_

namespace barelyapi {

/// Generates next id.
///
/// @return Unique id.
int GetNextId();

/// Resets id counter state.
void ResetIdCount();

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_ID_H_
