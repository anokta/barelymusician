#ifndef BARELYMUSICIAN_BASE_CLOCK_H_
#define BARELYMUSICIAN_BASE_CLOCK_H_

namespace barelyapi {

// Audio clock that keeps track of beats.
class Clock {
 public:
  // Constructs new |Clock|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Clock(int sample_rate);

  // Returns the current beat.
  //
  // @return Beat.
  int GetBeat() const;

  // Returns the leftover samples from the current beat.
  //
  // @return Leftover samples.
  int GetLeftoverSamples() const;

  // Returns number of samples per beat.
  //
  // @return Number of samples per beat.
  int GetNumSamplesPerBeat() const;

  // Returns the tempo.
  //
  // @return Tempo (BPM).
  float GetTempo() const;

  // Resets the clock.
  void Reset();

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
