#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <any>
#include <functional>
#include <map>
#include <unordered_map>
#include <variant>
#include <vector>

#include "barelymusician/common/id.h"
#include "barelymusician/common/id_generator.h"
#include "barelymusician/engine/instrument_controller.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"
#include "barelymusician/engine/instrument_processor.h"
#include "barelymusician/engine/task_runner.h"

namespace barelyapi {

/// Class that manages processing of instruments.
class InstrumentManager {
 public:
  /// Note off callback signature.
  using NoteOffCallback =
      std::function<void(Id instrument_id, float note_pitch)>;

  /// Note on callback signature.
  using NoteOnCallback = std::function<void(Id instrument_id, float note_pitch,
                                            float note_intensity)>;

  /// Constructs new |InstrumentManager|.
  ///
  /// @param sample_rate Sampling rate in Hz.
  /// @param id_generator Pointer to id generator.
  InstrumentManager(int sample_rate, IdGenerator* id_generator);

  /// Creates new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param param_definitions Instrument parameter definitions.
  /// @return Instrument id.
  Id Create(InstrumentDefinition definition,
            InstrumentParamDefinitions param_definitions);

  /// Destroys instrument.
  ///
  /// @param instrument_id Instrument id.
  /// @return True if successful, false otherwise.
  bool Destroy(Id instrument_id);

  /// Returns all active instrument notes.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of active note pitches.
  std::vector<float> GetAllNotes(Id instrument_id) const;

  /// Returns all instrument parameters.
  ///
  /// @param instrument_id Instrument id.
  /// @return List of parameters.
  std::vector<InstrumentParam> GetAllParams(Id instrument_id) const;

  /// Returns instrument parameter value.
  ///
  /// @param instrument_id Instrument id.
  /// @param param_id Parameter id.
  /// @return Pointer to parameter value if successful, nullptr otherwise.
  const float* GetParam(Id instrument_id, int param_id) const;

  /// Returns whether instrument note is active or not.
  ///
  /// @param instrument_id Instrument id.
  /// @param note_pitch Note pitch.
  /// @return True if note is active, false otherwise.
  bool IsNoteOn(Id instrument_id, float note_pitch) const;

  /// Processes the next instrument output buffer at timestamp.
  ///
  /// @param instrument_id Instrument id.
  /// @param timestamp Timestamp in seconds.
  /// @param output Pointer to the output buffer.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames.
  /// @return True if successful, false otherwise.
  bool Process(Id instrument_id, double timestamp, float* output,
               int num_channels, int num_frames);

  bool SetEvent(Id instrument_id, double timestamp,
                InstrumentControllerEvent event);

  /// Sets instrument events at their timestamps.
  ///
  /// @param instrument_id Instrument id.
  /// @param events List of events with their timestamps.
  /// @return True if successful, false otherwise.
  bool SetEvents(Id instrument_id, InstrumentControllerEvents events);

  /// Sets the note off callback.
  ///
  /// @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  /// Sets the note on callback.
  ///
  /// @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  /// Sets the sampling rate.
  ///
  /// @param sample_rate Sampling rate in Hz.
  void SetSampleRate(int sample_rate);

  void Update(double timestamp);

 private:
  // Sampling rate in Hz.
  int sample_rate_;

  // Instrument id generator.
  IdGenerator* id_generator_;  // not owned.

  // List of instruments.
  std::unordered_map<Id, InstrumentController> controllers_;
  std::unordered_map<Id, InstrumentProcessor> processors_;

  // Audio thread task runner.
  TaskRunner task_runner_;

  // Instrument note off callback.
  NoteOffCallback note_off_callback_;

  // Instrument note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
