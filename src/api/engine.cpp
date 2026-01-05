#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>
#include <variant>

#include "api/performer.h"
#include "common/constants.h"
#include "common/time.h"
#include "dsp/control.h"
#include "dsp/voice_pool.h"
#include "engine/instrument_params.h"
#include "engine/instrument_processor.h"
#include "engine/message.h"

namespace {

using ::barely::Control;
using ::barely::EngineControlArray;
using ::barely::EngineControlMessage;
using ::barely::EngineControlType;
using ::barely::InstrumentControlMessage;
using ::barely::InstrumentControlType;
using ::barely::kStereoChannelCount;
using ::barely::MessageVisitor;
using ::barely::NoteControlMessage;
using ::barely::NoteControlType;
using ::barely::NoteOffMessage;
using ::barely::NoteOnMessage;
using ::barely::SampleDataMessage;

// Returns an engine control array.
EngineControlArray BuildEngineControlArray(float sample_rate) noexcept {
  return {
      Control(0.0f, 0.0f, 1.0f),                // kCompressorMix
      Control(0.0f, 0.0f, 10.0f),               // kCompressorAttack
      Control(0.0f, 0.0f, 10.0f),               // kCompressorRelease
      Control(1.0f, 0.0f, 1.0f),                // kCompressorThreshold
      Control(1.0f, 1.0f, 64.0f),               // kCompressorRatio
      Control(1.0f, 0.0f, 1.0f),                // kDelayMix
      Control(0.0f, 0.0f, 10.0f),               // kDelayTime
      Control(0.0f, 0.0f, 1.0f),                // kDelayFeedback
      Control(sample_rate, 0.0f, sample_rate),  // kDelayLowPassFrequency
      Control(0.0f, 0.0f, sample_rate),         // kDelayHighPassFrequency
      Control(1.0f, 0.0f, 1.0f),                // kSidechainMix
      Control(0.0f, 0.0f, 10.0f),               // kSidechainAttack
      Control(0.0f, 0.0f, 10.0f),               // kSidechainRelease
      Control(1.0f, 0.0f, 1.0f),                // kSidechainThreshold
      Control(1.0f, 1.0f, 64.0f),               // kSidechainRatio
  };
}

void SetNoteControl(const barely::InstrumentParams& params, barely::VoicePool& voice_pool,
                    float pitch, NoteControlType type, float value) noexcept {
  switch (type) {
    case NoteControlType::kGain: {
      uint32_t voice_index = params.first_voice_index;
      while (voice_index != 0) {
        auto& voice = voice_pool.Get(voice_index);
        if (voice.pitch == pitch && voice.IsOn()) {
          voice.note_params.gain = value;
          break;
        }
        voice_index = voice.next_voice_index;
      }
    } break;
    case NoteControlType::kPitchShift: {
      uint32_t voice_index = params.first_voice_index;
      while (voice_index != 0) {
        auto& voice = voice_pool.Get(voice_index);
        if (voice.pitch == pitch && voice.IsOn()) {
          voice.pitch_shift = value;
          voice.UpdatePitchIncrements();
          break;
        }
        voice_index = voice.next_voice_index;
      }
    } break;
    default:
      assert(!"Invalid note control type");
      break;
  }
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyEngine::BarelyEngine(int sample_rate, int max_frame_count) noexcept
    : controls_(BuildEngineControlArray(static_cast<float>(sample_rate))),
      engine_processor_(sample_rate),
      output_samples_(kStereoChannelCount * max_frame_count),
      sample_rate_(sample_rate),
      sample_interval_(1.0f / static_cast<float>(sample_rate)) {
  assert(sample_rate >= 0);
  assert(max_frame_count > 0);
}

float BarelyEngine::GetControl(BarelyEngineControlType type) const noexcept {
  return controls_[type].value;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Process(float* output_samples, int output_channel_count, int output_frame_count,
                           double timestamp) noexcept {
  assert(output_samples != nullptr);
  assert(output_channel_count > 0);
  assert(output_frame_count > 0);

  std::fill_n(output_samples_.begin(), kStereoChannelCount * output_frame_count, 0.0f);

  const int64_t process_frame = barely::SecondsToFrames(sample_rate_, timestamp);
  const int64_t end_frame = process_frame + output_frame_count;
  int current_frame = 0;

  // Process *all* messages before the end sample.
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        current_frame < message_frame) {
      engine_processor_.Process(audio_rng_, params_array_, voice_pool_,
                                &output_samples_[kStereoChannelCount * current_frame],
                                message_frame - current_frame);
      current_frame = message_frame;
    }
    std::visit(
        MessageVisitor{
            [this](EngineControlMessage& engine_control_message) noexcept {
              engine_processor_.SetControl(engine_control_message.type,
                                           engine_control_message.value);
            },
            [this](InstrumentControlMessage& instrument_control_message) noexcept {
              auto& params = params_array_[instrument_control_message.instrument_index];
              if (instrument_control_message.type == InstrumentControlType::kVoiceCount) {
                const uint32_t new_voice_count =
                    static_cast<uint32_t>(instrument_control_message.value);
                uint32_t active_voice_count = 0;
                uint32_t active_voice_index = params.first_voice_index;
                while (active_voice_index != 0 && active_voice_count <= new_voice_count) {
                  active_voice_index = voice_pool_.Get(active_voice_index).next_voice_index;
                  ++active_voice_count;
                }
                while (active_voice_index != 0) {
                  auto& voice = voice_pool_.Get(active_voice_index);
                  if (voice.previous_voice_index != 0) {
                    voice_pool_.Get(voice.previous_voice_index).next_voice_index = 0;
                    voice.previous_voice_index = 0;
                  }
                  voice_pool_.Release(active_voice_index);
                  active_voice_index = voice.next_voice_index;
                  voice.next_voice_index = 0;
                }
              }
              SetInstrumentControl(params, sample_interval_, instrument_control_message.type,
                                   instrument_control_message.value);
            },
            [this](NoteControlMessage& note_control_message) noexcept {
              SetNoteControl(params_array_[note_control_message.instrument_index], voice_pool_,
                             note_control_message.pitch, note_control_message.type,
                             note_control_message.value);
            },
            [this](NoteOffMessage& note_off_message) noexcept {
              auto& params = params_array_[note_off_message.instrument_index];
              uint32_t active_voice_index = params.first_voice_index;
              while (active_voice_index != 0) {
                auto& voice = voice_pool_.Get(active_voice_index);
                if (voice.pitch == note_off_message.pitch && voice.IsOn() &&
                    (params.sample_data.empty() || params.slice_mode != barely::SliceMode::kOnce)) {
                  voice.Stop();
                  break;
                }
                active_voice_index = voice.next_voice_index;
              }
            },
            [this](NoteOnMessage& note_on_message) noexcept {
              auto& params = params_array_[note_on_message.instrument_index];
              if (auto* voice = AcquireVoice(voice_pool_, params, note_on_message.pitch);
                  voice != nullptr) {
                voice->Start(params, note_on_message.instrument_index,
                             params.sample_data.Select(note_on_message.pitch, audio_rng_),
                             note_on_message.pitch, note_on_message.controls);
              }
            },
            [this](SampleDataMessage& sample_data_message) noexcept {
              auto& params = params_array_[sample_data_message.instrument_index];
              params.sample_data.Swap(sample_data_message.sample_data);
              uint32_t active_voice_index = params.first_voice_index;
              while (active_voice_index != 0) {
                auto& voice = voice_pool_.Get(active_voice_index);
                voice.slice = params.sample_data.Select(voice.pitch, audio_rng_);
                voice.UpdatePitchIncrements();
                active_voice_index = voice.next_voice_index;
              }
            }},
        message->second);
  }

  // Process the rest of the samples.
  if (current_frame < output_frame_count) {
    engine_processor_.Process(audio_rng_, params_array_, voice_pool_,
                              &output_samples_[kStereoChannelCount * current_frame],
                              output_frame_count - current_frame);
  }

  // Fill the output samples.
  if (output_channel_count > 1) {
    std::fill_n(output_samples, output_channel_count * output_frame_count, 0.0f);
    for (int frame = 0; frame < output_frame_count; ++frame) {
      output_samples[output_channel_count * frame] = output_samples_[kStereoChannelCount * frame];
      output_samples[output_channel_count * frame + 1] =
          output_samples_[kStereoChannelCount * frame + 1];
    }
  } else {  // downmix to mono.
    for (int frame = 0; frame < output_frame_count; ++frame) {
      output_samples[frame] = output_samples_[kStereoChannelCount * frame] +
                              output_samples_[kStereoChannelCount * frame + 1];
    }
  }
}

void BarelyEngine::ScheduleMessage(barely::Message message) noexcept {
  message_queue_.Add(update_frame_, std::move(message));
}

void BarelyEngine::SetControl(BarelyEngineControlType type, float value) noexcept {
  if (auto& control = controls_[type]; control.SetValue(value)) {
    ScheduleMessage(EngineControlMessage{static_cast<EngineControlType>(type), control.value});
  }
}

void BarelyEngine::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      BarelyPerformer::TaskKey next_key = {barely::SecondsToBeats(tempo_, timestamp - timestamp_),
                                           std::numeric_limits<int>::min()};
      bool has_tasks_to_process = false;
      for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
        if (const auto maybe_next_key = performer_pool_.GetActive(i).GetNextTaskKey();
            maybe_next_key.has_value() && *maybe_next_key < next_key) {
          has_tasks_to_process = true;
          next_key = *maybe_next_key;
        }
      }
      for (uint32_t i = 0; i < instrument_pool_.GetActiveCount(); ++i) {
        auto& instrument = instrument_pool_.GetActive(i);
        if (!instrument.IsArpEnabled() || instrument.pitches.empty()) {
          continue;
        }
        if (const auto maybe_next_duration = instrument.arp.GetNextDuration(
                static_cast<double>(
                    instrument.controls[BarelyInstrumentControlType_kArpRate].value),
                static_cast<double>(
                    instrument.controls[BarelyInstrumentControlType_kArpGateRatio].value));
            maybe_next_duration.has_value() && *maybe_next_duration < next_key.first) {
          has_tasks_to_process = true;
          next_key = {*maybe_next_duration, std::numeric_limits<int>::max()};
        }
      }

      const auto& [update_duration, max_priority] = next_key;
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
          performer_pool_.GetActive(i).Update(update_duration);
        }
        for (uint32_t i = 0; i < instrument_pool_.GetActiveCount(); ++i) {
          instrument_pool_.GetActive(i).Update(update_duration);
        }

        timestamp_ += barely::BeatsToSeconds(tempo_, update_duration);
        update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
      }

      if (has_tasks_to_process) {
        for (uint32_t i = 0; i < performer_pool_.GetActiveCount(); ++i) {
          performer_pool_.GetActive(i).ProcessAllTasksAtPosition(max_priority);
        }
        if (max_priority == std::numeric_limits<int>::max()) {
          for (uint32_t i = 0; i < instrument_pool_.GetActiveCount(); ++i) {
            ProcessArp(instrument_pool_.GetActiveIndex(i));
          }
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
    }
  }
}

