#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/base/constants.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/util/task_runner.h"

namespace barelyapi {

// Class that manages processing of instruments.
class Engine {
 public:
  // Beat callback signature.
  using BeatCallback = std::function<void(double timestamp, int beat)>;

  // Note off callback signature.
  using NoteOffCallback =
      std::function<void(double timestamp, Id instrument_id, float note_index)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(double timestamp, Id instrument_id, float note_index,
                         float note_intensity)>;

  // Constructs new |Engine|.
  Engine();

  // Creates new instrument.
  //
  // @param instrument instrument Instrument to play.
  // @param params Default instrument params.
  // @return Instrument id.
  Id Create(std::unique_ptr<Instrument> instrument,
            std::vector<ParamData> params);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  bool Destroy(Id instrument_id);

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value, if found.
  std::optional<float> GetParam(Id instrument_id, int param_id) const;

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
  // @return True if note is active, if instrument found.
  std::optional<bool> IsNoteOn(Id instrument_id, float note_index) const;

  // Returns playback state.
  //
  // @return True if playing.
  bool IsPlaying() const;

  // Stops all notes.
  //
  // @param instrument_id Instrument id.
  // @return True if instrument found.
  bool AllNotesOff(Id instrument_id);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param note_index Note index.
  // @return True if successful, if instrument found.
  std::optional<bool> NoteOff(Id instrument_id, float note_index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param note_index Note index.
  // @param note_intensity Note intensity.
  // @return True if successful, if instrument found.
  std::optional<bool> NoteOn(Id instrument_id, float note_index,
                             float note_intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp Begin timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return True if instrument found.
  bool Process(Id instrument_id, double begin_timestamp, double end_timestamp,
               float* output, int num_channels, int num_frames);

  // Resets all parameters.
  //
  // @param instrument_id Instrument id.
  // @return True if instrument found.
  bool ResetAllParams(Id instrument_id);

  // Schedules note off.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param note_index Note index.
  // @return True if successful, if instrument found.
  std::optional<bool> ScheduleNoteOff(Id instrument_id, double position,
                                      float note_index);

  // Schedules note on.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param note_index Note index.
  // @param note_intensity Note intensity.
  // @return True if successful, if instrument found.
  std::optional<bool> ScheduleNoteOn(Id instrument_id, double position,
                                     float note_index, float note_intensity);

  // Sets beat callback.
  //
  // @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  // Sets control parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param param_value Parameter value.
  // @return True if successful, if instrument parameter found.
  std::optional<bool> SetParam(Id instrument_id, int param_id,
                               float param_value);

  // Sets playback position.
  //
  // @param position Position in beats.
  void SetPosition(double position);

  // Sets playback tempo.
  //
  // @param tempo Tempo in BPM.
  void SetTempo(double tempo);

  // Starts playback.
  //
  // @param timestamp Start timestamp.
  void Start(double timestamp);

  // Stops playback.
  void Stop();

  // Updates the internal state.
  //
  // @param timestamp Update timestamp.
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
  struct InstrumentController {
    // Constructs new |InstrumentController| with the given |default_params|.
    InstrumentController(const std::vector<ParamData>& default_params);

    // Instrument params.
    std::unordered_map<int, InstrumentParam> params;

    // Active note indices.
    std::unordered_set<float> notes;

    // Scheduled messages.
    std::multimap<double, MessageData> messages;
  };

  // Instrument processor (audio thread).
  struct InstrumentProcessor {
    // Constructs new |InstrumentProcessor| with the given
    // |instrument_to_process| and |default_params|.
    InstrumentProcessor(std::unique_ptr<Instrument> instrument_to_process,
                        const std::vector<ParamData>& default_params);

    // Instrument to process.
    std::unique_ptr<Instrument> instrument;

    // Messages to process.
    std::vector<std::pair<double, MessageData>> messages;
  };

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Last timestamp.
  double last_timestamp_;

  // Playback position.
  double position_;

  // Playback tempo.
  double tempo_;

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
