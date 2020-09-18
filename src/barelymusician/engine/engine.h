#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

// Engine that plays instruments with respect to its beat sequencer.
// @note This class is *not* thread-safe.
class Engine {
 public:
  // Beat callback signature.
  using BeatCallback = std::function<void(int beat)>;

  // Note off callback signature.
  using NoteOffCallback = std::function<void(int instrument_id, float index)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(int instrument_id, float index, float intensity)>;

  // Constructs new |Engine|.
  //
  // @param sample_rate Sampling rate in Hz.
  explicit Engine(int sample_rate);

  // Creates new instrument.
  //
  // @param instrument_id Instrument id.
  // @param instrument Instrument to play.
  void Create(int instrument_id, std::unique_ptr<Instrument> instrument);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  void Destroy(int instrument_id);

  // Returns playback position.
  //
  // @return Position in beats.
  double GetPosition() const;

  // Returns playback tempo.
  //
  // @return Tempo in BPM.
  double GetTempo() const;

  // Returns playback state.
  //
  // @return True if playing.
  bool IsPlaying() const;

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  void NoteOff(int instrument_id, float index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOn(int instrument_id, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(int instrument_id, float* output, int num_channels,
               int num_frames);

  // Schedules note.
  //
  // @param instrument_id Instrument id.
  // @param position Note position in beats.
  // @param duration Note duration in beats.
  // @param index Note index.
  // @param intensity Note intensity.
  void ScheduleNote(int instrument_id, double position, double duration,
                    float index, float intensity);

  // Schedules note off.
  //
  // @param instrument_id Instrument id.
  // @param position Note position in beats.
  // @param index Note index.
  void ScheduleNoteOff(int instrument_id, double position, float index);

  // Schedules note on.
  //
  // @param instrument_id Instrument id.
  // @param position Note position in beats.
  // @param index Note index.
  // @param index Note intensity.
  void ScheduleNoteOn(int instrument_id, double position, float index,
                      float intensity);

  // Sets beat callback.
  //
  // @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback&& beat_callback);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback&& note_off_callback);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback&& note_on_callback);

  // Sets playback position.
  //
  // @param position Position in beats.
  void SetPosition(double position);

  // Sets playback tempo.
  //
  // @param tempo Tempo in BPM.
  void SetTempo(double tempo);

  // Starts playback.
  void Start();

  // Stops playback.
  void Stop();

  // Updates engine.
  //
  // @param num_frames Number of frames to iterate.
  void Update(int num_frames);

 private:
  // Instrument data.
  struct InstrumentData {
    // Instrument to play.
    std::unique_ptr<Instrument> instrument;

    // Active note indices that are being played.
    std::unordered_set<float> active_note_indices;
  };

  // Returns |InstrumentData| for the given |instrument_id|.
  InstrumentData* GetInstrumentData(int instrument_id);

  // Clock for playback.
  Clock clock_;

  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Last playback position.
  double last_position_;

  // List of instruments.
  std::unordered_map<int, InstrumentData> instruments_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
