#ifndef BARELYMUSICIAN_ENGINE_SEQUENCER_H_
#define BARELYMUSICIAN_ENGINE_SEQUENCER_H_

#include <functional>
#include <unordered_map>

#include "barelymusician/engine/engine.h"
#include "barelymusician/engine/message_queue.h"

namespace barelyapi {

class Sequencer {
 public:
  // Beat callback signature.
  using BeatCallback = std::function<void(int beat)>;

  explicit Sequencer(Engine* engine);

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

  void Create(int instrument_id);
  void Destroy(int instrument_id);

  void ScheduleNote(int instrument_id, double position, double duration,
                    float index, float intensity);

  void ScheduleNoteOff(int instrument_id, double position, float index);

  void ScheduleNoteOn(int instrument_id, double position, float index,
                      float intensity);

  // Sets beat callback.
  //
  // @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  // Sets playback position.
  //
  // @param position Position in beats.
  void SetPosition(double position);

  // Sets playback tempo.
  //
  // @param tempo Tempo in BPM.
  void SetTempo(double tempo);

  // Starts playback.
  void Start(double timestamp);

  // Stops playback.
  void Stop();

  void Update(double timestamp, double lookahead);

 private:
  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Playback position.
  double position_;

  // Playback tempo.
  double tempo_;

  double start_timestamp_;
  double last_timestamp_;

  // Beat callback.
  BeatCallback beat_callback_;

  Engine* engine_;  // not owned.

  std::unordered_map<int, MessageQueue> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_SEQUENCER_H_
