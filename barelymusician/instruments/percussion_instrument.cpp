#include "barelymusician/instruments/percussion_instrument.h"

#include <array>
#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelyPercussionInstrument_GetDefinition() {
  return barely::PercussionInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of pads allowed to be set.
constexpr int kMaxPadCount = 64;

// Default pad release in seconds.
constexpr double kDefaultPadRelease = 0.1;

}  // namespace

InstrumentDefinition PercussionInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{Control::kGain, 1.0, 0.0, 1.0},
          // Pad release.
          ControlDefinition{Control::kRelease, kDefaultPadRelease, 0.0, 60.0},
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

void PercussionInstrument::SetControl(int id, double value) noexcept {
  switch (static_cast<Control>(id)) {
    case Control::kGain:
      gain_processor_.SetGain(value);
      break;
    case Control::kRelease:
      release_ = value;
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
    // Pad data is sequentially aligned by note, frequency, length, and data.
    pad.note = static_cast<double>(*data_double++);
    const int frequency = static_cast<int>(static_cast<double>(*data_double++));
    const int length = static_cast<int>(static_cast<double>(*data_double++));
    pad.voice.generator().SetData(data_double, frequency, length);
    pad.voice.envelope().SetRelease(release_);
    data_double += length;
  }
}

void PercussionInstrument::SetNoteOff(double note) noexcept {
  for (auto& pad : pads_) {
    if (pad.note == note) {
      pad.voice.Stop();
      break;
    }
  }
}

void PercussionInstrument::SetNoteOn(double note, double intensity) noexcept {
  for (auto& pad : pads_) {
    if (pad.note == note) {
      pad.voice.set_gain(intensity);
      pad.voice.Start();
      break;
    }
  }
}

}  // namespace barely
