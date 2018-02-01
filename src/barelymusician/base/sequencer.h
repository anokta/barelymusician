#ifndef BARELYMUSICIAN_BASE_SEQUENCER_H_
#define BARELYMUSICIAN_BASE_SEQUENCER_H_

namespace barelyapi {

// Step sequencer that keeps track of beats, bars and sections.
class Sequencer {
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

  // Updates the sequencer.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

  // Resets the sequencer.
  void Reset();

  // Returns the current beat.
  //
  // @return Current beat.
  int GetCurrentBeat() const;

  // Returns the current bar.
  //
  // @return Current bar.
  int GetCurrentBar() const;

  // Returns the current section.
  //
  // @return Current section.
  int GetCurrentSection() const;

  // Returns the sample offset from the current beat.
  //
  // @return Current sample offset.
  int GetCurrentSampleOffset() const;

  // Sets the tempo of the sequencer.
  //
  // @param bpm Number of beats per minute.
  void SetBpm(float bpm);

  // Sets the beat length (denominator of the time signature).
  //
  // @ beat_length Beat length.
  void SetBeatLength(NoteValue beat_length);

  // Set number of beats per bar (nominator of the time signature).
  //
  // @param num_beats Number of beats per bar.
  void SetNumBeatsPerBar(int num_beats_per_bar);

  // Set number of bars per section.
  //
  // @param num_bars Number of bars per section.
  void SetNumBarsPerSection(int num_bars_per_section);

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
