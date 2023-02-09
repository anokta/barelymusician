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

  /// Returns whether metronome is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Resets metronome.
  void Reset() noexcept;

  /// Sets beat callback.
  ///
  /// @param callback Beat callback.
  void SetBeatCallback(BeatCallback callback) noexcept;

  /// Starts metronome.
  void Start() noexcept;

  /// Stops metronome.
  void Stop() noexcept;

 private:
  friend Metronome Musician::CreateComponent() noexcept;

  // Creates new `Metronome`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Metronome(Musician& musician) noexcept;

  // Metonome performer.
  Performer performer_;

  // Current beat.
  int beat_ = 0;

  // Beat callback.
  BeatCallback callback_ = nullptr;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPONENTS_METRONOME_H_
