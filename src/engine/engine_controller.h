#ifndef BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
#define BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_

#include <cassert>
#include <cstdint>

#include "core/constants.h"
#include "core/time.h"
#include "engine/engine_processor.h"
#include "engine/engine_state.h"
#include "engine/instrument_controller.h"
#include "engine/performer_controller.h"

namespace barely {

class EngineController {
 public:
  EngineController(EngineState& engine) noexcept : engine_(engine), processor_(engine_) {}

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept {
    assert(output_samples != nullptr);
    assert(output_channel_count > 0);
    assert(output_frame_count > 0);
    assert(output_frame_count <= BARELYMUSICIAN_MAX_FRAME_COUNT);

    float temp_samples[kStereoChannelCount * BARELYMUSICIAN_MAX_FRAME_COUNT];
    std::fill_n(temp_samples, kStereoChannelCount * output_frame_count, 0.0f);

    const int64_t process_frame = SecondsToFrames(engine_.sample_rate, timestamp);
    const int64_t end_frame = process_frame + output_frame_count;
    int current_frame = 0;

    // Process *all* messages before the end sample.
    for (auto* message = engine_.message_queue.GetNext(end_frame); message;
         message = engine_.message_queue.GetNext(end_frame)) {
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
        output_samples[frame] = temp_samples[kStereoChannelCount * frame] +
                                temp_samples[kStereoChannelCount * frame + 1];
      }
    }
  }

  /// Updates the engine at timestamp.
  ///
  /// @param instrument_controller Instrument controller.
  /// @param performer_controller Performer controller.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(InstrumentController& instrument_controller,
              PerformerController& performer_controller, double timestamp) noexcept {
    while (engine_.timestamp < timestamp) {
      if (engine_.tempo > 0.0) {
        const double update_duration = SecondsToBeats(engine_.tempo, timestamp - engine_.timestamp);

        auto next_key = performer_controller.GetNextTaskKey(update_duration);
        if (const double next_duration = instrument_controller.GetNextDuration();
            next_duration < next_key.first) {
          next_key = {next_duration, INT32_MAX};
        }
        const auto& [next_update_duration, max_priority] = next_key;

        if (next_update_duration > 0) {
          performer_controller.Update(next_update_duration);
          instrument_controller.Update(next_update_duration);

          engine_.timestamp += BeatsToSeconds(engine_.tempo, next_update_duration);
          engine_.update_frame = SecondsToFrames(engine_.sample_rate, engine_.timestamp);
        }

        if (next_update_duration < update_duration) {
          performer_controller.ProcessAllTasksAtPosition(max_priority);
          if (max_priority == INT32_MAX) {
            instrument_controller.ProcessArp(engine_.main_rng);
          }
        }
      } else if (engine_.timestamp < timestamp) {
        engine_.timestamp = timestamp;
        engine_.update_frame = SecondsToFrames(engine_.sample_rate, engine_.timestamp);
      }
    }
  }

 private:
  EngineState& engine_;
  EngineProcessor processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_ENGINE_CONTROLLER_H_
