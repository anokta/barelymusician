#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/common/status.h"
#include "barelymusician/common/task_runner.h"
#include "barelymusician/engine/clock.h"
#include "barelymusician/instrument/instrument_controller.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "barelymusician/instrument/instrument_processor.h"

namespace barelyapi {

/// Beat callback signature.
using BeatCallback = std::function<void(double timestamp, int beat)>;

/// Instrument note off callback signature.
using NoteOffCallback =
    std::function<void(double timestamp, int instrument_id, float note_pitch)>;

/// Instrument note on callback signature.
using NoteOnCallback =
    std::function<void(double timestamp, int instrument_id, float note_pitch,
                       float note_intensity)>;

/// Instrument playback engine.
class Engine {
 public:
  /// Constructs new |Engine|.
  Engine();

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  int CreateInstrument(InstrumentDefinition definition,
                       InstrumentParamDefinitions param_definitions);

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status DestroyInstrument(int instrument_id);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches if successful, error status otherwise.
  StatusOr<std::vector<float>> GetAllInstrumentNotes(int instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters if successful, error status otherwise.
  StatusOr<std::vector<Param>> GetAllInstrumentParams(int instrument_id) const;

  /// Returns instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Parameter value if successful, error status otherwise.
  StatusOr<float> GetInstrumentParam(int instrument_id, int param_id) const;

  /// Returns the playback position.
  ///
  /// @return Position in beats.
  double GetPlaybackPosition() const;

  /// Returns the playback tempo.
  ///
  /// @return Tempo in BPM.
  double GetPlaybackTempo() const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, error status otherwise.
  StatusOr<bool> IsInstrumentNoteOn(int instrument_id, float note_pitch) const;

  bool IsPlaying() const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return Status.
  Status ProcessInstrument(int instrument_id, double timestamp, float* output,
                           int num_channels, int num_frames);

  /// Removes all scheduled instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status RemoveAllScheduledInstrumentNotes(int instrument_id);

  /// Resets all parameters of all instruments to their default values.
  void ResetAllInstrumentParams();

  /// Resets all instrument parameters to their default values.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status ResetAllInstrumentParams(int instrument_id);

  /// Resets instrument parameter to its default value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Status.
  Status ResetInstrumentParam(int instrument_id, int param_id);

  /// Schedules instrument note.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_position Note position in beats.
  /// @param note_duration Note duration in beats.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status ScheduleInstrumentNote(int instrument_id, double note_position,
                                double note_duration, float note_pitch,
                                float note_intensity);

  /// Sets all active notes of all instruments off.
  void SetAllInstrumentNotesOff();

  /// Sets all active instrument notes off.
  ///
  /// @param instrument_id Instrument id.
  /// @return Status.
  Status SetAllInstrumentNotesOff(int instrument_id);

  /// Sets beat callback.
  ///
  /// @param beat_callback Beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  /// Sets instrument custom data.
  ///
  /// @param instrument_id Instrument id.
  /// @param custom_data Custom data.
  /// @return Status.
  Status SetCustomInstrumentData(int instrument_id, void* custom_data);

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return Status.
  Status SetInstrumentNoteOff(int instrument_id, float note_pitch);

  /// Sets instrument note on.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return Status.
  Status SetInstrumentNoteOn(int instrument_id, float note_pitch,
                             float note_intensity);

  /// Sets instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @return Status.
  Status SetInstrumentParam(int instrument_id, int param_id, float param_value);

  /// Sets note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets the playback position.
  ///
  /// @param position Position in beats.
  void SetPlaybackPosition(double position);

  /// Sets the playback tempo.
  ///
  /// @param tempo Tempo in BPM.
  void SetPlaybackTempo(double tempo);

  /// Sets sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(int sample_rate);

  /// Starts the playback.
  void StartPlayback();

  // Stops the playback.
  void StopPlayback();

  /// Updates internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Sets instrument processor |data| at |timestamp|.
  void SetProcessorData(int instrument_id, double timestamp,
                        InstrumentData data);

  // Sampling rate in Hz.
  int sample_rate_;

  // Playback clock.
  Clock clock_;

  // Denotes whether playback is active or not.
  bool is_playing_;

  // Last updated timestamp in seconds.
  double timestamp_;

  // Instrument id counter.
  int id_counter_;

  // List of instruments.
  std::unordered_map<int, InstrumentController> controllers_;
  std::unordered_map<int, InstrumentProcessor> processors_;

  // Audio thread task runner.
  TaskRunner task_runner_;

  // Beat callback.
  BeatCallback beat_callback_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
