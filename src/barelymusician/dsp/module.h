#ifndef BARELYMUSICIAN_DSP_MODULE_H_
#define BARELYMUSICIAN_DSP_MODULE_H_

namespace barelyapi {

// Resettable DSP module interface.
class Module {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Module() = default;

  // Resets module state.
  virtual void Reset() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_MODULE_H_
