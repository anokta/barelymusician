#include "api/engine.h"

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <span>
#include <unordered_map>
#include <utility>
#include <variant>

#include "api/instrument.h"
#include "api/performer.h"
#include "common/time.h"
#include "dsp/control.h"
#include "dsp/instrument_processor.h"
#include "dsp/message.h"

namespace {

using ::barely::Control;
using ::barely::ControlMessage;
using ::barely::EffectControlArray;
using ::barely::EffectControlMessage;
using ::barely::EffectControlType;
using ::barely::MessageVisitor;
using ::barely::NoteControlMessage;
using ::barely::NoteOffMessage;
using ::barely::NoteOnMessage;
using ::barely::SampleDataMessage;

// Returns an effect control array.
EffectControlArray BuildEffectControlArray() noexcept {
  return {
      Control(1.0f, 0.0f, 1.0f),   // kDelayMix
      Control(0.0f, 0.0f, 10.0f),  // kDelayTime
      Control(0.0f, 0.0f, 1.0f),   // kDelayFeedback
      Control(1.0f, 0.0f, 1.0f),   // kSidechainMix
      Control(0.0f, 0.0f, 10.0f),  // kSidechainAttack
      Control(0.0f, 0.0f, 10.0f),  // kSidechainRelease
      Control(1.0f, 0.0f, 1.0f),   // kSidechainThreshold
      Control(1.0f, 1.0f, 64.0f),  // kSidechainRatio
  };
}

// Builds a new mutable instrument map.
// NOLINTNEXTLINE(bugprone-exception-escape)
std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor*> BuildMutableInstrumentMap(
    const std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor>& instruments,
    BarelyInstrument* excluded_instrument = nullptr) noexcept {
  std::unordered_map<BarelyInstrument*, barely::InstrumentProcessor*> new_instruments;
  new_instruments.reserve(instruments.size());
  for (const auto& [instrument, processor] : instruments) {
    if (instrument != excluded_instrument) {
      new_instruments.emplace(instrument, const_cast<barely::InstrumentProcessor*>(&processor));
    }
  }
  return new_instruments;
}

}  // namespace

// TODO(#146): `max_frame_count` is currently not used, but likely needed for reverb implementation.
// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyEngine::BarelyEngine(int sample_rate, int channel_count, [[maybe_unused]] int max_frame_count,
                           float reference_frequency) noexcept
    : sample_rate_(sample_rate),
      channel_count_(channel_count),
      reference_frequency_(reference_frequency),
      effect_controls_(BuildEffectControlArray()),
      engine_processor_(sample_rate_) {
  assert(sample_rate >= 0);
  assert(channel_count == kChannelCount);
  assert(max_frame_count > 0);
  assert(reference_frequency >= 0.0f);
}

