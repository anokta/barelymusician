#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_

#include <map>

#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"

namespace barelyapi {

class InstrumentProcessor {
 public:
  explicit InstrumentProcessor(InstrumentDefinition definition);
  ~InstrumentProcessor();

  // Non-copyable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;

  // Movable.
  InstrumentProcessor(InstrumentProcessor&& other) noexcept;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept;

  void Process(int64 timestamp, float* output, int num_channels,
               int num_frames);

  void SetData(int64 timestamp, InstrumentData data);

 private:
  InstrumentDefinition definition_;
  InstrumentState state_;
  std::multimap<int64, InstrumentData> data_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PROCESSOR_H_
