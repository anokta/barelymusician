#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <memory>
#include <unordered_map>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

// Core sequencing engine that manages instrument performers.
// @note This class is *not* thread-safe.
class Engine {
 public:
  // Beat callback signature.
  using BeatCallback = std::function<void(int beat)>;

  // Note off callback signature.
  using NoteOffCallback = std::function<void(int performer_id, float index)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(int performer_id, float index, float intensity)>;

  // Constructs new |Engine|.
  //
  // @param sample_rate Sampling rate in Hz.
  explicit Engine(int sample_rate);

  // Creates new performer.
  //
  // @param instrument Instrument to perform.
  // @return Performer id.
  int Create(std::unique_ptr<Instrument> instrument);

  // Destroys performer.
  //
  // @param performer_id Performer id.
  void Destroy(int performer_id);

  // Returns playback position.
  //
  // @return Position in beats.
  double GetPosition() const;

  // Returns playback tempo.
  //
  // @return Tempo (BPM).
  double GetTempo() const;

  // Returns whether the engine is playing.
  //
  // @return True if playing.
  bool IsPlaying() const;

  // Stops playing note.
  //
  // @param performer_id Performer id.
  // @param index Note index.
  void NoteOff(int performer_id, float index);

  // Starts playing note.
  //
  // @param performer_id Performer id.
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOn(int performer_id, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param performer_id Performer id.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(int performer_id, float* output, int num_channels,
               int num_frames);

  // Schedules note off to be performed.
  //
  // @param performer_id Performer id.
  // @param index Note index.
  // @param position Position to perform note off.
  void ScheduleNoteOff(int performer_id, float index, double position);

  // Schedules note on to be performed.
  //
  // @param performer_id Performer id.
  // @param index Note index.
  // @param index Note intensity.
  // @param position Position to perform note on.
  void ScheduleNoteOn(int performer_id, float index, float intensity,
                      double position);

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
  // @param tempo Tempo (BPM).
  void SetTempo(double tempo);

  // Starts playback.
  void Start();

  // Stops playback.
  void Stop();

  // Updates engine state.
  //
  // @param num_frames Number of frames to iterate.
  void Update(int num_frames);

 private:
  // Returns |Performer| with the given |performer_id|.
  Performer* GetPerformer(int performer_id);

  // Audio clock.
  Clock clock_;

  // Denotes whether the playback is currently playing.
  bool is_playing_;

  // Current playback position.
  double current_position_;

  // Previous playback position.
  double previous_position_;

  // Counter to generate unique performer ids.
  int id_counter_;

  // List of performers.
  std::unordered_map<int, Performer> performers_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
