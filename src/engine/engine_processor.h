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
#include "engine/engine_state.h"
#include "engine/instrument_processor.h"
#include "engine/message.h"

namespace barely {

/// Class that wraps the audio processing of an effect.
class EngineProcessor {
 public:
  explicit EngineProcessor(EngineState& engine) noexcept
      : engine_(engine), instrument_processor_(engine_) {}

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
      engine_.sidechain.Process(sidechain_frame, engine_.current_params.sidechain_mix,
                                engine_.current_params.sidechain_threshold_db,
                                engine_.current_params.sidechain_ratio);
      instrument_processor_.ProcessAllVoices<false>(delay_frame, sidechain_frame, output_frame);

      engine_.delay_filter.Process(delay_frame, output_frame, engine_.current_params.delay_params);

      engine_.compressor.Process(output_frame, engine_.current_params.compressor_params);

      engine_.Approach();
    }
  }

  void ProcessMessage(Message& message) noexcept {
    std::visit(
        MessageVisitor{[this](EngineControlMessage& engine_control_message) noexcept {
                         SetControl(engine_control_message.type, engine_control_message.value);
                       },
                       [this](EngineSeedMessage& engine_seed_message) noexcept {
                         engine_.audio_rng.SetSeed(engine_seed_message.seed);
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
        engine_.target_params.compressor_params.mix = value;
        break;
      case BarelyEngineControlType_kCompressorAttack:
        engine_.compressor.SetAttack(value, engine_.sample_interval);
        break;
      case BarelyEngineControlType_kCompressorRelease:
        engine_.compressor.SetRelease(value, engine_.sample_interval);
        break;
      case BarelyEngineControlType_kCompressorThreshold:
        engine_.target_params.compressor_params.threshold_db = AmplitudeToDecibels(value);
        break;
      case BarelyEngineControlType_kCompressorRatio:
        engine_.target_params.compressor_params.ratio = value;
        break;
      case BarelyEngineControlType_kDelayMix:
        engine_.target_params.delay_params.mix = value;
        break;
      case BarelyEngineControlType_kDelayTime:
        engine_.target_params.delay_params.frame_count =
            std::min(value * static_cast<float>(engine_.sample_rate),
                     static_cast<float>(kMaxDelayFrameCount));
        break;
      case BarelyEngineControlType_kDelayFeedback:
        engine_.target_params.delay_params.feedback = value;
        break;
      case BarelyEngineControlType_kDelayLowPassFrequency:
        engine_.target_params.delay_params.low_pass_coeff =
            GetFilterCoefficient(engine_.sample_rate, value);
        break;
      case BarelyEngineControlType_kDelayHighPassFrequency:
        engine_.target_params.delay_params.high_pass_coeff =
            GetFilterCoefficient(engine_.sample_rate, value);
        break;
      case BarelyEngineControlType_kSidechainMix:
        engine_.target_params.sidechain_mix = value;
        break;
      case BarelyEngineControlType_kSidechainAttack:
        engine_.sidechain.SetAttack(value, engine_.sample_interval);
        break;
      case BarelyEngineControlType_kSidechainRelease:
        engine_.sidechain.SetRelease(value, engine_.sample_interval);
        break;
      case BarelyEngineControlType_kSidechainThreshold:
        engine_.target_params.sidechain_threshold_db = AmplitudeToDecibels(value);
        break;
      case BarelyEngineControlType_kSidechainRatio:
        engine_.target_params.sidechain_ratio = value;
        break;
      default:
        assert(!"Invalid engine control type");
        return;
    }
  }

 private:
  EngineState& engine_;
  InstrumentProcessor instrument_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_
