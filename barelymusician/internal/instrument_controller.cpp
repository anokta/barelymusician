#include "barelymusician/internal/instrument_controller.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/instrument_processor.h"
#include "barelymusician/internal/message.h"

namespace barely {

namespace {

static constexpr std::array<ControlDefinition, static_cast<int>(InstrumentControl::kCount)>
    control_definitions = {
        // kGain
        ControlDefinition{1.0, 0.0, 1.0},
        // kVoiceCount
        ControlDefinition{8, 1, 32},
        // kOscillatorType
        ControlDefinition{static_cast<double>(OscillatorType::kNone),
                          static_cast<double>(OscillatorType::kNone),
                          static_cast<double>(OscillatorType::kNoise)},
        // kSamplePlayerLoop
        ControlDefinition{false},
        // kAttack
        ControlDefinition{0.05, 0.0, 60.0},
        // kDecay
        ControlDefinition{0.0, 0.0, 60.0},
        // kSustain
        ControlDefinition{1.0, 0.0, 1.0},
        // kRelease
        ControlDefinition{0.25, 0.0, 60.0},
        // kPitchShift
        ControlDefinition{0.0},
        // kRetrigger
        ControlDefinition{false},
};

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentController::InstrumentController(int frame_rate, double reference_frequency,
                                           int64_t update_frame) noexcept
    : controls_(BuildControls(control_definitions)),
      update_frame_(update_frame),
      processor_(frame_rate, reference_frequency) {
  assert(frame_rate > 0);
  for (int i = 0; i < static_cast<int>(controls_.size()); ++i) {
    processor_.SetControl(i, controls_[i].GetValue());
  }
}

InstrumentController::~InstrumentController() noexcept { SetAllNotesOff(); }

const double* InstrumentController::GetControl(int index) const noexcept {
  return (index >= 0 && index < static_cast<int>(controls_.size())) ? &controls_[index].GetValue()
                                                                    : nullptr;
}

const double* InstrumentController::GetNoteControl(double pitch, int index) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return (index >= 0 && index < static_cast<int>(note_controls->size()))
               ? &(*note_controls)[index].GetValue()
               : nullptr;
  }
  return nullptr;
}

bool InstrumentController::IsNoteOn(double pitch) const noexcept {
  return note_controls_.contains(pitch);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool InstrumentController::Process(double* output_samples, int output_channel_count,
                                   int output_frame_count, int64_t process_frame) noexcept {
  if ((!output_samples && output_channel_count > 0 && output_frame_count > 0) ||
      output_channel_count < 0 || output_frame_count < 0) {
    return false;
  }
  int frame = 0;
  // Process *all* messages before the end frame.
  const int64_t end_frame = process_frame + output_frame_count;
  for (auto* message = message_queue_.GetNext(end_frame); message;
       message = message_queue_.GetNext(end_frame)) {
    if (const int message_frame = static_cast<int>(message->first - process_frame);
        frame < message_frame) {
      processor_.Process(&output_samples[frame * output_channel_count], output_channel_count,
                         message_frame - frame);
      frame = message_frame;
    }
    std::visit(MessageVisitor{[this](ControlMessage& control_message) noexcept {
                                processor_.SetControl(control_message.index, control_message.value);
                              },
                              [this](NoteControlMessage& note_control_message) noexcept {
                                processor_.SetNoteControl(note_control_message.pitch,
                                                          note_control_message.index,
                                                          note_control_message.value);
                              },
                              [this](NoteOffMessage& note_off_message) noexcept {
                                processor_.SetNoteOff(note_off_message.pitch);
                              },
                              [this](NoteOnMessage& note_on_message) noexcept {
                                processor_.SetNoteOn(note_on_message.pitch,
                                                     note_on_message.intensity);
                              },
                              [this](SampleDataMessage& sample_data_message) noexcept {
                                processor_.SetSampleData(sample_data_message.sample_data);
                              }},
               message->second);
  }
  // Process the rest of the buffer.
  if (frame < output_frame_count) {
    processor_.Process(&output_samples[frame * output_channel_count], output_channel_count,
                       output_frame_count - frame);
  }
  return true;
}

void InstrumentController::ResetAllControls() noexcept {
  for (int i = 0; i < static_cast<int>(controls_.size()); ++i) {
    if (controls_[i].ResetValue()) {
      message_queue_.Add(update_frame_, ControlMessage{i, controls_[i].GetValue()});
    }
  }
}

bool InstrumentController::ResetControl(int index) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (controls_[index].ResetValue()) {
      message_queue_.Add(update_frame_, ControlMessage{index, controls_[index].GetValue()});
    }
    return true;
  }
  return false;
}

