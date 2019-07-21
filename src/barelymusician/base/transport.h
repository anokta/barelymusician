#ifndef BARELYMUSICIAN_BASE_TRANSPORT_H_
#define BARELYMUSICIAN_BASE_TRANSPORT_H_

namespace barelyapi {

// Playback transport.
struct Transport {
  // Tempo (BPM).
  float tempo;

  // Number of bars per section.
  int num_bars;

  // Number of beats per bar.
  int num_beats;

  // Number of samples per beat.
  int num_samples;

  // Current section.
  int section;

  // Current bar.
  int bar;

  // Current beat.
  int beat;

  // Leftover samples from the current beat.
  int sample;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_TRANSPORT_H_
