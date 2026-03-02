#ifndef BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <unordered_map>

#include "core/constants.h"
#include "core/control.h"
#include "core/decibels.h"
#include "dsp/compressor.h"
#include "dsp/delay_filter.h"
#include "dsp/one_pole_filter.h"
#include "dsp/sidechain.h"
#include "engine/effect_params.h"
#include "engine/engine_state.h"
#include "engine/instrument_processor.h"
#include "engine/message.h"

namespace barely {

class EngineProcessor {
 public:
  explicit EngineProcessor(EngineState& engine) noexcept
      : engine_(engine), instrument_processor_(engine_) {}

  void Process(float* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept {
    assert(output_samples != nullptr);
    assert(output_channel_count > 0);
    assert(output_frame_count > 0);
    assert(output_frame_count <= BARELY_MAX_FRAME_COUNT);

    float temp_samples[kStereoChannelCount * BARELY_MAX_FRAME_COUNT];
    std::fill_n(temp_samples, kStereoChannelCount * output_frame_count, 0.0f);

    const int64_t process_frame = SecondsToFrames(engine_.sample_rate, timestamp);
    const int64_t end_frame = process_frame + output_frame_count;
    int current_frame = 0;

    // Process *all* messages before the end sample.
    for (auto* message = engine_.message_queue.GetNext(end_frame); message;
         message = engine_.message_queue.GetNext(end_frame)) {
      if (const int message_frame = static_cast<int>(message->first - process_frame);
          current_frame < message_frame) {
        ProcessSamples(&temp_samples[kStereoChannelCount * current_frame],
                       message_frame - current_frame);
        current_frame = message_frame;
      }
      ProcessMessage(message->second);
    }

    // Process the rest of the samples.
    if (current_frame < output_frame_count) {
      ProcessSamples(&temp_samples[kStereoChannelCount * current_frame],
                     output_frame_count - current_frame);
    }

    engine_.slice_pool.MarkSafeToRelease(end_frame);

    // Fill the output samples.
    if (output_channel_count > 1) {
      std::fill_n(output_samples, output_channel_count * output_frame_count, 0.0f);
      for (int frame = 0; frame < output_frame_count; ++frame) {
        output_samples[output_channel_count * frame] = temp_samples[kStereoChannelCount * frame];
        output_samples[output_channel_count * frame + 1] =
            temp_samples[kStereoChannelCount * frame + 1];
      }
    } else {  // downmix to mono.
      for (int frame = 0; frame < output_frame_count; ++frame) {
        output_samples[frame] = temp_samples[kStereoChannelCount * frame] +
                                temp_samples[kStereoChannelCount * frame + 1];
      }
    }
  }

  void SetControl(BarelyEngineControlType type, float value) noexcept {
    switch (type) {
      case BarelyEngineControlType_kCompMix:
        engine_.target_params.comp_params.mix = value;
        break;
      case BarelyEngineControlType_kCompAttack:
        engine_.comp.SetAttack(value, engine_.sample_rate);
        break;
      case BarelyEngineControlType_kCompRelease:
        engine_.comp.SetRelease(value, engine_.sample_rate);
        break;
      case BarelyEngineControlType_kCompThreshold:
        engine_.target_params.comp_params.threshold_db = AmplitudeToDecibels(value * value);
        break;
      case BarelyEngineControlType_kCompRatio:
        engine_.target_params.comp_params.SetRatio(value);
        break;
      case BarelyEngineControlType_kDelayMix:
        engine_.target_params.delay_params.mix = value;
        break;
      case BarelyEngineControlType_kDelayTime:
        engine_.target_params.delay_params.frame_count = std::min(
            std::max(value * engine_.sample_rate, 1.0f), static_cast<float>(kMaxDelayFrameCount));
        break;
      case BarelyEngineControlType_kDelayFeedback:
        engine_.target_params.delay_params.feedback = value * kMaxDelayFeedback;
        break;
      case BarelyEngineControlType_kDelayLpfCutoff:
        engine_.target_params.delay_params.lpf_coeff =
            GetFilterCoeff(engine_.sample_rate, GetFrequency(value, 0.5f * engine_.sample_rate));
        break;
      case BarelyEngineControlType_kDelayHpfCutoff:
        engine_.target_params.delay_params.hpf_coeff =
            GetFilterCoeff(engine_.sample_rate, GetFrequency(value, 0.5f * engine_.sample_rate));
        break;
      case BarelyEngineControlType_kDelayPingPong:
        engine_.target_params.delay_params.ping_pong = value;
        break;
      case BarelyEngineControlType_kDelayReverbSend:
        engine_.target_params.delay_params.reverb_send = value;
        break;
      case BarelyEngineControlType_kReverbMix:
        engine_.target_params.reverb_params.mix = value;
        break;
      case BarelyEngineControlType_kReverbDamping:
        engine_.target_params.reverb_params.damping_ratio = value * kMaxDampingRatio;
        break;
      case BarelyEngineControlType_kReverbRoomSize:
        engine_.target_params.reverb_params.SetFeedback(value);
        break;
      case BarelyEngineControlType_kReverbStereoWidth:
        engine_.target_params.reverb_params.width = value;
        break;
      case BarelyEngineControlType_kReverbFreeze:
        engine_.target_params.reverb_params.freeze = static_cast<bool>(value);
        break;
      case BarelyEngineControlType_kSidechainMix:
        engine_.target_params.sidechain_params.mix = value;
        break;
      case BarelyEngineControlType_kSidechainAttack:
        engine_.sidechain.SetAttack(value, engine_.sample_rate);
        break;
      case BarelyEngineControlType_kSidechainRelease:
        engine_.sidechain.SetRelease(value, engine_.sample_rate);
        break;
      case BarelyEngineControlType_kSidechainThreshold:
        engine_.target_params.sidechain_params.threshold_db = AmplitudeToDecibels(value * value);
        break;
      case BarelyEngineControlType_kSidechainRatio:
        engine_.target_params.sidechain_params.SetRatio(value);
        break;
      default:
        assert(!"Invalid engine control type");
        return;
    }
  }

 private:
  void ProcessMessage(Message& message) noexcept {
    std::visit(
        MessageVisitor{[this](EngineControlMessage& engine_control_message) noexcept {
                         SetControl(engine_control_message.type, engine_control_message.value);
                       },
                       [this](EngineSeedMessage& engine_seed_message) noexcept {
                         engine_.audio_rng.ResetSeed(engine_seed_message.seed);
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
                         instrument_processor_.SetNoteControl(note_control_message.note_index,
                                                              note_control_message.type,
                                                              note_control_message.value);
                       },
                       [this](NoteOffMessage& note_off_message) noexcept {
                         instrument_processor_.SetNoteOff(note_off_message.note_index);
                       },
                       [this](NoteOnMessage& note_on_message) noexcept {
                         instrument_processor_.SetNoteOn(note_on_message.note_index,
                                                         note_on_message.instrument_index,
                                                         note_on_message.pitch);
                       },
                       [this](SampleDataMessage& sample_data_message) noexcept {
                         instrument_processor_.SetSampleData(sample_data_message.instrument_index,
                                                             sample_data_message.first_slice_index);
                       }},
        message);
  }

  void ProcessSamples(float* output_samples, int output_frame_count) noexcept {
    for (int frame = 0; frame < output_frame_count; ++frame) {
      float delay_frame[kStereoChannelCount] = {};
      float reverb_frame[kStereoChannelCount] = {};
      float sidechain_frame[kStereoChannelCount] = {};
      float* output_frame = &output_samples[kStereoChannelCount * frame];

      instrument_processor_.ProcessAllVoices<true>(delay_frame, reverb_frame, sidechain_frame,
                                                   output_frame);
      engine_.sidechain.Process(sidechain_frame, engine_.current_params.sidechain_params);
      instrument_processor_.ProcessAllVoices<false>(delay_frame, reverb_frame, sidechain_frame,
                                                    output_frame);

      engine_.delay_filter.Process(delay_frame, reverb_frame, output_frame,
                                   engine_.current_params.delay_params);
      engine_.reverb.Process(reverb_frame, output_frame, engine_.current_params.reverb_params);

      engine_.comp.Process(output_frame, engine_.current_params.comp_params);

      engine_.Approach();
    }
  }

  EngineState& engine_;
  InstrumentProcessor instrument_processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_PROCESSOR_H_
