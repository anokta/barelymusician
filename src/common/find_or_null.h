#ifndef BARELYMUSICIAN_COMMON_FIND_OR_NULL_H_
#define BARELYMUSICIAN_COMMON_FIND_OR_NULL_H_

namespace barely {

/// Returns a map value by key.
///
/// @param map Map.
/// @param key Key.
/// @return Const pointer to value if succeeds, nullptr otherwise.
template <typename MapType, typename KeyType = typename MapType::key_type,
          typename ValueType = typename MapType::mapped_type>
constexpr const ValueType* FindOrNull(const MapType& map, const KeyType& key) noexcept {
  if (const auto it = map.find(key); it != map.end()) {
    return &it->second;
  }
  return nullptr;
}

/// Returns a map value by key.
///
/// @param map Map.
/// @param key Key.
/// @return Pointer to value if succeeds, nullptr otherwise.
template <typename MapType, typename KeyType = typename MapType::key_type,
          typename ValueType = typename MapType::mapped_type>
constexpr ValueType* FindOrNull(MapType& map, const KeyType& key) noexcept {
  if (auto it = map.find(key); it != map.end()) {
    return &it->second;
  }
  return nullptr;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_FIND_OR_NULL_H_
