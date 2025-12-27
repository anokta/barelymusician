#include "api/instrument.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <utility>

#include "api/engine.h"
#include "common/find_or_null.h"
#include "dsp/control.h"
#include "dsp/message.h"

namespace {

using ::barely::Control;
using ::barely::InstrumentControlArray;
using ::barely::InstrumentControlMessage;
using ::barely::InstrumentControlType;
using ::barely::NoteControlArray;
using ::barely::NoteControlMessage;
using ::barely::NoteControlType;
using ::barely::NoteOffMessage;
using ::barely::NoteOnMessage;
using ::barely::SampleDataMessage;

// Returns a control array with overrides.
[[nodiscard]] InstrumentControlArray BuildControlArray(
    std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept {
  InstrumentControlArray control_array = {
      Control(1.0f, 0.0f, 1.0f),                   // kGain
      Control(0.0f),                               // kPitchShift
      Control(false),                              // kRetrigger
      Control(0.0f, -1.0f, 1.0f),                  // kStereoPan
      Control(8, 1, 16),                           // kVoiceCount
      Control(0.0f, 0.0f, 60.0f),                  // kAttack
      Control(0.0f, 0.0f, 60.0f),                  // kDecay
      Control(1.0f, 0.0f, 1.0f),                   // kSustain
      Control(0.0f, 0.0f, 60.0f),                  // kRelease
      Control(0.0f, 0.0f, 1.0f),                   // kOscMix
      Control(0, 0, BarelyOscMode_kCount - 1),     // kOscMode
      Control(0.0f, 0.0f, 1.0f),                   // kOscNoiseMix
      Control(0.0f),                               // kOscPitchShift
      Control(0.0f, 0.0f, 1.0f),                   // kOscShape
      Control(0.0f, -0.5f, 0.5f),                  // kOscSkew
      Control(0, 0, BarelySliceMode_kCount - 1),   // kSliceMode
      Control(16.0f, 1.0f, 16.0f),                 // kBitCrusherDepth
      Control(1.0f, 0.0f, 1.0f),                   // kBitCrusherRate
      Control(0.0f, 0.0f, 1.0f),                   // kDistortionAmount
      Control(1.0f, 0.0f, 20.0f),                  // kDistortionDrive
      Control(0, 0, BarelyFilterType_kCount - 1),  // kFilterType
      Control(0.0f, 0.0f),                         // kFilterFrequency
      Control(std::sqrt(0.5f), 0.1f),              // kFilterQ
      Control(0.0f, 0.0f, 1.0f),                   // kDelaySend
      Control(0.0f, -1.0f, 1.0f),                  // kSidechainSend
      Control(0, 0, BarelyArpMode_kCount - 1),     // kArpMode
      Control(0.5f, 0.001f, 1.0f),                 // kArpGateRatio
      Control(1.0f, 0.0f, 16.0f),                  // kArpRate
  };
  for (const auto& [type, value] : control_overrides) {
    control_array[type].SetValue(value);
  }
  return control_array;
}

// Returns a note control array with overrides.
[[nodiscard]] NoteControlArray BuildNoteControlArray(
    std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  NoteControlArray note_control_array = {
      Control(1.0f, 0.0f, 1.0f),  // kGain
      Control(0.0f),              // kPitchShift
  };
  for (const auto& [type, value] : note_control_overrides) {
    note_control_array[type].SetValue(value);
  }
  return note_control_array;
}

// Returns an array of note control values from a given note control array.
[[nodiscard]] std::array<float, BarelyNoteControlType_kCount> BuildNoteControls(
    const NoteControlArray& note_control_array) noexcept {
  std::array<float, BarelyNoteControlType_kCount> note_controls;
  for (int i = 0; i < BarelyNoteControlType_kCount; ++i) {
    note_controls[i] = note_control_array[i].value;
  }
  return note_controls;
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
BarelyInstrument::BarelyInstrument(
    BarelyEngine& engine,
    std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept {
  Init(engine, control_overrides);
}

void BarelyInstrument::Init(
    BarelyEngine& engine,
    std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept {
  controls_ = BuildControlArray(control_overrides);
  engine_ = &engine;
  arp_index = engine.AddPerformer();
  arp_task_index = engine.AddTask();
  const float arp_rate = controls_[BarelyInstrumentControlType_kArpRate].value;

  auto& arp = engine.GetPerformer(arp_index);
  arp.SetLooping(true);
  arp.SetLoopLength((arp_rate > 0.0f) ? 1.0 / static_cast<double>(arp_rate) : 0.0);

  auto& arp_task = engine.GetTask(arp_task_index);
  arp_task = {
      {[](BarelyTaskEventType type, void* user_data) {
         auto* instrument = static_cast<BarelyInstrument*>(user_data);
         assert(instrument);
         if (type == BarelyTaskEventType_kBegin) {
           assert(!instrument->arp_pitch_.has_value());
           instrument->UpdateArp();
           assert(instrument->arp_pitch_.has_value());
           const float pitch = *instrument->arp_pitch_;
           instrument->note_event_callback_(BarelyNoteEventType_kBegin, pitch);
           instrument->engine_->ScheduleMessage(
               NoteOnMessage{instrument->instrument_index, pitch,
                             BuildNoteControls(instrument->note_controls_.at(pitch))});
         } else if (type == BarelyTaskEventType_kEnd) {
           assert(instrument->arp_pitch_.has_value());
           const float pitch = *instrument->arp_pitch_;
           instrument->note_event_callback_(BarelyNoteEventType_kEnd, pitch);
           instrument->engine_->ScheduleMessage(
               NoteOffMessage{instrument->instrument_index, pitch});
           instrument->arp_pitch_ = std::nullopt;
         }
       },
       this},
      0.0,
      static_cast<double>(controls_[BarelyInstrumentControlType_kArpGateRatio].value) *
          arp.loop_length,
      std::numeric_limits<int32_t>::max(),
      arp_index,
  };

  arp.AddTask(&arp_task);
}

float BarelyInstrument::GetControl(BarelyInstrumentControlType type) const noexcept {
  return controls_[type].value;
}

const float* BarelyInstrument::GetNoteControl(float pitch,
                                              BarelyNoteControlType type) const noexcept {
  if (const auto* note_controls = FindOrNull(note_controls_, pitch)) {
    return &(*note_controls)[type].value;
  }
  return nullptr;
}

bool BarelyInstrument::IsNoteOn(float pitch) const noexcept {
  return engine_->GetPerformer(arp_index).is_playing ? arp_pitch_.has_value()
                                                     : note_controls_.contains(pitch);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetAllNotesOff() noexcept {
  note_controls_.clear();
  auto& arp = engine_->GetPerformer(arp_index);
  if (arp.is_playing) {
    pitches_.clear();
    arp.Stop();
    arp.SetPosition(0.0);
    arp_pitch_index_ = -1;
  } else {
    for (const float pitch : std::exchange(pitches_, {})) {
      note_event_callback_(BarelyNoteEventType_kEnd, pitch);
      engine_->ScheduleMessage(NoteOffMessage{instrument_index, pitch});
    }
  }
}

void BarelyInstrument::SetControl(BarelyInstrumentControlType type, float value) noexcept {
  if (auto& control = controls_[type]; control.SetValue(value)) {
    ProcessControl(static_cast<InstrumentControlType>(type), control.value);
  }
}

void BarelyInstrument::SetNoteControl(float pitch, BarelyNoteControlType type,
                                      float value) noexcept {
  if (auto* note_controls = FindOrNull(note_controls_, pitch)) {
    if (auto& note_control = (*note_controls)[type]; note_control.SetValue(value)) {
      engine_->ScheduleMessage(NoteControlMessage{
          instrument_index, pitch, static_cast<NoteControlType>(type), note_control.value});
    }
  }
}

void BarelyInstrument::SetNoteOff(float pitch) noexcept {
  if (note_controls_.erase(pitch) > 0) {
    pitches_.erase(std::find(pitches_.begin(), pitches_.end(), pitch));
    auto& arp = engine_->GetPerformer(arp_index);
    if (pitches_.empty() && arp.is_playing) {
      arp.Stop();
      arp.SetPosition(0.0);
      arp_pitch_index_ = -1;
    } else if (!arp.is_playing) {
      note_event_callback_(BarelyNoteEventType_kEnd, pitch);
      engine_->ScheduleMessage(NoteOffMessage{instrument_index, pitch});
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void BarelyInstrument::SetNoteOn(
    float pitch, std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept {
  if (const auto [it, success] =
          note_controls_.try_emplace(pitch, BuildNoteControlArray(note_control_overrides));
      success) {
    pitches_.insert(std::lower_bound(pitches_.begin(), pitches_.end(), pitch), pitch);
    auto& arp = engine_->GetPerformer(arp_index);
    if (pitches_.size() == 1 &&
        static_cast<BarelyArpMode>(controls_[BarelyInstrumentControlType_kArpMode].value) !=
            BarelyArpMode_kNone) {
      arp.Start();
    } else if (!arp.is_playing) {
      note_event_callback_(BarelyNoteEventType_kBegin, pitch);
      engine_->ScheduleMessage(
          NoteOnMessage{instrument_index, pitch, BuildNoteControls(it->second)});
    }
  }
}

void BarelyInstrument::SetSampleData(std::span<const BarelySlice> slices) noexcept {
  engine_->ScheduleMessage(SampleDataMessage{instrument_index, slices});
}

void BarelyInstrument::ProcessControl(InstrumentControlType type, float value) noexcept {
  switch (type) {
    case InstrumentControlType::kArpMode: {
      auto& arp = engine_->GetPerformer(arp_index);
      if (static_cast<BarelyArpMode>(value) == BarelyArpMode_kNone) {
        if (arp.is_playing) {
          arp.Stop();
          arp.SetPosition(0.0);
          for (const auto& [pitch, note_controls] : note_controls_) {
            note_event_callback_(BarelyNoteEventType_kBegin, pitch);
            engine_->ScheduleMessage(
                NoteOnMessage{instrument_index, pitch, BuildNoteControls(note_controls)});
          }
        }
      } else if (!pitches_.empty() && !arp.is_playing) {
        arp.Start();
        for (const float pitch : pitches_) {
          note_event_callback_(BarelyNoteEventType_kEnd, pitch);
          engine_->ScheduleMessage(NoteOffMessage{instrument_index, pitch});
        }
      }
    } break;
    case InstrumentControlType::kArpGateRatio:
      engine_->GetPerformer(arp_index).SetTaskDuration(
          &engine_->GetTask(arp_task_index),
          static_cast<double>(value) * engine_->GetPerformer(arp_index).loop_length);
      break;
    case InstrumentControlType::kArpRate:
      engine_->GetPerformer(arp_index).SetLoopLength(
          (value > 0.0f) ? 1.0 / static_cast<double>(value) : 0.0);
      engine_->GetPerformer(arp_index).SetTaskDuration(
          &engine_->GetTask(arp_task_index),
          static_cast<double>(controls_[BarelyInstrumentControlType_kArpGateRatio].value) *
              engine_->GetPerformer(arp_index).loop_length);
      break;
    default:
      engine_->ScheduleMessage(InstrumentControlMessage{instrument_index, type, value});
      break;
  }
}

void BarelyInstrument::UpdateArp() noexcept {
  const int size = static_cast<int>(pitches_.size());
  switch (static_cast<BarelyArpMode>(controls_[BarelyInstrumentControlType_kArpMode].value)) {
    case BarelyArpMode_kUp:
      arp_pitch_index_ = (arp_pitch_index_ + 1) % size;
      break;
    case BarelyArpMode_kDown:
      arp_pitch_index_ = (arp_pitch_index_ == -1) ? size - 1 : (arp_pitch_index_ + size - 1) % size;
      break;
    case BarelyArpMode_kRandom:
      arp_pitch_index_ = engine_->main_rng().Generate(0, size);
      break;
    default:
      assert(!"Invalid arpeggiator mode");
      return;
  }
  assert(arp_pitch_index_ >= 0 && arp_pitch_index_ < size);
  arp_pitch_ = pitches_[arp_pitch_index_];
}
