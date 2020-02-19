#ifndef BARELYMUSICIAN_ENGINE_CLOCK_H_
#define BARELYMUSICIAN_ENGINE_CLOCK_H_

namespace barelyapi {

// Audio clock that keeps track of position in beats.
class Clock {
 public:
  // Constructs new |Clock|.
  //
  // @param sample_rate Sampling rate in Hz.
  explicit Clock(int sample_rate);

  // Returns the current position.
  //
  // @return Position in beats.
  double GetPosition() const;

  // Returns the tempo.
  //
  // @return Tempo in BPM.
  double GetTempo() const;

  // Sets the current position.
  //
  // @param position Position in beats.
  void SetPosition(double position);

  // Sets the tempo.
  //
  // @param tempo Tempo in BPM.
  void SetTempo(double tempo);

  // Updates the current position.
  //
  // @num_samples Number of samples to iterate.
  void UpdatePosition(int num_samples);

 private:
  // Returns the corresponding number of beats for the given |samples|.
  double BeatsFromSamples(int samples) const;

  // Returns the corresponding number of samples for the given |beats|.
  int SamplesFromBeats(double beats) const;

  // Number of samples per minute.
  const double num_samples_per_minute_;

  // Current beat.
  int beat_;

  // Leftover samples from the current beat.
  int leftover_samples_;

  // Number of samples per beat.
  int num_samples_per_beat_;

  // Position in beats.
  double position_;

  // Tempo in BPM.
  double tempo_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CLOCK_H_
