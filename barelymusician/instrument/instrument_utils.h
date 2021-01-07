#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_

#include <unordered_map>

namespace barelyapi {

// Returns a map value by key.
//
// @param map Map.
// @param key Key.
// @return Const pointer to value if succeeds, nullptr otherwise.
template <typename KeyType, typename ValueType>
const ValueType* FindOrNull(const std::unordered_map<KeyType, ValueType>& map,
                            const KeyType& key);

// Returns a map value by key.
//
// @param map Map.
// @param key Key.
// @return Pointer to value if succeeds, nullptr otherwise.
template <typename KeyType, typename ValueType>
ValueType* FindOrNull(std::unordered_map<KeyType, ValueType>& map,
                      const KeyType& key);

template <typename KeyType, typename ValueType>
const ValueType* FindOrNull(const std::unordered_map<KeyType, ValueType>& map,
                            const KeyType& key) {
  if (const auto it = map.find(key); it != map.cend()) {
    return &it->second;
  }
  return nullptr;
}

template <typename KeyType, typename ValueType>
ValueType* FindOrNull(std::unordered_map<KeyType, ValueType>& map,
                      const KeyType& key) {
  if (auto it = map.find(key); it != map.end()) {
    return &it->second;
  }
  return nullptr;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
