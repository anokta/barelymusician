#ifndef BARELYMUSICIAN_API_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_API_BARELYMUSICIAN_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/composition/performer.h"

namespace barelyapi {

// BarelyMusician API that maintains a native real-time music engine.
class BarelyMusician {
 public:
  // Sequencer beat event callback signature.
  using BeatCallback = Sequencer::BeatCallback;

  // Instrument note off event callback signature.
  using NoteOffCallback = Performer::NoteOffCallback;

  // Instrument note on event callback signature.
  using NoteOnCallback = Performer::NoteOnCallback;

  // Constructs new |BarelyMusician| with the given system configuration.
  //
  // @param sample_rate Sampling rate.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  BarelyMusician(int sample_rate, int num_channels, int num_frames);

  // Creates new instrument of |InstrumentType|, and returns its ID.
  //
  // @return Instrument ID.
  template <typename InstrumentType, typename... ArgumentTypes>
  int CreateInstrument(ArgumentTypes... arguments);

  // Destroys instrument.
  //
  // @param instrument_id Instrument ID.
  void DestroyInstrument(int instrument_id);

  // Clears all instrument notes.
  //
  // @param instrument_id Instrument ID.
  void ClearAllInstrumentNotes(int instrument_id);

  // Processes instrument.
  //
  // @param instrument_id Instrument ID.
  // @param output Output buffer.
  void ProcessInstrument(int instrument_id, float* output);

  // Registers new instrument note off callback.
  //
  // @param instrument_id Instrument ID.
  // @param note_off_callback Instrument note off callback.
  void RegisterInstrumentNoteOffCallback(int instrument_id,
                                         NoteOffCallback&& note_off_callback);

  // Registers new instrument note on callback.
  //
  // @param instrument_id Instrument ID.
  // @param note_on_callback Instrument note on callback.
  void RegisterInstrumentNoteOnCallback(int instrument_id,
                                        NoteOnCallback&& note_on_callback);

  // Starts playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  // @param intensity Note intensity.
  // @param offset_samples Relative sample offset to start the note.
  void StartInstrumentNote(int instrument_id, float index, float intensity,
                           int offset_samples);

  // Stops playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  // @param offset_samples Relative sample offset to stop the note.
  void StopInstrumentNote(int instrument_id, float index, int offset_samples);

  // Creates new sequencer, and returns its ID.
  //
  // @return Sequencer ID.
  int CreateSequencer();

  // Destroys sequencer.
  //
  // @param sequencer_id Sequencer ID.
  void DestroySequencer(int sequencer_id);

  // Registers new sequencer beat callback.
  //
  // @param sequencer_id Sequencer ID.
  // @param beat_callback Sequencer beat callback.
  void RegisterSequencerBeatCallback(int sequencer_id,
                                     BeatCallback&& beat_callback);

  // Sets sequencer number of bars per section.
  //
  // @param sequencer_id Sequencer ID.
  // @param num_beats Number of bars per section.
  void SetSequencerNumBars(int sequencer_id, int num_bars);

  // Sets sequencer number of beats per bar.
  //
  // @param sequencer_id Sequencer ID.
  // @param num_beats Number of beats per bar.
  void SetSequencerNumBeats(int sequencer_id, int num_beats);

  // Sets sequencer position.
  //
  // @param sequencer_id Sequencer ID.
  // @param section Section.
  // @param bar Bar.
  // @param beat Beat.
  void SetSequencerPosition(int sequencer_id, int section, int bar, int beat);

  // Sets sequencer tempo.
  //
  // @param sequencer_id Sequencer ID.
  // @param tempo Sequencer tempo.
  void SetSequencerTempo(int sequencer_id, float tempo);

  // Starts the playback.
  //
  // @param sequencer_id Sequencer ID.
  void StartSequencer(int sequencer_id);

  // Stops the playback.
  //
  // @param sequencer_id Sequencer ID.
  void StopSequencer(int sequencer_id);

  // Updates the internal state.
  void Update();

 private:
  // Returns performer with the given |instrument_id|.
  Performer* GetPerformer(int instrument_id);

  // Returns instrument with the given |sequencer_id|.
  Sequencer* GetSequencer(int sequencer_id);

  // Sampling rate.
  const int sample_rate_;

  // Number of output channels.
  const int num_channels_;

  // Number of output frames.
  const int num_frames_;

  // Global counter to generate unique IDs.
  int id_counter_;

  // Task runner to ensure thread-safety between main and audio threads.
  TaskRunner task_runner_;

  // Instrument performers.
  std::unordered_map<int, Performer> performers_;

  // Sequencers.
  std::unordered_map<int, Sequencer> sequencers_;
};

template <typename InstrumentType, typename... ArgumentTypes>
int BarelyMusician::CreateInstrument(ArgumentTypes... arguments) {
  const int instrument_id = ++id_counter_;
  task_runner_.Add([this, arguments..., instrument_id]() {
    performers_.insert(std::make_pair(
        instrument_id,
        Performer(std::make_unique<InstrumentType>(arguments...))));
  });
  return instrument_id;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_API_BARELYMUSICIAN_H_
