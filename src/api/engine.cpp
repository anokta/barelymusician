#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <utility>
#include <variant>

#include "core/constants.h"
#include "core/time.h"
#include "dsp/control.h"
#include "engine/instrument_params.h"
#include "engine/instrument_processor.h"
#include "engine/message.h"
#include "engine/performer_state.h"

namespace {

using ::barely::Control;
using ::barely::EngineControlArray;
using ::barely::EngineControlMessage;
using ::barely::EngineControlType;
using ::barely::InstrumentControlMessage;
using ::barely::InstrumentControlType;
using ::barely::InstrumentCreateMessage;
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

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyEngine::BarelyEngine(int sample_rate, int max_frame_count) noexcept
    : controls_(BuildEngineControlArray(static_cast<float>(sample_rate))),
      processor_(sample_rate),
      output_samples_(kStereoChannelCount * max_frame_count),
      sample_rate_(sample_rate),
      instrument_controller_(message_queue_, update_frame_) {
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
      processor_.Process(&output_samples_[kStereoChannelCount * current_frame],
                         message_frame - current_frame);
      current_frame = message_frame;
    }
    processor_.ProcessMessage(message->second);
  }

  // Process the rest of the samples.
  if (current_frame < output_frame_count) {
    processor_.Process(&output_samples_[kStereoChannelCount * current_frame],
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
    ScheduleMessage(EngineControlMessage{type, control.value});
  }
}

void BarelyEngine::SetTempo(double tempo) noexcept { tempo_ = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Update(double timestamp) noexcept {
  while (timestamp_ < timestamp) {
    if (tempo_ > 0.0) {
      const double update_duration = barely::SecondsToBeats(tempo_, timestamp - timestamp_);

      auto next_key = performer_controller_.GetNextTaskKey(update_duration);
      if (const double next_duration = instrument_controller_.GetNextDuration();
          next_duration < next_key.first) {
        next_key = {next_duration, std::numeric_limits<int>::max()};
      }
      const auto& [next_update_duration, max_priority] = next_key;

      if (next_update_duration > 0) {
        performer_controller_.Update(next_update_duration);
        instrument_controller_.Update(next_update_duration);

        timestamp_ += barely::BeatsToSeconds(tempo_, next_update_duration);
        update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
      }

      if (next_update_duration < update_duration) {
        performer_controller_.ProcessAllTasksAtPosition(max_priority);
        if (max_priority == std::numeric_limits<int>::max()) {
          instrument_controller_.ProcessArp(main_rng_);
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
    }
  }
}
