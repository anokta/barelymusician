#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_

#ifdef BARELYMUSICIAN_UNITY_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif  // BARELYMUSICIAN_UNITY_EXPORTS

namespace barelyapi {
namespace unity {

// TODO(#49): Refactor below using barelymusician api.
extern "C" {

// Sequencer beat callback.
using BeatCallback = void(int, int, int);

// Initializes the system.
//
// @param sample_rate System sampling rate.
// @param num_channels System number of channels.
// @param num_frames System number of frames per buffer.
void EXPORT_API Initialize(int sample_rate, int num_channels, int num_frames);

// Shuts down the system.
void EXPORT_API Shutdown();

// Creates new sequencer.
//
// @param beat_callback Sequencer beat callback.
// @return Sequencer ID.
int EXPORT_API CreateSequencer(BeatCallback* beat_callback);

// Destroys sequencer.
//
// @param Sequencer ID.
void EXPORT_API DestroySequencer(int sequencer_id);

// Processes sequencer.
void EXPORT_API ProcessSequencer(int sequencer_id);

// Sets sequencer's number of bars per section.
//
// @param num_bars Sequencer number of bars per section.
void EXPORT_API SetSequencerNumBars(int sequencer_id, int num_bars);

// Sets sequencer's number of beats per bar.
//
// @param num_beats Sequencer number of beats per bar.
void EXPORT_API SetSequencerNumBeats(int sequencer_id, int num_beats);

// Sets sequencer's tempo.
//
// @param tempo Sequencer tempo.
void EXPORT_API SetSequencerTempo(int sequencer_id, float tempo);

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