void BarelyEngine::ProcessArp(uint32_t instrument_index) noexcept {
  auto& instrument = instrument_pool_.Get(instrument_index);
  if (!instrument.IsArpEnabled() || instrument.pitches.empty()) {
    return;
  }
  if (!instrument.arp.is_active && instrument.arp.phase == 0.0) {
    const int size = static_cast<int>(instrument.pitches.size());
    switch (static_cast<BarelyArpMode>(
        instrument.controls[BarelyInstrumentControlType_kArpMode].value)) {
      case BarelyArpMode_kUp:
        instrument.arp.pitch_index = (instrument.arp.pitch_index + 1) % size;
        break;
      case BarelyArpMode_kDown:
        instrument.arp.pitch_index = (instrument.arp.pitch_index == -1)
                                         ? size - 1
                                         : (instrument.arp.pitch_index + size - 1) % size;
        break;
      case BarelyArpMode_kRandom:
        instrument.arp.pitch_index = main_rng_.Generate(0, size);
        break;
      default:
        assert(!"Invalid arpeggiator mode");
        return;
    }
    assert(instrument.arp.pitch_index >= 0 && instrument.arp.pitch_index < size);
    instrument.arp.pitch = instrument.pitches[instrument.arp.pitch_index];

    instrument.arp.is_active = true;
    instrument.note_event_callback(BarelyNoteEventType_kBegin, instrument.arp.pitch);
    ScheduleMessage(barely::NoteOnMessage{
        instrument_index, instrument.arp.pitch,
        barely::BuildNoteControls(instrument.note_controls.at(instrument.arp.pitch))});
  } else if (instrument.arp.is_active &&
             instrument.arp.phase ==
                 static_cast<double>(
                     instrument.controls[BarelyInstrumentControlType_kArpGateRatio].value)) {
    instrument.arp.is_active = false;
    instrument.note_event_callback(BarelyNoteEventType_kEnd, instrument.arp.pitch);
    ScheduleMessage(barely::NoteOffMessage{instrument_index, instrument.arp.pitch});
  }
}
