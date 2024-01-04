#include "barelymusician/internal/instrument.h"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/seconds.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/message.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const InstrumentDefinition& definition, int frame_rate, double initial_tempo,
                       double initial_timestamp) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback),
      set_control_callback_(definition.set_control_callback),
      set_data_callback_(definition.set_data_callback),
      set_note_control_callback_(definition.set_note_control_callback),
      set_note_off_callback_(definition.set_note_off_callback),
      set_note_on_callback_(definition.set_note_on_callback),
      frame_rate_(frame_rate),
      default_note_controls_(
          BuildControls(static_cast<const ControlDefinition*>(definition.note_control_definitions),
                        definition.note_control_definition_count)),
      controls_(BuildControls(static_cast<const ControlDefinition*>(definition.control_definitions),
                              definition.control_definition_count)),
      tempo_(initial_tempo),
      timestamp_(initial_timestamp) {
  assert(frame_rate > 0);
  assert(initial_tempo >= 0.0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (int index = 0; index < definition.control_definition_count; ++index) {
      set_control_callback_(&state_, index, controls_[index].GetValue(), 0.0);
    }
  }
}

Instrument::~Instrument() noexcept {
  SetAllNotesOff();
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::AddEffect(Effect& effect) noexcept {
  [[maybe_unused]] const bool success = effects_.emplace(effect.GetProcessOrder(), &effect).second;
  assert(success);
  UpdateEffectReferences();
}

const Control* Instrument::GetControl(int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    return &controls_[index];
  }
  return nullptr;
}

int Instrument::GetFrameRate() const noexcept { return frame_rate_; }

const Control* Instrument::GetNoteControl(double pitch, int index) const noexcept {
  if (index >= 0 && index < static_cast<int>(default_note_controls_.size())) {
    if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
      return &(*note_controls)[index];
    }
  }
  return nullptr;
}

bool Instrument::IsNoteOn(double pitch) const noexcept {
  // TODO(#111): Use `contains` instead of `find`.
  return note_controls_.find(pitch) != note_controls_.end();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Instrument::Process(double* output_samples, int output_channel_count, int output_frame_count,
                         double timestamp) noexcept {
  if ((!output_samples && output_channel_count > 0 && output_frame_count > 0) ||
      output_channel_count < 0 || output_frame_count < 0) {
    return false;
  }
  int frame = 0;
  // Process *all* messages before the end timestamp.
  const double end_timestamp = timestamp + SecondsFromFrames(frame_rate_, output_frame_count);
  auto effect_ptrs = effect_ptrs_.GetScopedView();
  for (auto* message = message_queue_.GetNext(end_timestamp); message;
       message = message_queue_.GetNext(end_timestamp)) {
    if (const int message_frame = FramesFromSeconds(frame_rate_, message->first - timestamp);
        frame < message_frame) {
      const int sample_offset = frame * output_channel_count;
      const int frame_count = message_frame - frame;
      if (process_callback_) {
        process_callback_(&state_, &output_samples[sample_offset], output_channel_count,
                          frame_count);
      }
      for (auto* effect_ptr : *effect_ptrs) {
        assert(effect_ptr);
        effect_ptr->Process(&output_samples[sample_offset], output_channel_count, frame_count);
      }
      frame = message_frame;
    }
    std::visit(
        MessageVisitor{
            [this](ControlMessage& control_message) noexcept {
              if (set_control_callback_) {
                set_control_callback_(&state_, control_message.index, control_message.value,
                                      control_message.slope_per_frame);
              }
            },
            [this](DataMessage& data_message) noexcept {
              if (set_data_callback_) {
                data_.swap(data_message.data);
                set_data_callback_(&state_, data_.data(), static_cast<int>(data_.size()));
              }
            },
            [](EffectControlMessage& effect_control_message) noexcept {
              assert(effect_control_message.effect);
              effect_control_message.effect->ProcessControlMessage(
                  effect_control_message.index, effect_control_message.value,
                  effect_control_message.slope_per_frame);
            },
            [](EffectDataMessage& effect_data_message) noexcept {
              assert(effect_data_message.effect);
              effect_data_message.effect->ProcessDataMessage(effect_data_message.data);
            },
            [this](NoteControlMessage& note_control_message) noexcept {
              if (set_note_control_callback_) {
                set_note_control_callback_(&state_, note_control_message.pitch,
                                           note_control_message.index, note_control_message.value,
                                           note_control_message.slope_per_frame);
              }
            },
            [this](NoteOffMessage& note_off_message) noexcept {
              if (set_note_off_callback_) {
                set_note_off_callback_(&state_, note_off_message.pitch);
              }
            },
            [this](NoteOnMessage& note_on_message) noexcept {
              if (set_note_on_callback_) {
                set_note_on_callback_(&state_, note_on_message.pitch, note_on_message.intensity);
              }
            }},
        message->second);
  }
  // Process the rest of the buffer.
  if (frame < output_frame_count) {
    const int sample_offset = frame * output_channel_count;
    const int frame_count = output_frame_count - frame;
    if (process_callback_) {
      process_callback_(&state_, &output_samples[sample_offset], output_channel_count, frame_count);
    }
    for (auto* effect_ptr : *effect_ptrs) {
      assert(effect_ptr);
      effect_ptr->Process(&output_samples[sample_offset], output_channel_count, frame_count);
    }
  }
  return true;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::RemoveEffect(Effect& effect) noexcept {
  [[maybe_unused]] const bool success =
      effects_.erase({effect.GetProcessOrder(), const_cast<Effect*>(&effect)}) == 1;
  assert(success);
  UpdateEffectReferences();
}

void Instrument::ResetAllControls() noexcept {
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (auto& control = controls_[index]; control.Reset()) {
      control_event_.Process(index, control.GetValue());
      message_queue_.Add(timestamp_, ControlMessage{index, control.GetValue(), 0.0});
    }
  }
}

void Instrument::ResetAllEffectControls(Effect& effect) noexcept {
  auto& effect_controls = effect.GetAllControls();
  for (int index = 0; index < static_cast<int>(effect_controls.size()); ++index) {
    if (auto& effect_control = effect_controls[index]; effect_control.Reset()) {
      effect.ProcessControlEvent(index);
      message_queue_.Add(timestamp_,
                         EffectControlMessage{&effect, index, effect_control.GetValue(), 0.0});
    }
  }
}

bool Instrument::ResetAllNoteControls(double pitch) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    for (int index = 0; index < static_cast<int>(note_controls->size()); ++index) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        note_control_event_.Process(pitch, index, note_control.GetValue());
        message_queue_.Add(timestamp_,
                           NoteControlMessage{pitch, index, note_control.GetValue(), 0.0});
      }
    }
    return true;
  }
  return false;
}

