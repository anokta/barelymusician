#ifndef BARELYMUSICIAN_API_TRIGGER_H_
#define BARELYMUSICIAN_API_TRIGGER_H_

#include <barelymusician.h>

#include "common/callback.h"

/// Implementation of a trigger.
struct BarelyTrigger {
 public:
  /// Process callback alias.
  using ProcessCallback = barely::Callback<BarelyTrigger_ProcessCallback>;

  /// Constructs a new `BarelyTrigger`.
  ///
  /// @param performer Performer.
  /// @param position Trigger position.
  /// @param callback Trigger process callback.
  BarelyTrigger(BarelyPerformer& performer, double position, ProcessCallback callback) noexcept;

  /// Destroys `BarelyTrigger`.
  ~BarelyTrigger() noexcept;

  /// Non-copyable and non-movable.
  BarelyTrigger(const BarelyTrigger& other) noexcept = delete;
  BarelyTrigger& operator=(const BarelyTrigger& other) noexcept = delete;
  BarelyTrigger(BarelyTrigger&& other) noexcept = delete;
  BarelyTrigger& operator=(BarelyTrigger&& other) noexcept = delete;

  /// Returns the position.
  ///
  /// @return Position in beats.
  double GetPosition() const noexcept { return position_; }

  /// Processes the trigger.
  void Process() noexcept { process_callback_(); }

  /// Sets the position.
  ///
  /// @param position Position in beats.
  void SetPosition(double position) noexcept;

  /// Sets the process callback.
  ///
  /// @param callback Trigger process callback.
  void SetProcessCallback(ProcessCallback callback) noexcept { process_callback_ = callback; }

 private:
  // Performer.
  BarelyPerformer& performer_;

  // Position in beats.
  double position_;

  // Process callback.
  ProcessCallback process_callback_;
};

#endif  // BARELYMUSICIAN_API_TRIGGER_H_
