#ifndef BARELYMUSICIAN_BASE_SEQUENCER_H_
#define BARELYMUSICIAN_BASE_SEQUENCER_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Step sequencer that keeps track of beats, bars and sections.
class Sequencer : public Module {
 public:
  // Constructs new |Sequencer|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Sequencer(int sample_rate);

  // Implements |Module|.
  void Reset() override;

  // Returns the current bar.
  //
  // @return Current bar.
  int GetCurrentBar() const;

  // Returns the current beat.
  //
  // @return Current beat.
  int GetCurrentBeat() const;

  // Returns the current section.
  //
  // @return Current section.
  int GetCurrentSection() const;

  // Returns the number of samples per beat.
  //
  // @return Number of samples per beat.
  int GetNumSamplesPerBeat() const;

  // Returns the sample offset from the current beat.
  //
  // @return Sample offset.
  int GetSampleOffset() const;

  // Sets the number of bars per each section of the sequencer.
  //
  // @param num_bars Number of bars per section.
  void SetNumBarsPerSection(int num_bars_per_section);

  // Set the number of beats per each bar of the sequencer.
  //
  // @param num_beats Number of beats per bar.
  void SetNumBeatsPerBar(int num_beats_per_bar);

  // Sets the tempo of the sequencer.
  //
  // @param tempo Tempo (BPM).
  void SetTempo(float tempo);

  // Updates the sequencer.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

 private:
  // Sampling rate.
  const float sample_rate_float_;

  // Tempo (BPM).
  float tempo_;

  // Number of bars per section.
  int num_bars_per_section_;

  // Number of beats per bar.
  int num_beats_per_bar_;

  // Number of samples per beat.
  int num_samples_per_beat_;

  // Current state of the sequencer.
  int current_section_;
  int current_bar_;
  int current_beat_;

  // Leftover samples from the current beat.
  int leftover_samples_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_SEQUENCER_H_
