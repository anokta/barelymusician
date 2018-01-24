#ifndef BARELYMUSICIAN_INSTRUMENT_UNIT_GENERATOR_H_
#define BARELYMUSICIAN_INSTRUMENT_UNIT_GENERATOR_H_

namespace barelyapi {

// Unit generator interface that produces per sample PCM output.
class UnitGenerator {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~UnitGenerator() = default;

  // Generates the next output sample.
  //
  // @return Next output sample.
  virtual float Next() = 0;

  // Resets the generator.
  virtual void Reset() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_UNIT_GENERATOR_H_