bool InstrumentController::ResetAllNoteControls(double pitch) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    for (int i = 0; i < static_cast<int>(note_controls->size()); ++i) {
      if ((*note_controls)[i].ResetValue()) {
        message_queue_.Add(update_frame_,
                           NoteControlMessage{pitch, i, (*note_controls)[i].GetValue()});
      }
    }
    return true;
  }
  return false;
}

bool InstrumentController::ResetNoteControl(double pitch, int index) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index >= 0 && index < static_cast<int>(note_controls->size())) {
      if ((*note_controls)[index].ResetValue()) {
        message_queue_.Add(update_frame_,
                           NoteControlMessage{pitch, index, (*note_controls)[index].GetValue()});
      }
      return true;
    }
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentController::SetAllNotesOff() noexcept {
  for (const auto& [note, _] : std::exchange(note_controls_, {})) {
    note_off_event_.Process(note);
    message_queue_.Add(update_frame_, NoteOffMessage{note});
  }
}

bool InstrumentController::SetControl(int index, double value) noexcept {
  if (index >= 0 && index < static_cast<int>(controls_.size())) {
    if (controls_[index].SetValue(value)) {
      message_queue_.Add(update_frame_, ControlMessage{index, controls_[index].GetValue()});
    }
    return true;
  }
  return false;
}

bool InstrumentController::SetNoteControl(double pitch, int index, double value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (index >= 0 && index < static_cast<int>(note_controls->size())) {
      if ((*note_controls)[index].SetValue(value)) {
        message_queue_.Add(update_frame_,
                           NoteControlMessage{pitch, index, (*note_controls)[index].GetValue()});
      }
      return true;
    }
  }
  return false;
}

void InstrumentController::SetNoteOff(double pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    note_off_event_.Process(pitch);
    message_queue_.Add(update_frame_, NoteOffMessage{pitch});
  }
}

void InstrumentController::SetNoteOffEvent(NoteOffEventDefinition definition,
                                           void* user_data) noexcept {
  note_off_event_ = {definition, user_data};
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void InstrumentController::SetNoteOn(double pitch, double intensity) noexcept {
  // TODO(#139): Revisit note controls.
  if (const auto [it, success] = note_controls_.try_emplace(pitch, BuildControls({})); success) {
    note_on_event_.Process(pitch, intensity);
    message_queue_.Add(update_frame_, NoteOnMessage{pitch, intensity});
    for (int i = 0; i < static_cast<int>(it->second.size()); ++i) {
      message_queue_.Add(update_frame_, NoteControlMessage{pitch, i, (it->second)[i].GetValue()});
    }
  }
}

void InstrumentController::SetNoteOnEvent(NoteOnEventDefinition definition,
                                          void* user_data) noexcept {
  note_on_event_ = {definition, user_data};
}

void InstrumentController::SetSampleData(SampleData sample_data) noexcept {
  message_queue_.Add(update_frame_, SampleDataMessage{std::move(sample_data)});
}

void InstrumentController::Update(int64_t update_frame) noexcept {
  assert(update_frame >= update_frame_);
  update_frame_ = update_frame;
}

}  // namespace barely