BarelyEngine::~BarelyEngine() noexcept { mutable_instruments_.Update({}); }

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::AddInstrument(
    BarelyInstrumentHandle instrument,
    std::span<const BarelyControlOverride> control_overrides) noexcept {
  [[maybe_unused]] const bool success =
      instruments_
          .emplace(instrument, barely::InstrumentProcessor(control_overrides, audio_rng_,
                                                           sample_rate_, reference_frequency_))
          .second;
  assert(success);
  mutable_instruments_.Update(BuildMutableInstrumentMap(instruments_));
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::AddPerformer(BarelyPerformer* performer) noexcept {
  [[maybe_unused]] const bool success = performers_.emplace(performer).second;
  assert(success);
}

float BarelyEngine::GetEffectControl(BarelyEffectControlType type) const noexcept {
  return effect_controls_[type].value;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::Process(float* output_samples, int output_frame_count,
                           double timestamp) noexcept {
  assert(output_samples != nullptr);
  assert(output_frame_count > 0);

  std::fill_n(output_samples, channel_count_ * output_frame_count, 0.0f);

  const int64_t process_frame = barely::SecondsToFrames(sample_rate_, timestamp);
  const int64_t end_frame = process_frame + output_frame_count;
  int current_frame = 0;

  auto instruments = mutable_instruments_.GetScopedView();

  // Process *all* messages before the end sample.
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        current_frame < message_frame) {
      engine_processor_.Process(*instruments, &output_samples[channel_count_ * current_frame],
                                message_frame - current_frame);
      current_frame = message_frame;
    }
    std::visit(
        MessageVisitor{[&instruments](ControlMessage& control_message) noexcept {
                         // TODO(#126): This shouldn't need an instrument look up and can directly
                         // pass the processor pointer here and below once memory is fixed.
                         if (const auto it = instruments->find(control_message.instrument);
                             it != instruments->end()) {
                           it->second->SetControl(control_message.type, control_message.value);
                         }
                       },
                       [this](EffectControlMessage& effect_control_message) noexcept {
                         engine_processor_.SetControl(effect_control_message.type,
                                                      effect_control_message.value);
                       },
                       [&instruments](NoteControlMessage& note_control_message) noexcept {
                         if (const auto it = instruments->find(note_control_message.instrument);
                             it != instruments->end()) {
                           it->second->SetNoteControl(note_control_message.pitch,
                                                      note_control_message.type,
                                                      note_control_message.value);
                         }
                       },
                       [&instruments](NoteOffMessage& note_off_message) noexcept {
                         if (const auto it = instruments->find(note_off_message.instrument);
                             it != instruments->end()) {
                           it->second->SetNoteOff(note_off_message.pitch);
                         }
                       },
                       [&instruments](NoteOnMessage& note_on_message) noexcept {
                         if (const auto it = instruments->find(note_on_message.instrument);
                             it != instruments->end()) {
                           it->second->SetNoteOn(note_on_message.pitch, note_on_message.controls);
                         }
                       },
                       [&instruments](SampleDataMessage& sample_data_message) noexcept {
                         if (const auto it = instruments->find(sample_data_message.instrument);
                             it != instruments->end()) {
                           it->second->SetSampleData(sample_data_message.sample_data);
                         }
                       }},
        message->second);
  }

  // Process the rest of the samples.
  if (current_frame < output_frame_count) {
    engine_processor_.Process(*instruments, &output_samples[channel_count_ * current_frame],
                              output_frame_count - current_frame);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::RemoveInstrument(BarelyInstrumentHandle instrument) noexcept {
  mutable_instruments_.Update(BuildMutableInstrumentMap(instruments_, instrument));
  instruments_.erase(instrument);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyEngine::RemovePerformer(BarelyPerformer* performer) noexcept {
  performers_.erase(performer);
}

void BarelyEngine::ScheduleMessage(barely::Message message) noexcept {
  message_queue_.Add(update_frame_, std::move(message));
}

void BarelyEngine::SetEffectControl(BarelyEffectControlType type, float value) noexcept {
  if (auto& effect_control = effect_controls_[type]; effect_control.SetValue(value)) {
    ScheduleMessage(
        EffectControlMessage{static_cast<EffectControlType>(type), effect_control.value});
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
      for (auto* performer : performers_) {
        if (const auto maybe_next_key = performer->GetNextTaskKey();
            maybe_next_key.has_value() && *maybe_next_key < next_key) {
          has_tasks_to_process = true;
          next_key = *maybe_next_key;
        }
      }

      const auto& [update_duration, max_priority] = next_key;
      assert(update_duration > 0.0 || has_tasks_to_process);

      if (update_duration > 0) {
        for (auto* performer : performers_) {
          performer->Update(update_duration);
        }

        timestamp_ += barely::BeatsToSeconds(tempo_, update_duration);
        update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
      }

      if (has_tasks_to_process) {
        for (auto* performer : performers_) {
          performer->ProcessAllTasksAtPosition(max_priority);
        }
      }
    } else if (timestamp_ < timestamp) {
      timestamp_ = timestamp;
      update_frame_ = barely::SecondsToFrames(sample_rate_, timestamp_);
    }
  }
}
