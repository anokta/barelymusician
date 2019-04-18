#ifndef BARELYMUSICIAN_BASE_SEQUENCER_H_
#define BARELYMUSICIAN_BASE_SEQUENCER_H_

#include "barelymusician/base/module.h"

namespace barelyapi {

// Step sequencer that keeps track of beats, bars and sections.
class Sequencer : public Module {
 public:
  // Note value in length.
  enum class NoteValue {
    kWholeNote = 1,      // Whole note (semibreve).
    kHalfNote = 2,       // Half note (minim).
    kQuarterNote = 4,    // Quarter note (crotchet).
    kEighthNote = 8,     // Eighth note (quaver).
    kSixteenthNote = 16  // Sixteenth note (semiquaver).
  };

  // Constructs new |Sequencer|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Sequencer(int sample_rate);

  // Implements |Module|.
  void Reset() override;

  // Sets the tempo of the sequencer.
  //
  // @param bpm Number of beats per minute.
  void SetBpm(float bpm);

  // Sets the number of bars per each section of the sequencer.
  //
  // @param num_bars Number of bars per section.
  void SetNumBarsPerSection(int num_bars_per_section);

  // Set the time signature of the sequencer.
  //
  // @param num_beats Number of beats per bar (i.e., nominator).
  // @ beat_length Beat length (i.e., denominator).
  void SetTimeSignature(int num_beats_per_bar, NoteValue beat_length);

  // Updates the sequencer.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

  // Returns the current bar.
  //
  // @return Current bar.
  int current_bar() const { return current_bar_; }

  // Returns the current beat.
  //
  // @return Current beat.
  int current_beat() const { return current_beat_; }

  // Returns the current section.
  //
  // @return Current section.
  int current_section() const { return current_section_; }

  // Returns the sample offset from the current beat.
  //
  // @return Current sample offset.
  int sample_offset() const { return sample_offset_; }

  // Returns the number of samples per beat.
  //
  // @return Number of samples per beat.
  int num_samples_per_beat() const { return num_samples_per_beat_; }

 private:
  // Calculates number of samples per beat with the current settings.
  void CalculateNumSamplesPerBeat();

  // Sampling rate.
  const float sample_rate_float_;

  // Number of beats per minute (tempo).
  float bpm_;

  // Beat length relative to quarter note.
  float beat_length_;

  // Beats per bar.
  int num_beats_per_bar_;

  // Bars per section.
  int num_bars_per_section_;

  // Current state of the sequencer.
  int current_beat_;
  int current_bar_;
  int current_section_;

  // Sample offset from the current beat.
  int sample_offset_;

  // Number of samples per beat.
  int num_samples_per_beat_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_SEQUENCER_H_
