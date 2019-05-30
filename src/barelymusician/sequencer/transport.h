#ifndef BARELYMUSICIAN_SEQUENCER_TRANSPORT_H_
#define BARELYMUSICIAN_SEQUENCER_TRANSPORT_H_

namespace barelyapi {

// Playback transport.
struct Transport {
  // Tempo (BPM).
  float tempo;

  // Number of bars per section.
  int num_bars;

  // Number of beats per bar.
  int num_beats;

  // Current section.
  int section;

  // Current bar.
  int bar;

  // Current beat.
  int beat;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_SEQUENCER_TRANSPORT_H_
