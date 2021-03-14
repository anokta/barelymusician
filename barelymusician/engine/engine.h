#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

/// Beat callback signature.
using BeatCallback = std::function<void(int beat)>;

/// Note off callback signature.
using NoteOffCallback =
    std::function<void(int instrument_id, float note_pitch)>;

/// Note on callback signature.
using NoteOnCallback = std::function<void(int instrument_id, float note_pitch,
                                          float note_intensity)>;

/// Instrument playback engine.
class Engine {
 public:
  /// Constructs new |Engine|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  explicit Engine(int sample_rate);

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
  /// @return True if successful, false otherwise.
  bool DestroyInstrument(int instrument_id);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches.
  std::vector<float> GetAllInstrumentNotes(int instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters.
  std::vector<Param> GetAllInstrumentParams(int instrument_id) const;

  /// Returns instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Pointer to parameter value if successful, nullptr otherwise.
  const float* GetInstrumentParam(int instrument_id, int param_id) const;

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
  /// @return True if note is active, false otherwise.
  bool IsInstrumentNoteOn(int instrument_id, float note_pitch) const;

  /// Returns whether the playback is currently active or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return True if successful, false otherwise.
  bool ProcessInstrument(int instrument_id, double timestamp, float* output,
                         int num_channels, int num_frames);

  /// Removes all scheduled notes of all instruments.
  void RemoveAllScheduledInstrumentNotes();

  /// Removes all scheduled instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  bool RemoveAllScheduledInstrumentNotes(int instrument_id);

  /// Resets all parameters of all instruments to their default values.
  void ResetAllInstrumentParams();

  /// Resets all instrument parameters to their default values.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  bool ResetAllInstrumentParams(int instrument_id);

  /// Resets instrument parameter to its default value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return True if successful, false otherwise.
  bool ResetInstrumentParam(int instrument_id, int param_id);

  /// Schedules instrument note.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_begin_position Note begin position in beats.
  /// @param note_end_position Note end position in beats.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return True if successful, false otherwise.
  bool ScheduleInstrumentNote(int instrument_id, double note_begin_position,
                              double note_end_position, float note_pitch,
                              float note_intensity);

  /// Sets all active notes of all instruments off.
  void SetAllInstrumentNotesOff();

  /// Sets all active instrument notes off.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  bool SetAllInstrumentNotesOff(int instrument_id);

  /// Sets the beat callback.
  ///
  /// @param beat_callback Playback beat callback.
  void SetBeatCallback(BeatCallback beat_callback);

  /// Sets custom instrument data.
  ///
  /// @param instrument_id Instrument id.
  /// @param custom_data Custom data.
  /// @return True if successful, false otherwise.
  bool SetCustomInstrumentData(int instrument_id, void* custom_data);

  /// Sets instrument note off.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if successful, false otherwise.
  bool SetInstrumentNoteOff(int instrument_id, float note_pitch);

  /// Sets instrument note on.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @param note_intensity Note intensity.
  /// @return True if successful, false otherwise.
  bool SetInstrumentNoteOn(int instrument_id, float note_pitch,
                           float note_intensity);

  /// Sets instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @param param_value Parameter value.
  /// @return True if successful, false otherwise.
  bool SetInstrumentParam(int instrument_id, int param_id, float param_value);

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets the note on callback.
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

  /// Sets the sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(int sample_rate);

  /// Starts the playback.
  void StartPlayback();

  // Stops the playback.
  void StopPlayback();

  /// Updates the internal state at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp);

 private:
  // Sets processor |event| of instrument with the given |instrument_id|.
  void SetProcessorData(int instrument_id, InstrumentEvent event);

  // Playback clock.
  Clock clock_;

  // Instrument id counter.
  int id_counter_;

  // List of instruments.
  std::unordered_map<int, InstrumentController> controllers_;
  std::unordered_map<int, InstrumentProcessor> processors_;

  // Sampling rate in Hz.
  int sample_rate_;

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
