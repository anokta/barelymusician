#ifndef BARELYMUSICIAN_API_DATA_H_
#define BARELYMUSICIAN_API_DATA_H_

// NOLINTBEGIN
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Data definition move callback signature.
///
/// @param other_data Other data to move.
/// @param out_data Output data.
typedef void (*BarelyDataDefinition_MoveCallback)(void* other_data,
                                                  void** out_data);

/// Data definition destroy callback signature.
///
/// @param data Data to destroy.
typedef void (*BarelyDataDefinition_DestroyCallback)(void* data);

/// Data definition.
typedef struct BarelyDataDefinition {
  /// Move callback.
  BarelyDataDefinition_MoveCallback move_callback;

  /// Destroy callback.
  BarelyDataDefinition_DestroyCallback destroy_callback;

  /// Data.
  void* data;
} BarelyDataDefinition;

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
// NOLINTEND

#ifdef __cplusplus
#include <utility>

namespace barely {

/// Returns data definition.
///
/// @param typed_data Typed data.
/// @return Data definition.
template <typename DataType>
BarelyDataDefinition GetDataDefinition(DataType typed_data) {
  return {[](void* other_data, void** out_data) {
            *out_data = reinterpret_cast<void*>(new DataType(
                std::move(*reinterpret_cast<DataType*>(other_data))));
          },
          [](void* data) { delete reinterpret_cast<DataType*>(data); },
          reinterpret_cast<void*>(&typed_data)};
}

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_API_DATA_H_
