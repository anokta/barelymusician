#ifndef BARELYMUSICIAN_API_BARELYMUSICIAN_H_
#define BARELYMUSICIAN_API_BARELYMUSICIAN_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

// BarelyMusician API that maintains a native real-time music engine.
class BarelyMusician {
 public:
  // Constructs new |BarelyMusician| with the given system configuration.
  //
  // @param sample_rate Sampling rate.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  BarelyMusician(int sample_rate, int num_channels, int num_frames);

  // Updates the internal state.
  void Update();

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
                                     Sequencer::BeatCallback&& beat_callback);

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

  // Creates new instrument of |InstrumentType|, and returns its ID.
  //
  // @return Instrument ID.
  template <typename InstrumentType, typename... ArgumentTypes>
  int CreateInstrument(ArgumentTypes... arguments);

  // Destroys instrument.
  //
  // @param instrument_id Instrument ID.
  void DestroyInstrument(int instrument_id);

  // Processes instrument.
  //
  // @param instrument_id Instrument ID.
  // @param output Output buffer.
  void ProcessInstrument(int instrument_id, float* output);

  // Clears instrument.
  //
  // @param instrument_id Instrument ID.
  void SetInstrumentClear(int instrument_id);

  // Stops playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  void SetInstrumentNoteOff(int instrument_id, float index);

  // Starts playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  // @param intensity Note intensity.
  void SetInstrumentNoteOn(int instrument_id, float index, float intensity);

 private:
  // Returns instrument with the given |instrument_id|.
  Instrument* GetInstrument(int instrument_id);

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

  // Instruments.
  std::unordered_map<int, std::unique_ptr<Instrument>> instruments_;

  // Sequencers.
  std::unordered_map<int, Sequencer> sequencers_;
};

template <typename InstrumentType, typename... ArgumentTypes>
int BarelyMusician::CreateInstrument(ArgumentTypes... arguments) {
  const int instrument_id = ++id_counter_;
  task_runner_.Add([this, arguments..., instrument_id]() {
    instruments_.insert(std::make_pair(
        instrument_id, std::make_unique<InstrumentType>(arguments...)));
  });
  return instrument_id;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_API_BARELYMUSICIAN_H_
