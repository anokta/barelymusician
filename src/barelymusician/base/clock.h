#ifndef BARELYMUSICIAN_BASE_CLOCK_H_
#define BARELYMUSICIAN_BASE_CLOCK_H_

#include <functional>

namespace barelyapi {

// Audio clock that keeps track of beats.
class Clock {
 public:
  // Beat event callback signature.
  using BeatCallback = std::function<void(int beat, int leftover_samples)>;

  // Constructs new |Clock|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Clock(int sample_rate);

  // Returns number of samples per beat.
  //
  // @return Number of samples per beat.
  int GetNumSamplesPerBeat() const;

  // Returns the current clock position in beats.
  //
  // @return Absolute position in beats.
  float GetPosition() const;

  // Returns the tempo.
  //
  // @return Tempo (BPM).
  float GetTempo() const;

  // Sets the beat callback.
  //
  // @param beat_callback Beat callback to be triggered in each beat.
  void SetBeatCallback(BeatCallback&& beat_callback);

  // Sets the current clock position in beats.
  //
  // @param beat Absolute position in beats.
  void SetPosition(float beat);

  // Sets the tempo.
  //
  // @param tempo Tempo (BPM).
  void SetTempo(float tempo);

  // Updates the clock.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

 private:
  // Number of samples per minute.
  const float num_samples_per_minute_;

  // Callback to be triggered for each beat.
  BeatCallback beat_callback_;

  // Current beat.
  int beat_;

  // Leftover samples from the current beat.
  int leftover_samples_;

  // Number of samples per beat.
  int num_samples_per_beat_;

  // Tempo (BPM).
  float tempo_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CLOCK_H_
