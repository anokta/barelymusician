#ifndef BARELYMUSICIAN_ENGINE_CONFIG_H_
#define BARELYMUSICIAN_ENGINE_CONFIG_H_

namespace barely::internal {

/// Maximum number of instrument allowed in the engine.
inline constexpr int kMaxInstrumentCount = 128;

/// Maximum number of performers allowed in the engine.
inline constexpr int kMaxPerformerCount = 128;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_CONFIG_H_
