#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/util/task_runner.h"

namespace barelyapi {

// Class that manages processing of instruments.
class Engine {
 public:
  // Id type.
  using Id = std::int64_t;

  // Beat callback signature.
  using BeatCallback = std::function<void(double timestamp, int beat)>;

  // Note off callback signature.
  using NoteOffCallback =
      std::function<void(double timestamp, Id instrument_id, float index)>;

  // Note on callback signature.
  using NoteOnCallback = std::function<void(double timestamp, Id instrument_id,
                                            float index, float intensity)>;

  // Constructs new |Engine|.
  Engine();

  // Creates new instrument.
  //
  // @param definition Instrument definition.
  // @return Instrument id.
  Id Create(std::unique_ptr<Instrument> instrument,
            std::vector<std::pair<int, float>> params);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  bool Destroy(Id instrument_id);

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param id Parameter id.
  // @param value Parameter value.
  std::optional<float> GetParam(Id instrument_id, int id) const;

  // Returns playback position.
  //
  // @return Position in beats.
  double GetPosition() const;

  // Returns playback tempo.
  //
  // @return Tempo in BPM.
  double GetTempo() const;

  // Returns whether note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  std::optional<bool> IsNoteOn(Id instrument_id, float index) const;

  // Returns playback state.
  //
  // @return True if playing.
  bool IsPlaying() const;

  // Stops all notes.
  void AllNotesOff();

  // Stops all notes.
  //
  // @param instrument_id Instrument id.
  bool AllNotesOff(Id instrument_id);

  // Sets control parameter value.
  //
  // @param instrument_id Instrument id.
  // @param id Parameter id.
  // @param value Parameter value.
  std::optional<bool> Control(Id instrument_id, int id, float value);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  bool NoteOff(Id instrument_id, float index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  // @param intensity Note intensity.
  bool NoteOn(Id instrument_id, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp Begin timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  bool Process(Id instrument_id, double begin_timestamp, double end_timestamp,
               float* output, int num_channels, int num_frames);

  // Resets all parameters.
  //
  // @param instrument_id Instrument id.
  bool ResetAllParams(Id instrument_id);

  // Schedules control.
  bool ScheduleControl(Id instrument_id, double position, int id, float value);

  // Schedules note.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param duration Note duration.
  // @param index Note index.
  // @param intensity Note intensity.
  bool ScheduleNote(Id instrument_id, double position, double duration,
                    float index, float intensity);

  // Schedules note off.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param index Note index.
  bool ScheduleNoteOff(Id instrument_id, double position, float index);

  // Schedules note on.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param index Note index.
  // @param index Note intensity.
  bool ScheduleNoteOn(Id instrument_id, double position, float index,
                      float intensity);

  // Sets beat callback.
  //
  // @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  // Sets conductor.
  //
  // @param conductor Conductor.
  void SetConductor(std::unique_ptr<Conductor> conductor);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

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

  // Updates the internal state.
  //
  // @param timestamp Timestamp.
  void Update(double timestamp);

 private:
  // Instrument parameter.
  struct InstrumentParam {
    // Current parameter value.
    float value;

    // Default parameter value.
    float default_value;
  };

  // Instrument controller (main thread).
  struct Controller {
    InstrumentController controller;

    // Scheduled messages.
    MessageQueue messages;
  };

  // List of instruments.
  std::unordered_map<Id, Controller> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Playback position.
  double position_;

  // Playback tempo.
  double tempo_;

  double last_timestamp_;

  // Conductor.
  std::unique_ptr<Conductor> conductor_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // Id counter.
  Id id_counter_;

  // Task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