bool Instrument::ResetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Reset()) {
      control_event_.Process(index, control.GetValue());
      message_queue_.Add(timestamp_, ControlMessage{index, control.GetValue(), 0.0});
    }
    return true;
  }
  return false;
}

bool Instrument::ResetEffectControl(Effect& effect, int index) noexcept {
  if (auto* effect_control = effect.GetControl(index)) {
    if (effect_control->Reset()) {
      effect.ProcessControlEvent(index);
      message_queue_.Add(timestamp_, ControlMessage{index, effect_control->GetValue(), 0.0});
    }
    return true;
  }
  return false;
}

bool Instrument::ResetNoteControl(double pitch, int index) noexcept {
  if (index >= 0 && index < static_cast<int>(default_note_controls_.size())) {
    if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
      if (auto& note_control = (*note_controls)[index]; note_control.Reset()) {
        note_control_event_.Process(pitch, index, note_control.GetValue());
        message_queue_.Add(timestamp_,
                           NoteControlMessage{pitch, index, note_control.GetValue(), 0.0});
      }
      return true;
    }
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetAllNotesOff() noexcept {
  for (const auto& [pitch, note_controls] : std::exchange(note_controls_, {})) {
    note_off_event_.Process(pitch);
    message_queue_.Add(timestamp_, NoteOffMessage{pitch});
  }
}

bool Instrument::SetControl(int index, double value, double slope_per_beat) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Set(value, slope_per_beat)) {
      control_event_.Process(index, control.GetValue());
      message_queue_.Add(
          timestamp_, ControlMessage{index, control.GetValue(), GetSlopePerFrame(slope_per_beat)});
    }
    return true;
  }
  return false;
}

void Instrument::SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept {
  control_event_ = {definition, user_data};
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(timestamp_, DataMessage{std::move(data)});
}

bool Instrument::SetEffectControl(Effect& effect, int index, double value,
                                  double slope_per_beat) noexcept {
  if (auto* effect_control = effect.GetControl(index)) {
    if (effect_control->Set(value, slope_per_beat)) {
      effect.ProcessControlEvent(index);
      message_queue_.Add(timestamp_,
                         EffectControlMessage{&effect, index, effect_control->GetValue(),
                                              GetSlopePerFrame(slope_per_beat)});
    }
    return true;
  }
  return false;
}

