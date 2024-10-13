#include "barelymusician/internal/instrument_processor.h"

#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 32;

// Returns the frequency of a given pitch.
//
// @param pitch Note pitch.
// @param reference_frequency Reference frequency in hertz.
// @return Note frequency.
double GetFrequency(double pitch, double reference_frequency) noexcept {
  return reference_frequency * std::pow(2.0, pitch);
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int frame_rate, double reference_frequency) noexcept
    : reference_frequency_(reference_frequency),
      gain_processor_(frame_rate),
      voice_(frame_rate, kMaxVoiceCount) {}

void InstrumentProcessor::Process(double* output_samples, int output_channel_count,
                                  int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentProcessor::SetControl(int id, double value) noexcept {
  switch (static_cast<InstrumentControl>(id)) {
    case InstrumentControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case InstrumentControl::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
    case InstrumentControl::kOscillatorType:
      voice_.Update([value](Voice& voice) noexcept {
        voice.oscillator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case InstrumentControl::kSamplePlayerLoop:
      voice_.Update([value](Voice& voice) noexcept {
        voice.sample_player().SetLoop(static_cast<bool>(value));
      });
      break;
    case InstrumentControl::kAttack:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetAttack(value); });
      break;
    case InstrumentControl::kDecay:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetDecay(value); });
      break;
    case InstrumentControl::kSustain:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetSustain(value); });
      break;
    case InstrumentControl::kRelease:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetRelease(value); });
      break;
    case InstrumentControl::kPitchShift:
      // TODO(#139): Simplify pitch shift.
      if (const double pitch_offset = value - pitch_shift_; pitch_offset != 0.0) {
        pitch_shift_ = value;
        voice_.Update([frequency_ratio = std::pow(2.0, pitch_offset)](Voice& voice) noexcept {
          if (voice.IsActive()) {
            voice.oscillator().SetFrequency(voice.oscillator().GetFrequency() * frequency_ratio);
            voice.sample_player().SetSpeed(voice.sample_player().GetSpeed() * frequency_ratio);
          }
        });
      }
      break;
    case InstrumentControl::kRetrigger:
      voice_.SetRetrigger(static_cast<bool>(value));
      break;
    default:
      assert(false);
      break;
  }
}

void InstrumentProcessor::SetData(const void* data, int size) noexcept {
  sample_data_.clear();
  voice_.Update([](Voice& voice) { voice.sample_player().SetData(nullptr, 0, 0); });

  const double* data_double = static_cast<const double*>(data);
  if (data_double == nullptr || size == 0) {
    return;
  }

  const int sample_data_count = static_cast<int>(*data_double++);
  sample_data_.reserve(sample_data_count);

  for (int i = 0; i < sample_data_count; ++i) {
    const double pitch = *data_double++;
    const int frame_rate = static_cast<int>(*data_double++);
    const int length = static_cast<int>(*data_double++);
    sample_data_.push_back({pitch, data_double, length, frame_rate});
    data_double += length;
  }

  // TODO(#139): Support data update for already playing voices.
}

void InstrumentProcessor::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void InstrumentProcessor::SetNoteOn(double pitch, double intensity) noexcept {
  const double frequency = GetFrequency(pitch + pitch_shift_, reference_frequency_);
  const SampleData* sample_data = SelectSampleData(pitch);
  const double speed = (sample_data != nullptr && pitch + pitch_shift_ != sample_data->pitch)
                           ? frequency / GetFrequency(sample_data->pitch, reference_frequency_)
                           : 1.0;
  voice_.Start(pitch, [&](Voice& voice) {
    voice.oscillator().SetFrequency(frequency);
    if (sample_data != nullptr) {
      voice.sample_player().SetData(sample_data->data, sample_data->frame_rate,
                                    sample_data->length);
      voice.sample_player().SetSpeed(speed);
    }
    voice.set_gain(intensity);
  });
}

const InstrumentProcessor::SampleData* InstrumentProcessor::SelectSampleData(
    double pitch) const noexcept {
  if (sample_data_.empty()) {
    return nullptr;
  }
  // TODO(#139): `std::upper_bound` turned out to be slow here, but this may be optimized further.
  for (int i = 0; i < static_cast<int>(sample_data_.size()); ++i) {
    if (const SampleData* current = &sample_data_[i]; pitch <= current->pitch) {
      return (i == 0 || pitch - sample_data_[i - 1].pitch > current->pitch - pitch)
                 ? current
                 : &sample_data_[i - 1];
    }
  }
  return &sample_data_.back();
}

}  // namespace barely
