#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_

namespace barelyapi {

/// Instrument parameter.
struct InstrumentParam {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const InstrumentParam&) const = default;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PARAM_H_
