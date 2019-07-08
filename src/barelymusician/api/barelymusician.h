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
  // Sequencer beat event callback signature.
  using BeatCallback = Sequencer::BeatCallback;

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

  // Starts playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  // @param intensity Note intensity.
  void StartInstrumentNote(int instrument_id, float index, float intensity);

  // Stops playing instrument note.
  //
  // @param instrument_id Instrument ID.
  // @param index Note index.
  void StopInstrumentNote(int instrument_id, float index);

  // Resets sequencer playback.
  void ResetSequencer();

  // Sets sequencer beat callback.
  //
  // @param beat_callback Sequencer beat callback.
  void SetSequencerBeatCallback(BeatCallback beat_callback);

  // Sets sequencer number of bars per section.
  //
  // @param num_beats Number of bars per section.
  void SetSequencerNumBars(int num_bars);

  // Sets sequencer number of beats per bar.
  //
  // @param num_beats Number of beats per bar.
  void SetSequencerNumBeats(int num_beats);

  // Sets sequencer tempo.
  //
  // @param tempo Sequencer tempo.
  void SetSequencerTempo(float tempo);

  // Starts sequencer playback.
  void StartSequencer();

  // Stops sequencer playback.
  void StopSequencer();

  // Updates the internal state.
  void Update();

 private:
  // Returns instrument with the given |instrument_id|.
  Instrument* GetInstrument(int instrument_id);

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

  // Sequencer.
  Sequencer sequencer_;

  // Denotes whether the sequencer is playing.
  bool is_playing_;
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
