#ifndef BARELYMUSICIAN_ENGINE_ENGINE_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_H_

#include <map>
#include <memory>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/status.h"
#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "barelymusician/instrument/instrument_manager.h"

namespace barelyapi {

// Class that manages processing of instruments.
class Engine {
 public:
  // Beat callback signature.
  using BeatCallback = std::function<void(int64 timestamp, int beat)>;

  // Note off callback signature.
  using NoteOffCallback =
      std::function<void(int64 timestamp, int64 instrument_id, float pitch)>;

  // Note on callback signature.
  using NoteOnCallback = std::function<void(
      int64 timestamp, int64 instrument_id, float pitch, float intensity)>;

  // Constructs new |Engine|.
  Engine();

  // Creates new instrument.
  //
  // @param instrument instrument Instrument to play.
  // @param params Default instrument params.
  // @return Instrument id.
  StatusOr<int64> Create(InstrumentDefinition definition,
                         InstrumentParamDefinitions param_definitions = {});

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  // @return Status.
  Status Destroy(int64 instrument_id);

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value, if found.
  StatusOr<float> GetParam(int64 instrument_id, int param_id) const;

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
  // @param pitch Pitch.
  // @return True if note is active, if instrument found.
  StatusOr<bool> IsNoteOn(int64 instrument_id, float pitch) const;

  // Returns playback state.
  //
  // @return True if playing.
  bool IsPlaying() const;

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp Begin timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return Status.
  Status Process(int64 instrument_id, int64 timestamp, float* output,
                 int num_channels, int num_frames);

  // Resets all parameters.
  //
  // @param instrument_id Instrument id.
  // @return True if instrument found.
  Status ResetAllParams(int64 instrument_id);

  Status ResetParam(int64 instrument_id, int param_id);

  // Stops all notes.
  void SetAllNotesOff();

  // Stops all notes.
  //
  // @param instrument_id Instrument id.
  // @return True if instrument found.
  Status SetAllNotesOff(int64 instrument_id);

  // Sets custom data.
  //
  // @param instrument_id Instrument id.
  // @param custom_data Custom data.
  // @return True if successful, if instrument found.
  Status SetCustomData(int64 instrument_id, void* custom_data);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param pitch Note pitch.
  // @return True if successful, if instrument found.
  Status SetNoteOff(int64 instrument_id, float pitch);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param pitch Note pitch.
  // @param intensity Note intensity.
  // @return True if successful, if instrument found.
  Status SetNoteOn(int64 instrument_id, float pitch, float intensity);

  // Sets control parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param param_value Parameter value.
  // @return True if successful, if instrument parameter found.
  Status SetParam(int64 instrument_id, int param_id, float param_value);

  // Clears all scheduled notes.
  void ClearAllScheduledNotes();

  // Clears all scheduled notes.
  //
  // @param instrument_id Instrument id.
  // @return True if instrument found.
  Status ClearAllScheduledNotes(int64 instrument_id);

  // Schedules note.
  //
  // @param instrument_id Instrument id.
  // @param position Note position.
  // @param duration Note duration.
  // @param pitch Note pitch.
  // @param intensity Note intensity.
  // @return True if successful, if instrument found.
  Status ScheduleNote(int64 instrument_id, double position, double duration,
                      float pitch, float intensity);

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
  void Start(int64 timestamp);

  // Stops playback.
  void Stop();

  // Updates the internal state.
  //
  // @param timestamp Update timestamp.
  void Update(int sample_rate, int64 timestamp);

 private:
  // List of instruments.
  InstrumentManager manager_;
  std::unordered_map<int64, std::multimap<double, InstrumentData>> scores_;

  // Denotes whether the clock is currently playing.
  bool is_playing_;

  // Last timestamp.
  int64 last_timestamp_;

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
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_H_
