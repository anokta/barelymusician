#include "barelymusician/instruments/percussion_instrument.h"

#include <array>
#include <cassert>

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

void PercussionInstrument::Process(double* output_samples, int output_channel_count,
                                   int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    double mono_sample = 0.0;
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
      gain_processor_.SetGain(static_cast<double>(value));
      break;
    case Control::kRelease:
      release_ = static_cast<double>(value);
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
  const double* data_double = static_cast<const double*>(data);
  if (!data_double) {
    pads_.clear();
    return;
  }
  const int voice_count = static_cast<int>(*data_double++);
  pads_.resize(voice_count, Pad(frame_rate_));
  for (auto& pad : pads_) {
    // Pad data is sequentially aligned by pitch, frequency, length and data.
    const auto pitch_numerator = static_cast<std::int64_t>(static_cast<double>(*data_double++));
    const auto pitch_denominator = static_cast<std::int64_t>(static_cast<double>(*data_double++));
    pad.pitch = Rational(pitch_numerator, pitch_denominator);
    const int frequency = static_cast<int>(static_cast<double>(*data_double++));
    const int length = static_cast<int>(static_cast<double>(*data_double++));
    pad.voice.generator().SetData(data_double, frequency, length);
    pad.voice.envelope().SetRelease(release_);
    data_double += length;
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

void PercussionInstrument::SetNoteOn(Rational pitch, Rational intensity) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.set_gain(static_cast<double>(intensity));
      pad.voice.Start();
      break;
    }
  }
}

}  // namespace barely
