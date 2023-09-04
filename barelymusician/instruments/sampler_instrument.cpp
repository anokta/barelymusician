#include "barelymusician/instruments/sampler_instrument.h"

#include <cmath>
#include <cstdlib>

#include "barelymusician/dsp/enveloped_voice.h"

namespace barely {

// NOLINTNEXTLINE(bugprone-exception-escape)
SamplerInstrument::SamplerInstrument(int frame_rate) noexcept
    : voice_(SamplerVoice(frame_rate), kMaxSamplerVoiceCount),
      gain_processor_(frame_rate) {}

void SamplerInstrument::Process(double* output_samples,
                                int output_channel_count,
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
void SamplerInstrument::SetControl(int index, double value,
                                   double /*slope*/) noexcept {
  switch (static_cast<SamplerInstrumentControl>(index)) {
    case SamplerInstrumentControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case SamplerInstrumentControl::kRootPitch:
      root_pitch_ = value;
      break;
    case SamplerInstrumentControl::kLoop:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->generator().SetLoop(static_cast<bool>(value));
      });
      break;
    case SamplerInstrumentControl::kAttack:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SamplerInstrumentControl::kDecay:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SamplerInstrumentControl::kSustain:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SamplerInstrumentControl::kRelease:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SamplerInstrumentControl::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
    default:
      std::abort();
      break;
  }
}

void SamplerInstrument::SetData(const void* data, int size) noexcept {
  const double* data_double = static_cast<const double*>(data);
  const double* sample_data = size > 0 ? &data_double[1] : nullptr;
  const int frame_rate = size > 0 ? static_cast<int>(data_double[0]) : 0;
  const int length = size > 0 ? size / static_cast<int>(sizeof(double)) - 1 : 0;
  voice_.Update(
      [sample_data, frame_rate, length](SamplerVoice* voice) noexcept {
        voice->generator().SetData(sample_data, frame_rate, length);
      });
}

void SamplerInstrument::SetNoteOff(double pitch) noexcept {
  voice_.Stop(pitch);
}

void SamplerInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  const double speed = std::pow(2.0, pitch - root_pitch_);
  voice_.Start(pitch, [speed, intensity](SamplerVoice* voice) noexcept {
    voice->generator().SetSpeed(speed);
    voice->set_gain(intensity);
  });
}

}  // namespace barely
