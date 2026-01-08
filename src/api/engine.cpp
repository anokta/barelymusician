#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
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

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyEngine::BarelyEngine(int sample_rate) noexcept
    : state_(sample_rate),
      instrument_controller_(state_),
      performer_controller_(state_),
      processor_(state_) {
  assert(sample_rate >= 0);
}

float BarelyEngine::GetControl(BarelyEngineControlType type) const noexcept {
  return state_.controls[type].value;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Process(float* output_samples, int output_channel_count, int output_frame_count,
                           double timestamp) noexcept {
  assert(output_samples != nullptr);
  assert(output_channel_count > 0);
  assert(output_frame_count > 0);
  assert(output_frame_count <= BARELYMUSICIAN_MAX_FRAME_COUNT);

  float temp_samples[kStereoChannelCount * BARELYMUSICIAN_MAX_FRAME_COUNT];
  std::fill_n(temp_samples, kStereoChannelCount * output_frame_count, 0.0f);

  const int64_t process_frame = barely::SecondsToFrames(state_.sample_rate, timestamp);
  const int64_t end_frame = process_frame + output_frame_count;
  int current_frame = 0;

  // Process *all* messages before the end sample.
  for (auto* message = state_.message_queue.GetNext(end_frame); message;
       message = state_.message_queue.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        current_frame < message_frame) {
      processor_.Process(&temp_samples[kStereoChannelCount * current_frame],
                         message_frame - current_frame);
      current_frame = message_frame;
    }
    processor_.ProcessMessage(message->second);
  }

  // Process the rest of the samples.
  if (current_frame < output_frame_count) {
    processor_.Process(&temp_samples[kStereoChannelCount * current_frame],
                       output_frame_count - current_frame);
  }

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
      output_samples[frame] =
          temp_samples[kStereoChannelCount * frame] + temp_samples[kStereoChannelCount * frame + 1];
    }
  }
}

void BarelyEngine::SetControl(BarelyEngineControlType type, float value) noexcept {
  if (auto& control = state_.controls[type]; control.SetValue(value)) {
    ScheduleMessage(EngineControlMessage{type, control.value});
  }
}

void BarelyEngine::SetTempo(double tempo) noexcept { state_.tempo = std::max(tempo, 0.0); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Update(double timestamp) noexcept {
  while (state_.timestamp < timestamp) {
    if (state_.tempo > 0.0) {
      const double update_duration =
          barely::SecondsToBeats(state_.tempo, timestamp - state_.timestamp);

      auto next_key = performer_controller_.GetNextTaskKey(update_duration);
      if (const double next_duration = instrument_controller_.GetNextDuration();
          next_duration < next_key.first) {
        next_key = {next_duration, std::numeric_limits<int>::max()};
      }
      const auto& [next_update_duration, max_priority] = next_key;

      if (next_update_duration > 0) {
        performer_controller_.Update(next_update_duration);
        instrument_controller_.Update(next_update_duration);

        state_.timestamp += barely::BeatsToSeconds(state_.tempo, next_update_duration);
        state_.update_frame = barely::SecondsToFrames(state_.sample_rate, state_.timestamp);
      }

      if (next_update_duration < update_duration) {
        performer_controller_.ProcessAllTasksAtPosition(max_priority);
        if (max_priority == std::numeric_limits<int>::max()) {
          instrument_controller_.ProcessArp(state_.main_rng);
        }
      }
    } else if (state_.timestamp < timestamp) {
      state_.timestamp = timestamp;
      state_.update_frame = barely::SecondsToFrames(state_.sample_rate, state_.timestamp);
    }
  }
}
