#ifndef BARELYMUSICIAN_BASE_ID_GENERATOR_H_
#define BARELYMUSICIAN_BASE_ID_GENERATOR_H_

namespace barelyapi {

// Unique numerical id generator.
class IdGenerator {
 public:
  /// Constructs new |IdGenerator|.
  IdGenerator();

  /// Generates next id.
  ///
  /// @return Unique id.
  int Next();

 private:
  // Id counter.
  int counter_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_ID_GENERATOR_H_
