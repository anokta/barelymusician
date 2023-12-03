#ifndef BARELYMUSICIAN_COMPONENTS_METRONOME_H_
#define BARELYMUSICIAN_COMPONENTS_METRONOME_H_

#include <functional>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Simple metronome that keeps track of beats.
class Metronome {
 public:
  /// Beat callback signature.
  ///
  /// @param beat Beat.
  using BeatCallback = std::function<void(int beat)>;

  /// Returns whether the metronome is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Resets the metronome.
  void Reset() noexcept;

  /// Sets the beat callback.
  ///
  /// @param callback Beat callback.
  void SetBeatCallback(BeatCallback callback) noexcept;

  /// Starts the metronome.
  void Start() noexcept;

  /// Stops the metronome.
  void Stop() noexcept;

 private:
  // Ensures that `Metronome` can only be created by `Musician`.
  friend class Musician;

  // Creates a new `Metronome` with a given `musician` and `process_order`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Metronome(Musician& musician, int process_order = -1) noexcept;

  // Performer.
  Performer performer_;

  // Task.
  Task task_;

  // Current beat.
  int beat_ = 0;

  // Beat callback.
  BeatCallback callback_ = nullptr;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPONENTS_METRONOME_H_
