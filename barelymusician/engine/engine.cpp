
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <utility>
#include <variant>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->StopAllNotes(clock_.GetTimestamp());
  }
  instrument_refs_.Update({});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::CreateInstrument(Id instrument_id,
                              Instrument::Definition definition,
                              int frame_rate) noexcept {
  assert(instrument_id > kInvalid);
  assert(frame_rate >= 0);
  if (instruments_
          .emplace(instrument_id,
                   std::make_unique<Instrument>(definition, frame_rate))
          .second) {
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::CreateSequence(Id sequence_id) noexcept {
  assert(sequence_id > kInvalid);
  return sequences_.emplace(sequence_id, std::pair{Sequence(clock_), kInvalid})
      .second;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    for (auto& [sequence_id, sequence] : sequences_) {
      if (sequence.second == instrument_id) {
        sequence.first.SetInstrument(nullptr);
        sequence.second = kInvalid;
      }
    }
    instrument->StopAllNotes(clock_.GetTimestamp());
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

bool Engine::DestroySequence(Id sequence_id) noexcept {
  if (const auto sequence_it = sequences_.find(sequence_id);
      sequence_it != sequences_.end()) {
    sequence_it->second.first.Stop();
    sequences_.erase(sequence_it);
    return true;
  }
  return false;
}

Clock& Engine::GetClock() noexcept { return clock_; }

Instrument* Engine::GetInstrument(Id instrument_id) noexcept {
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return instrument->get();
  }
  return nullptr;
}

Sequence* Engine::GetSequence(Id sequence_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return &sequence->first;
  }
  return nullptr;
}

Id* Engine::GetSequenceInstrumentId(Id sequence_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return &sequence->second;
  }
  return nullptr;
}

Transport& Engine::GetTransport() noexcept { return transport_; }

bool Engine::ProcessInstrument(Id instrument_id, double* output,
                               int num_output_channels, int num_output_frames,
                               double timestamp) noexcept {
  assert(output || num_output_channels == 0 || num_output_frames == 0);
  assert(num_output_channels >= 0);
  assert(num_output_frames >= 0);
  assert(timestamp >= 0.0);
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    (*instrument_ref)
        ->Process(output, num_output_channels, num_output_frames, timestamp);
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::ScheduleInstrumentEvent(Id instrument_id, Event event,
                                     double timestamp) noexcept {
  if (timestamp > clock_.GetTimestamp()) {
    instrument_events_.emplace(timestamp,
                               std::pair{instrument_id, std::move(event)});
  } else {
    ProcessInstrumentEvent(instrument_id, event, clock_.GetTimestamp());
  }
}

bool Engine::SetSequenceInstrumentId(Id sequence_id,
                                     Id instrument_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    sequence->first.SetInstrument(GetInstrument(instrument_id));
    sequence->second = instrument_id;
    return true;
  }
  return false;
}

void Engine::Start() noexcept { transport_.Start(); }

void Engine::Stop() noexcept {
  for (auto& [sequence_id, sequence] : sequences_) {
    sequence.first.Stop();
  }
  transport_.Stop();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (timestamp <= clock_.GetTimestamp()) return;
  Clock clock = clock_;
  clock_.SetTimestamp(timestamp);
  const double duration = clock.GetDuration(timestamp);
  if (duration > 0) {
    // TODO: This won't work if `SetPosition` is called during `Update`.
    const double transport_position = transport_.GetPosition();
    transport_.SetBeatCallback([&](double position) {
      if (beat_callback_) {
        beat_callback_(position,
                       clock.GetTimestamp(position - transport_position));
      }
    });
    transport_.Update(
        duration, [&](double begin_position, double end_position) noexcept {
          for (auto& [sequence_id, sequence] : sequences_) {
            sequence.first.Process(
                begin_position, end_position, [&](double position) {
                  return clock.GetTimestamp(position - transport_position);
                });
          }
        });
  }
  const auto begin = instrument_events_.begin();
  const auto end = instrument_events_.lower_bound(timestamp);
  for (auto it = begin; begin != end; ++it) {
    ProcessInstrumentEvent(it->second.first, it->second.second, it->first);
  }
  instrument_events_.erase(begin, end);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::ProcessInstrumentEvent(Id instrument_id, Event& event,
                                    double timestamp) noexcept {
  if (auto* instrument = GetInstrument(instrument_id)) {
    std::visit(
        EventVisitor{
            [&](SetDataEvent& set_data_event) noexcept {
              instrument->SetData(std::move(set_data_event.data), timestamp);
            },
            [&](SetNoteOffEvent& set_note_off_event) noexcept {
              instrument->StopNote(set_note_off_event.pitch, timestamp);
            },
            [&](SetNoteOnEvent& set_note_on_event) noexcept {
              instrument->StartNote(set_note_on_event.pitch,
                                    set_note_on_event.intensity, timestamp);
            },
            [&](SetParameterEvent& set_parameter_event) noexcept {
              instrument->SetParameter(set_parameter_event.index,
                                       set_parameter_event.value,
                                       set_parameter_event.slope, timestamp);
            }},
        event);
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::UpdateInstrumentReferenceMap() noexcept {
  InstrumentReferenceMap new_instrument_refs;
  new_instrument_refs.reserve(instruments_.size());
  for (const auto& [instrument_id, instrument] : instruments_) {
    new_instrument_refs.emplace(instrument_id, instrument.get());
  }
  instrument_refs_.Update(std::move(new_instrument_refs));
}

}  // namespace barelyapi
