#ifndef EXAMPLES_INSTRUMENTS_BASIC_SAMPLER_VOICE_H_
#define EXAMPLES_INSTRUMENTS_BASIC_SAMPLER_VOICE_H_

#include <vector>

#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/instrument/voice.h"

namespace barelyapi {
namespace examples {

class BasicSamplerVoice : public Voice {
 public:
  // Constructs new |BasicSamplerVoice| with the given |sample_interval|.
  //
  // @param sample_interval Sampling interval in seconds.
  BasicSamplerVoice(float sample_interval);

  // Implements |Generator|.
  float Next() override;
  void Reset() override;

  // Implements |Voice|.
  bool IsActive() const override;
  void Start() override;
  void Stop() override;

  // Sets envelope attack.
  //
  // @param attack Envelope attack.
  void SetEnvelopeAttack(float attack);

  // Sets envelope decay.
  //
  // @param decay Envelope decay.
  void SetEnvelopeDecay(float decay);

  // Sets envelope sustain.
  //
  // @param sustain Envelope sustain.
  void SetEnvelopeSustain(float sustain);

  // Sets envelope release.
  //
  // @param release Envelope release.
  void SetEnvelopeRelease(float release);

  // Sets gain.
  //
  // @param gain Linear gain.
  void SetGain(float gain);

  // Sets sample player data.
  //
  // @param data Sample data.
  // @param frequency Data sampling frequency in Hz.
  void SetSamplePlayerData(const std::vector<float>& data, int frequency);

  // Sets sample player loop.
  //
  // @param loop True if looping.
  void SetSamplePlayerLoop(bool loop);

  // Sets sample player speed.
  //
  // @param speed Playback speed.
  void SetSamplePlayerSpeed(float speed);

 private:
  // Voice envelope.
  Envelope envelope_;

  // Voice unit generator.
  SamplePlayer sample_player_;

  // Voice gain.
  float gain_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_INSTRUMENTS_BASIC_SAMPLER_VOICE_H_
