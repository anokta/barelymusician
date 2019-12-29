#ifndef BARELYMUSICIAN_BASE_SEQUENCER_H_
#define BARELYMUSICIAN_BASE_SEQUENCER_H_

#include <functional>

namespace barelyapi {

// Step sequencer that keeps track of beats.
class Sequencer {
 public:
  // Beat event callback signature.
  using BeatCallback = std::function<void(int beat, int leftover_samples)>;

  // Constructs new |Sequencer|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Sequencer(int sample_rate);

  // Resets the sequencer.
  void Reset();

  // Sets beat callback.
  //
  // @param beat_callback Beat callback to be triggered in each beat.
  void SetBeatCallback(BeatCallback&& beat_callback);

  // Sets the tempo.
  //
  // @param tempo Tempo (BPM).
  void SetTempo(float tempo);

  // Updates the sequencer.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

 private:
  // Number of samples per minute.
  const float num_samples_per_minute_;

  // Callback to be triggered for each beat.
  BeatCallback beat_callback_;

  // Tempo (BPM).
  float tempo_;

  // Number of samples per beat.
  int num_samples_per_beat_;

  // Current beat.
  int beat_;

  // Leftover samples from the current beat.
  int leftover_samples_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_SEQUENCER_H_
