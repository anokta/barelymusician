#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <memory>
#include <unordered_map>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/performer.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

class Engine {
 public:
  using BeatCallback = std::function<void(int beat)>;
  using NoteOffCallback = std::function<void(int performer_id, float index)>;
  using NoteOnCallback =
      std::function<void(int performer_id, float index, float intensity)>;

  explicit Engine(int sample_rate);

  int Create(std::unique_ptr<Instrument> instrument);

  void Destroy(int performer_id);

  double GetPosition() const;

  double GetTempo() const;

  bool IsPlaying() const;

  void NoteOff(int performer_id, float index);

  void NoteOn(int performer_id, float index, float intensity);

  void Process(int performer_id, float* output, int num_channels,
               int num_frames);

  void ScheduleNoteOff(int performer_id, float index, double position);

  void ScheduleNoteOn(int performer_id, float index, float intensity,
                      double position);

  void SetBeatCallback(BeatCallback&& beat_callback);

  void SetNoteOffCallback(NoteOffCallback&& note_off_callback);

  void SetNoteOnCallback(NoteOnCallback&& note_on_callback);

  void SetPosition(double position);

  void SetTempo(double tempo);

  void Start();

  void Stop();

  void Update(int num_frames);

 private:
  // Returns |Performer| with the given |performer_id|.
  Performer* GetPerformer(int performer_id);

  // Audio clock.
  Clock clock_;

  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Current clock position.
  double current_position_;

  // Previous clock position.
  double previous_position_;

  // Counter to generate unique performer ids.
  int id_counter_;

  // List of performers.
  std::unordered_map<int, Performer> performers_;

  BeatCallback beat_callback_;

  NoteOffCallback note_off_callback_;

  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
