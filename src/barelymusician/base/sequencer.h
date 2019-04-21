#ifndef BARELYMUSICIAN_BASE_SEQUENCER_H_
#define BARELYMUSICIAN_BASE_SEQUENCER_H_

#include "barelymusician/base/event.h"
#include "barelymusician/base/module.h"

namespace barelyapi {

// Step sequencer that keeps track of beats, bars and sections.
class Sequencer : public Module {
 public:
  // Beat event callback signature.
  using BeatCallback = Event<int, int, int, int>::Callback;

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

  // Registers beat callback.
  //
  // @param Beat callback to trigger in each beat.
  void RegisterBeatCallback(BeatCallback&& callback);

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

  // Returns the number of samples per beat.
  //
  // @return Number of samples per beat.
  int num_samples_per_beat() const { return num_samples_per_beat_; }

 private:
  // Calculates number of samples per beat with the current settings.
  void CalculateNumSamplesPerBeat();

  // Event to be triggered for each beat.
  Event<int, int, int, int> beat_event_;

  // Sampling rate.
  const float sample_rate_float_;

  // Number of beats per minute (tempo).
  float bpm_;

  // Beat length relative to quarter note.
  float beat_length_;

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