void Instrument::SetEffectData(Effect& effect, std::vector<std::byte> data) noexcept {
  message_queue_.Add(timestamp_, EffectDataMessage{&effect, std::move(data)});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetEffectProcessOrder(Effect& effect, int process_order) noexcept {
  if (const int current_process_order = effect.GetProcessOrder();
      current_process_order != process_order) {
    auto node = effects_.extract({current_process_order, &effect});
    node.value().first = process_order;
    effects_.insert(std::move(node));
    effect.SetProcessOrder(process_order);
    UpdateEffectReferences();
  }
}

bool Instrument::SetNoteControl(double pitch, int index, double value,
                                double slope_per_beat) noexcept {
  if (index >= 0 && index < static_cast<int>(default_note_controls_.size())) {
    if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
      if (auto& note_control = (*note_controls)[index]; note_control.Set(value, slope_per_beat)) {
        note_control_event_.Process(pitch, index, note_control.GetValue());
        message_queue_.Add(timestamp_, NoteControlMessage{pitch, index, note_control.GetValue(),
                                                          GetSlopePerFrame(slope_per_beat)});
      }
      return true;
    }
  }
  return false;
}

void Instrument::SetNoteControlEvent(NoteControlEventDefinition definition,
                                     void* user_data) noexcept {
  note_control_event_ = {definition, user_data};
}

void Instrument::SetNoteOff(double pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_event_.Process(pitch);
    message_queue_.Add(timestamp_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept {
  note_off_event_ = {definition, user_data};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (note_controls_.try_emplace(pitch, default_note_controls_).second) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(timestamp_, NoteOnMessage{pitch, intensity});
    for (int index = 0; index < static_cast<int>(default_note_controls_.size()); ++index) {
      message_queue_.Add(
          timestamp_, NoteControlMessage{pitch, index, default_note_controls_[index].GetValue()});
    }
  }
}

void Instrument::SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept {
  note_on_event_ = {definition, user_data};
}

void Instrument::SetTempo(double tempo) noexcept {
  assert(tempo_ != tempo);
  tempo_ = tempo;
  // Update controls.
  for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
    if (const auto& control = controls_[index]; control.GetSlopePerBeat() != 0.0) {
      message_queue_.Add(timestamp_, ControlMessage{index, control.GetValue(),
                                                    GetSlopePerFrame(control.GetSlopePerBeat())});
    }
  }
  // Update effect controls.
  for (const auto& [process_order, effect] : effects_) {
    auto& effect_controls = effect->GetAllControls();
    for (int index = 0; index < static_cast<int>(effect_controls.size()); ++index) {
      if (const auto& effect_control = effect_controls[index];
          effect_control.GetSlopePerBeat() != 0.0) {
        message_queue_.Add(
            timestamp_, EffectControlMessage{effect, index, effect_control.GetValue(),
                                             GetSlopePerFrame(effect_control.GetSlopePerBeat())});
      }
    }
  }
  // Update note controls.
  for (auto& [pitch, note_controls] : note_controls_) {
    for (int index = 0; index < static_cast<int>(note_controls.size()); ++index) {
      if (const auto& note_control = note_controls[index]; note_control.GetSlopePerBeat() != 0.0) {
        message_queue_.Add(timestamp_,
                           NoteControlMessage{pitch, index, note_control.GetValue(),
                                              GetSlopePerFrame(note_control.GetSlopePerBeat())});
      }
    }
  }
}

void Instrument::Update(double timestamp) noexcept {
  if (timestamp_ >= timestamp) {
    return;
  }
  if (tempo_ > 0.0) {
    const double duration = BeatsFromSeconds(tempo_, timestamp - timestamp_);
    // Update controls.
    for (int index = 0; index < static_cast<int>(controls_.size()); ++index) {
      if (auto& control = controls_[index]; control.Update(duration)) {
        control_event_.Process(index, control.GetValue());
      }
    }
    // Update effect controls.
    for (const auto& [process_order, effect] : effects_) {
      auto& effect_controls = effect->GetAllControls();
      for (int index = 0; index < static_cast<int>(effect_controls.size()); ++index) {
        if (auto& effect_control = effect_controls[index]; effect_control.Update(duration)) {
          effect->ProcessControlEvent(index);
        }
      }
    }
    // Update note controls.
    for (auto& [pitch, note_controls] : note_controls_) {
      for (int index = 0; index < static_cast<int>(note_controls.size()); ++index) {
        if (auto& note_control = note_controls[index]; note_control.Update(duration)) {
          note_control_event_.Process(pitch, index, note_control.GetValue());
        }
      }
    }
  }
  timestamp_ = timestamp;
}

double Instrument::GetSlopePerFrame(double slope_per_beat) const noexcept {
  return tempo_ > 0.0 ? BeatsFromSeconds(tempo_, slope_per_beat) / static_cast<double>(frame_rate_)
                      : 0.0;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::UpdateEffectReferences() noexcept {
  std::vector<Effect*> new_effect_ptrs;
  new_effect_ptrs.reserve(effects_.size());
  for (const auto& [process_order, effect_ptr] : effects_) {
    new_effect_ptrs.push_back(effect_ptr);
  }
  effect_ptrs_.Update(std::move(new_effect_ptrs));
}

}  // namespace barely::internal
