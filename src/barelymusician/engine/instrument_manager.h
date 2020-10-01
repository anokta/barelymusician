#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/util/task_runner.h"

namespace barelyapi {

// Class that manages processing of instruments.
class InstrumentManager {
 public:
  // Note off callback signature.
  using NoteOffCallback = std::function<void(int instrument_id, float index)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(int instrument_id, float index, float intensity)>;

  // Constructs new |Engine|.
  //
  // @param sample_rate Sampling rate in Hz.
  explicit InstrumentManager(int sample_rate);

  // Creates new instrument.
  //
  // @param definition Instrument definition.
  // @return Instrument id.
  int Create(InstrumentDefinition definition);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  bool Destroy(int instrument_id);

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param value Parameter value.
  std::optional<float> GetParam(int instrument_id, int param_id) const;

  // Returns whether note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  std::optional<bool> IsNoteOn(int instrument_id, float index) const;

  // Stops all notes.
  //
  // @param instrument_id Instrument id.
  bool AllNotesOff(int instrument_id);

  // Sets control parameter value.
  //
  // @param instrument_id Instrument id.
  // @param id Parameter id.
  // @param value Parameter value.
  bool Control(int instrument_id, int id, float value);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  bool NoteOff(int instrument_id, float index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  // @param intensity Note intensity.
  bool NoteOn(int instrument_id, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp End timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  bool Process(int instrument_id, double begin_timestamp, double end_timestamp,
               float* output, int num_channels, int num_frames);

  // Resets all parameters.
  //
  // @param instrument_id Instrument id.
  bool ResetAllParams(int instrument_id);

  // Schedules control.
  bool ScheduleControl(int instrument_id, double timestamp, int id,
                       float value);

  // Schedules note.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param duration Note duration.
  // @param index Note index.
  // @param intensity Note intensity.
  bool ScheduleNote(int instrument_id, double timestamp, double duration,
                    float index, float intensity);

  // Schedules note off.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  bool ScheduleNoteOff(int instrument_id, double timestamp, float index);

  // Schedules note on.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  // @param index Note intensity.
  bool ScheduleNoteOn(int instrument_id, double timestamp, float index,
                      float intensity);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  // Updates the internal state.
  //
  // @param timestamp Timestamp.
  void Update(double timestamp);

 private:
  // Instrument controller (main thread).
  struct InstrumentController {
    // Instrument definition.
    InstrumentDefinition definition;

    // Instrument params.
    std::unordered_map<int, float> params;

    // Active note indices.
    std::unordered_set<float> active_notes;

    // Scheduled messages.
    MessageQueue messages;
  };

  // Instrument processor (audio thread).
  struct InstrumentProcessor {
    // Instrument to play.
    std::unique_ptr<Instrument> instrument;

    // Scheduled messages.
    MessageQueue messages;
  };

  // List of instruments.
  std::unordered_map<int, InstrumentController> controllers_;
  std::unordered_map<int, InstrumentProcessor> processors_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // Sampling rate.
  int sample_rate_;

  // Id counter.
  int id_counter_;

  // Task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
