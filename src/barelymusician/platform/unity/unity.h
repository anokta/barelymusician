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

// Registers beat callback.
//
// @param beat_callback Sequencer beat callback.
void EXPORT_API RegisterBeatCallback(BeatCallback* beat_callback);

// Sets number of bars per section.
//
// @param num_bars Sequencer number of bars per section.
void EXPORT_API SetNumBars(int num_bars);

// Sets number of beats per bar.
//
// @param num_beats Sequencer number of beats per bar.
void EXPORT_API SetNumBeats(int num_beats);

// Sets tempo.
//
// @param tempo Sequencer tempo.
void EXPORT_API SetTempo(float tempo);

// Shuts down the system.
void EXPORT_API Shutdown();

// Updates the state of the system to be called in each audio processing
// callback.
void EXPORT_API Update();

}  // extern "C"

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_H_
