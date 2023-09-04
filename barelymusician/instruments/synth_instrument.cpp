#include "barelymusician/instruments/synth_instrument.h"

#include <cstdlib>

#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/oscillator.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
SynthInstrument::SynthInstrument(int frame_rate) noexcept
    : voice_(SynthVoice(frame_rate), kMaxSynthVoiceCount),
      gain_processor_(frame_rate) {}

void SynthInstrument::Process(double* output_samples, int output_channel_count,
                              int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count,
                          output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SynthInstrument::SetControl(int index, double value,
                                 double /*slope_per_frame*/) noexcept {
  switch (static_cast<SynthInstrumentControl>(index)) {
    case SynthInstrumentControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case SynthInstrumentControl::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthInstrumentControl::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthInstrumentControl::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SynthInstrumentControl::kSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SynthInstrumentControl::kRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthInstrumentControl::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
    default:
      std::abort();
      break;
  }
}

void SynthInstrument::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  voice_.Start(pitch, [pitch, intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    voice->set_gain(intensity);
  });
}

}  // namespace barely
