#ifndef BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <unordered_map>

#include "core/constants.h"
#include "dsp/compressor.h"
#include "dsp/control.h"
#include "dsp/decibels.h"
#include "dsp/delay_filter.h"
#include "dsp/one_pole_filter.h"
#include "dsp/sidechain.h"
#include "engine/effect_params.h"
#include "engine/instrument_processor.h"
#include "engine/message.h"

namespace barely {

/// Class that wraps the audio processing of an effect.
class EngineProcessor {
 public:
  /// Constructs a new `EngineProcessor`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit EngineProcessor(int sample_rate) noexcept
      : sample_rate_(sample_rate),
        sample_interval_(1.0f / static_cast<float>(sample_rate)),
        instrument_processor_(rng_, sample_interval_) {
    assert(sample_rate > 0);
  }

  /// Processes the next output samples.
  ///
  /// @param rng Random number generator.
  /// @param output_samples Array of interleaved output samples.
  /// @param output_frame_count Number of output frames.
  void Process(float* output_samples, int output_frame_count) noexcept {
    for (int frame = 0; frame < output_frame_count; ++frame) {
      float delay_frame[kStereoChannelCount] = {};
      float sidechain_frame[kStereoChannelCount] = {};
      float* output_frame = &output_samples[kStereoChannelCount * frame];

      instrument_processor_.ProcessAllVoices<true>(delay_frame, sidechain_frame, output_frame);
      sidechain_.Process(sidechain_frame, current_params_.sidechain_mix,
                         current_params_.sidechain_threshold_db, current_params_.sidechain_ratio);
      instrument_processor_.ProcessAllVoices<false>(delay_frame, sidechain_frame, output_frame);

      delay_filter_.Process(delay_frame, output_frame, current_params_.delay_params);

      compressor_.Process(output_frame, current_params_.compressor_params);

      Approach();
    }
  }

  void ProcessMessage(Message& message) noexcept {
    std::visit(
        MessageVisitor{[this](EngineControlMessage& engine_control_message) noexcept {
                         SetControl(engine_control_message.type, engine_control_message.value);
                       },
                       [this](EngineSeedMessage& engine_seed_message) noexcept {
                         rng_.SetSeed(engine_seed_message.seed);
                       },
                       [this](InstrumentCreateMessage& instrument_create_message) noexcept {
                         instrument_processor_.Init(instrument_create_message.instrument_index);
                       },
                       [this](InstrumentControlMessage& instrument_control_message) noexcept {
                         instrument_processor_.SetControl(
                             instrument_control_message.instrument_index,
                             instrument_control_message.type, instrument_control_message.value);
                       },
                       [this](NoteControlMessage& note_control_message) noexcept {
                         instrument_processor_.SetNoteControl(
                             note_control_message.instrument_index, note_control_message.pitch,
                             note_control_message.type, note_control_message.value);
                       },
                       [this](NoteOffMessage& note_off_message) noexcept {
                         instrument_processor_.SetNoteOff(note_off_message.instrument_index,
                                                          note_off_message.pitch);
                       },
                       [this](NoteOnMessage& note_on_message) noexcept {
                         instrument_processor_.SetNoteOn(note_on_message.instrument_index,
                                                         note_on_message.pitch,
                                                         note_on_message.controls);
                       },
                       [this](SampleDataMessage& sample_data_message) noexcept {
                         instrument_processor_.SetSampleData(sample_data_message.instrument_index,
                                                             sample_data_message.sample_data);
                       }},
        message);
  }

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetControl(BarelyEngineControlType type, float value) noexcept {
    switch (type) {
      case BarelyEngineControlType_kCompressorMix:
        target_params_.compressor_params.mix = value;
        break;
      case BarelyEngineControlType_kCompressorAttack:
        compressor_.SetAttack(value, sample_interval_);
        break;
      case BarelyEngineControlType_kCompressorRelease:
        compressor_.SetRelease(value, sample_interval_);
        break;
      case BarelyEngineControlType_kCompressorThreshold:
        target_params_.compressor_params.threshold_db = AmplitudeToDecibels(value);
        break;
      case BarelyEngineControlType_kCompressorRatio:
        target_params_.compressor_params.ratio = value;
        break;
      case BarelyEngineControlType_kDelayMix:
        target_params_.delay_params.mix = value;
        break;
      case BarelyEngineControlType_kDelayTime:
        target_params_.delay_params.frame_count = std::min(value * static_cast<float>(sample_rate_),
                                                           static_cast<float>(kMaxDelayFrameCount));
        break;
      case BarelyEngineControlType_kDelayFeedback:
        target_params_.delay_params.feedback = value;
        break;
      case BarelyEngineControlType_kDelayLowPassFrequency:
        target_params_.delay_params.low_pass_coeff = GetFilterCoefficient(sample_rate_, value);
        break;
      case BarelyEngineControlType_kDelayHighPassFrequency:
        target_params_.delay_params.high_pass_coeff = GetFilterCoefficient(sample_rate_, value);
        break;
      case BarelyEngineControlType_kSidechainMix:
        target_params_.sidechain_mix = value;
        break;
      case BarelyEngineControlType_kSidechainAttack:
        sidechain_.SetAttack(value, sample_interval_);
        break;
      case BarelyEngineControlType_kSidechainRelease:
        sidechain_.SetRelease(value, sample_interval_);
        break;
      case BarelyEngineControlType_kSidechainThreshold:
        target_params_.sidechain_threshold_db = AmplitudeToDecibels(value);
        break;
      case BarelyEngineControlType_kSidechainRatio:
        target_params_.sidechain_ratio = value;
        break;
      default:
        assert(!"Invalid engine control type");
        return;
    }
  }

 private:
  // Approaches parameters.
  void Approach() noexcept {
    current_params_.compressor_params.Approach(target_params_.compressor_params);
    current_params_.delay_params.Approach(target_params_.delay_params);
    ApproachValue(current_params_.sidechain_mix, target_params_.sidechain_mix);
    ApproachValue(current_params_.sidechain_threshold_db, target_params_.sidechain_threshold_db);
    ApproachValue(current_params_.sidechain_ratio, target_params_.sidechain_ratio);
  }

  // Random number generator for the audio thread.
  AudioRng rng_;

  // Compressor.
  Compressor compressor_;

  // Delay filter.
  DelayFilter delay_filter_;

  // Sidechain.
  Sidechain sidechain_;

  // Current parameters.
  EffectParams current_params_ = {};

  // Target parameters.
  EffectParams target_params_ = {};

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Sampling interval in seconds.
  float sample_interval_ = 0.0f;

  // Instrument processor.
  InstrumentProcessor instrument_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_
