#ifndef EXAMPLES_PERFORMERS_METRONOME_H_
#define EXAMPLES_PERFORMERS_METRONOME_H_

#include <functional>

#include "barelymusician/barelymusician.h"

namespace barely::examples {

/// Simple metronome that keeps track of beats.
class Metronome {
 public:
  /// Beat callback signature.
  ///
  /// @param beat Beat.
  using BeatCallback = std::function<void(int beat)>;

  /// Constructs a new `Metronome`.
  ///
  /// @param musician Musician handle.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Metronome(MusicianHandle musician, int process_order = -1) noexcept;

  /// Destroys `Metronome`.
  ~Metronome() noexcept;

  /// Non-copyable.
  Metronome(const Metronome& other) noexcept = delete;
  Metronome& operator=(const Metronome& other) noexcept = delete;

  /// Movable.
  Metronome(Metronome&& other) noexcept = default;
  Metronome& operator=(Metronome&& other) noexcept = default;

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
  // Musician.
  MusicianHandle musician_;

  // Performer.
  PerformerHandle performer_;

  // Current beat.
  int beat_ = 0;

  // Beat callback.
  BeatCallback callback_ = nullptr;
};

}  // namespace barely::examples

#endif  // EXAMPLES_PERFORMERS_METRONOME_H_
