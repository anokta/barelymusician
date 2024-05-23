#include "barelymusician/internal/instrument.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/message.h"
#include "barelymusician/internal/seconds.h"

namespace barely::internal {

// NOLINTNEXTLINE(bugprone-exception-escape)
Instrument::Instrument(const InstrumentDefinition& definition, int frame_rate,
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
      update_frame_(FramesFromSeconds(frame_rate_, initial_timestamp)) {
  assert(frame_rate > 0);
  if (definition.create_callback) {
    definition.create_callback(&state_, frame_rate);
  }
  if (set_control_callback_) {
    for (const auto& [id, control] : controls_) {
      set_control_callback_(&state_, id, control.GetValue());
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

const Control* Instrument::GetControl(int id) const noexcept {
  if (const auto* control = FindOrNull(controls_, id)) {
    return control;
  }
  return nullptr;
}

int Instrument::GetFrameRate() const noexcept { return frame_rate_; }

const Control* Instrument::GetNoteControl(double pitch, int id) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (const auto* note_control = FindOrNull(*note_controls, id)) {
      return note_control;
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
  // Process *all* messages before the end frame.
  const int64_t begin_frame = FramesFromSeconds(frame_rate_, timestamp);
  const int64_t end_frame = begin_frame + output_frame_count;
  auto effect_ptrs = effect_ptrs_.GetScopedView();
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - begin_frame);
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
                set_control_callback_(&state_, control_message.id, control_message.value);
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
              effect_control_message.effect->ProcessControlMessage(effect_control_message.id,
                                                                   effect_control_message.value);
            },
            [](EffectDataMessage& effect_data_message) noexcept {
              assert(effect_data_message.effect);
              effect_data_message.effect->ProcessDataMessage(effect_data_message.data);
            },
            [this](NoteControlMessage& note_control_message) noexcept {
              if (set_note_control_callback_) {
                set_note_control_callback_(&state_, note_control_message.pitch,
                                           note_control_message.id, note_control_message.value);
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
  for (auto& [id, control] : controls_) {
    if (control.Reset()) {
      control_event_.Process(id, control.GetValue());
      message_queue_.Add(update_frame_, ControlMessage{id, control.GetValue()});
    }
  }
}

void Instrument::ResetAllEffectControls(Effect& effect) noexcept {
  auto& effect_controls = effect.GetAllControls();
  for (auto& [id, effect_control] : effect_controls) {
    if (effect_control.Reset()) {
      effect.ProcessControlEvent(id);
      message_queue_.Add(update_frame_,
                         EffectControlMessage{&effect, id, effect_control.GetValue()});
    }
  }
}

bool Instrument::ResetAllNoteControls(double pitch) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    for (auto& [id, note_control] : *note_controls) {
      if (note_control.Reset()) {
        note_control_event_.Process(pitch, id, note_control.GetValue());
        message_queue_.Add(update_frame_, NoteControlMessage{pitch, id, note_control.GetValue()});
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
      message_queue_.Add(update_frame_, ControlMessage{index, control.GetValue()});
    }
    return true;
  }
  return false;
}

bool Instrument::ResetEffectControl(Effect& effect, int index) noexcept {
  if (auto* effect_control = effect.GetControl(index)) {
    if (effect_control->Reset()) {
      effect.ProcessControlEvent(index);
      message_queue_.Add(update_frame_, ControlMessage{index, effect_control->GetValue()});
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
        message_queue_.Add(update_frame_,
                           NoteControlMessage{pitch, index, note_control.GetValue()});
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
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

bool Instrument::SetControl(int index, double value) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (auto& control = controls_[index]; control.Set(value)) {
      control_event_.Process(index, control.GetValue());
      message_queue_.Add(update_frame_, ControlMessage{index, control.GetValue()});
    }
    return true;
  }
  return false;
}

void Instrument::SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept {
  control_event_ = {definition, user_data};
}

void Instrument::SetData(std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, DataMessage{std::move(data)});
}

bool Instrument::SetEffectControl(Effect& effect, int index, double value) noexcept {
  if (auto* effect_control = effect.GetControl(index)) {
    if (effect_control->Set(value)) {
      effect.ProcessControlEvent(index);
      message_queue_.Add(update_frame_,
                         EffectControlMessage{&effect, index, effect_control->GetValue()});
    }
    return true;
  }
  return false;
}

void Instrument::SetEffectData(Effect& effect, std::vector<std::byte> data) noexcept {
  message_queue_.Add(update_frame_, EffectDataMessage{&effect, std::move(data)});
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

bool Instrument::SetNoteControl(double pitch, int index, double value) noexcept {
  if (index >= 0 && index < static_cast<int>(default_note_controls_.size())) {
    if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
      if (auto& note_control = (*note_controls)[index]; note_control.Set(value)) {
        note_control_event_.Process(pitch, index, note_control.GetValue());
        message_queue_.Add(update_frame_,
                           NoteControlMessage{pitch, index, note_control.GetValue()});
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
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void Instrument::SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept {
  note_off_event_ = {definition, user_data};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Instrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (note_controls_.try_emplace(pitch, default_note_controls_).second) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(update_frame_, NoteOnMessage{pitch, intensity});
    for (const auto& [id, note_control] : default_note_controls_) {
      message_queue_.Add(update_frame_, NoteControlMessage{pitch, id, note_control.GetValue()});
    }
  }
}

void Instrument::SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept {
  note_on_event_ = {definition, user_data};
}

void Instrument::Update(double timestamp) noexcept {
  update_frame_ = FramesFromSeconds(frame_rate_, timestamp);
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
