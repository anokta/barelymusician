#ifndef BARELYMUSICIAN_INSTRUMENT_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_INSTRUMENT_SAMPLE_PLAYER_H_

#include "barelymusician/instrument/unit_generator.h"

namespace barelyapi {

// Sample player that generates output samples from the provided mono data.
class SamplePlayer : public UnitGenerator {
 public:
  // Construct new |SamplePlayer| with the given sample data.
  //
  // @param sample_interval Target sampling interval in seconds.
  // @param frequency Original sampling frequency in Hz.
  // @param data Sample data.
  // @param length Sample data length.
  SamplePlayer(float sample_interval, int frequency, const float* data,
               int length);

  // Implements |UnitGenerator|.
  float Next() override;
  void Reset() override;

  // Sets whether the playback should be looping.
  //
  // @param loop True if looping.
  void SetLoop(bool loop);

  // Sets the playback speed.
  //
  // @param Playback speed.
  void SetSpeed(float speed);

 private:
  // Sampling ratio for the playback.
  const float sample_ratio_;

  // Sample data.
  const float* data_;

  // Sample data length.
  const float length_;

  // Denotes whether the playback is looping.
  bool loop_;

  // Playback speed.
  float speed_;

  // Playback cursor.
  float cursor_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_SAMPLE_PLAYER_H_
