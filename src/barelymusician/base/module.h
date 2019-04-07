#ifndef BARELYMUSICIAN_BASE_MODULE_H_
#define BARELYMUSICIAN_BASE_MODULE_H_

namespace barelyapi {

// Resettable module interface.
class Module {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Module() = default;

  // Resets the module.
  virtual void Reset() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_MODULE_H_
