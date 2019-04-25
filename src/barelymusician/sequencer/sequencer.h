#ifndef BARELYMUSICIAN_SEQUENCER_SEQUENCER_H_
#define BARELYMUSICIAN_SEQUENCER_SEQUENCER_H_

#include "barelymusician/base/module.h"
#include "barelymusician/sequencer/event.h"
#include "barelymusician/sequencer/transport.h"

namespace barelyapi {

// Step sequencer that keeps track of beats, bars and sections.
class Sequencer : public Module {
 public:
  // Beat event callback signature.
  using BeatCallback = Event<const Transport&, int>::Callback;

  // Constructs new |Sequencer|.
  //
  // @param sample_rate Sampling rate per second.
  explicit Sequencer(int sample_rate);

  // Implements |Module|.
  void Reset() override;

  // Returns the playback transport.
  //
  // @return Playback transport.
  const Transport& GetTransport() const;

  // Registers beat callback.
  //
  // @param Beat callback to trigger for each beat.
  void RegisterBeatCallback(BeatCallback&& callback);

  // Sets the number of bars per each section.
  //
  // @param num_bars Number of bars per section.
  void SetNumBars(int num_bars);

  // Set the number of beats per each bar.
  //
  // @param num_beats Number of beats per bar.
  void SetNumBeats(int num_beats);

  // Sets the playback position.
  //
  // @param section Section.
  // @param bar Bar.
  // @param beat Beat.
  void SetPosition(int section, int bar, int beat);

  // Sets the tempo.
  //
  // @param tempo Tempo (BPM).
  void SetTempo(float tempo);

  // Updates the sequencer.
  //
  // @num_samples Number of samples to iterate.
  void Update(int num_samples);

 private:
  // Number of samples per minute.
  const float num_samples_per_minute_;  
  
  // Event to be triggered for each beat.
  Event<const Transport&, int> beat_event_;

  // Number of samples per beat.
  int num_samples_per_beat_;

  // Offset samples from the current beat.
  int offset_samples_;

  // Playback transport.
  Transport transport_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_SEQUENCER_SEQUENCER_H_
