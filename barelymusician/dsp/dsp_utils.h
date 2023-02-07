#ifndef BARELYMUSICIAN_DSP_DSP_UTILS_H_
#define BARELYMUSICIAN_DSP_DSP_UTILS_H_

namespace barely {

/// PI.
inline constexpr double kPi = 3.14159265358979323846;
inline constexpr double kTwoPi = 2.0 * kPi;

/// Minimum decibel threshold.
inline constexpr double kMinDecibels = -80.0;

/// Converts a value from decibels to linear amplitude.
///
/// @param decibels Value in dB.
/// @return Value in linear amplitude.
double AmplitudeFromDecibels(double decibels) noexcept;

/// Returns the corresponding number of beats for a given number of seconds.
///
/// @param tempo Tempo in beats per minute.
/// @param seconds Number of seconds.
/// @return Number of beats.
double BeatsFromSeconds(double tempo, double seconds);

/// Converts a value from linear amplitude to decibels.
///
/// @param amplitude Value in linear amplitude.
/// @return Value in dB.
double DecibelsFromAmplitude(double amplitude) noexcept;

/// Returns the corresponding number of frames for a given number of seconds.
///
/// @param frame_rate Frame rate in hertz.
/// @param seconds Number of seconds.
/// @return Number of frames.
int FramesFromSeconds(int frame_rate, double seconds);

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param frame_rate Frame rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
double GetFilterCoefficient(int frame_rate, double cuttoff_frequency) noexcept;

/// Returns frequency for the given pitch.
///
/// @param pitch Pitch.
/// @return Frequency in hertz.
double GetFrequency(double pitch) noexcept;

/// Returns the corresponding number of seconds for a given number of beats.
///
/// @param tempo Tempo in beats per minute.
/// @param beats Number of beats.
/// @return Number of seconds.
double SecondsFromBeats(double tempo, double beats);

/// Returns the corresponding number of seconds for a given number of frames.
///
/// @param frame_rate Frame rate in hertz.
/// @param frames Number of frames.
/// @return Number of seconds.
double SecondsFromFrames(int frame_rate, int frames);

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_DSP_UTILS_H_
