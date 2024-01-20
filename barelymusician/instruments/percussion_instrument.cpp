#include "barelymusician/instruments/percussion_instrument.h"

#include <array>
#include <cassert>
#include <cstdint>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelyPercussionInstrument_GetDefinition() {
  return barely::PercussionInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of pads allowed to be set.
constexpr int kMaxPadCount = 64;

// Default pad release in seconds.
constexpr Rational kDefaultPadRelease = Rational(1, 10);

}  // namespace

InstrumentDefinition PercussionInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{1, 0, 1},
          // Pad release.
          ControlDefinition{kDefaultPadRelease, 0, 60},
      };
  return CustomInstrument::GetDefinition<PercussionInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
PercussionInstrument::PercussionInstrument(int frame_rate) noexcept
    : frame_rate_(frame_rate), gain_processor_(frame_rate), release_(kDefaultPadRelease) {
  pads_.reserve(kMaxPadCount);
}

void PercussionInstrument::Process(float* output_samples, int output_channel_count,
                                   int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    float mono_sample = 0.0f;
    for (auto& pad : pads_) {
      mono_sample += pad.voice.Next(0);
    }
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[frame * output_channel_count + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

void PercussionInstrument::SetControl(int index, Rational value,
                                      Rational /*slope_per_frame*/) noexcept {
  switch (static_cast<Control>(index)) {
    case Control::kGain:
      gain_processor_.SetGain(static_cast<float>(value));
      break;
    case Control::kRelease:
      release_ = static_cast<float>(value);
      for (auto& pad : pads_) {
        pad.voice.envelope().SetRelease(release_);
      }
      break;
    default:
      assert(false);
      break;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void PercussionInstrument::SetData(const void* data, [[maybe_unused]] int size) noexcept {
  const float* data_float = static_cast<const float*>(data);
  if (!data_float) {
    pads_.clear();
    return;
  }
  const int voice_count = static_cast<int>(*data_float++);
  pads_.resize(voice_count, Pad(frame_rate_));
  for (auto& pad : pads_) {
    // Pad data is sequentially aligned by pitch, frequency, length and data.
    const auto pitch_numerator = static_cast<int64_t>(static_cast<float>(*data_float++));
    const auto pitch_denominator = static_cast<int64_t>(static_cast<float>(*data_float++));
    pad.pitch = Rational(pitch_numerator, pitch_denominator);
    const int frequency = static_cast<int>(static_cast<float>(*data_float++));
    const int length = static_cast<int>(static_cast<float>(*data_float++));
    pad.voice.generator().SetData(data_float, frequency, length);
    pad.voice.envelope().SetRelease(release_);
    data_float += length;
  }
}

void PercussionInstrument::SetNoteOff(Rational pitch) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.Stop();
      break;
    }
  }
}

void PercussionInstrument::SetNoteOn(Rational pitch, float intensity) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.set_gain(intensity);
      pad.voice.Start();
      break;
    }
  }
}

}  // namespace barely
