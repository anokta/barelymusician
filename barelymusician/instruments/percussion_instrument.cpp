#include "barelymusician/instruments/percussion_instrument.h"

namespace barely {

namespace {

// Maximum number of pads allowed to be set.
constexpr int kMaxPadCount = 64;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
PercussionInstrument::PercussionInstrument(int frame_rate) noexcept
    : frame_rate_(frame_rate),
      gain_processor_(frame_rate),
      release_(kDefaultPadRelease) {
  pads_.reserve(kMaxPadCount);
}

void PercussionInstrument::Process(double* output_samples,
                                   int output_channel_count,
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
  gain_processor_.Process(output_samples, output_channel_count,
                          output_frame_count);
}

void PercussionInstrument::SetControl(int index, double value,
                                      double /*slope_per_frame*/) noexcept {
  switch (static_cast<PercussionInstrumentControl>(index)) {
    case PercussionInstrumentControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case PercussionInstrumentControl::kRelease:
      release_ = value;
      for (auto& pad : pads_) {
        pad.voice.envelope().SetRelease(release_);
      }
      break;
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void PercussionInstrument::SetData(const void* data,
                                   [[maybe_unused]] int size) noexcept {
  const double* data_double = static_cast<const double*>(data);
  if (!data_double) {
    pads_.clear();
    return;
  }
  const int voice_count = static_cast<int>(*data_double++);
  pads_.resize(voice_count, Pad(frame_rate_));
  for (int i = 0; i < static_cast<int>(pads_.size()); ++i) {
    // Pad data is sequentially aligned by pitch, frequency, length and data.
    pads_[i].pitch = static_cast<double>(*data_double++);
    const int frequency = static_cast<int>(static_cast<double>(*data_double++));
    const int length = static_cast<int>(static_cast<double>(*data_double++));
    pads_[i].voice.generator().SetData(data_double, frequency, length);
    pads_[i].voice.envelope().SetRelease(release_);
    data_double += length;
  }
}

void PercussionInstrument::SetNoteOff(double pitch) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.Stop();
      break;
    }
  }
}

void PercussionInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.set_gain(intensity);
      pad.voice.Start();
      break;
    }
  }
}

}  // namespace barely
