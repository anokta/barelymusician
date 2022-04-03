
#include "barelymusician/engine/engine.h"

#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

Engine::~Engine() { instrument_refs_.Update({}); }

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::CreateInstrument(Id instrument_id,
                              Instrument::Definition definition,
                              int frame_rate) noexcept {
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
  return sequences_
      .emplace(sequence_id,
               std::pair{Sequence(conductor_, transport_), kInvalid})
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
    instrument->StopAllNotes(transport_.GetTimestamp());
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
    return true;
  }
  return false;
}

Conductor& Engine::GetConductor() noexcept { return conductor_; }

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

Id Engine::GetSequenceInstrumentId(Id sequence_id) const noexcept {
  if (const auto* sequence = FindOrNull(sequences_, sequence_id)) {
    return sequence->second;
  }
  // TODO: This should ideally return `kNotFound` at call site.
  return kInvalid;
}

Transport& Engine::GetTransport() noexcept { return transport_; }

bool Engine::DestroySequence(Id sequence_id) noexcept {
  if (const auto sequence_it = sequences_.find(sequence_id);
      sequence_it != sequences_.end()) {
    sequence_it->second.first.Stop();
    sequences_.erase(sequence_it);
    return true;
  }
  return false;
}

bool Engine::ProcessInstrument(Id instrument_id, double* output,
                               int num_output_channels, int num_output_frames,
                               double timestamp) noexcept {
  auto instrument_refs = instrument_refs_.GetScopedView();
  if (const auto* instrument_ref =
          FindOrNull(*instrument_refs, instrument_id)) {
    (*instrument_ref)
        ->Process(output, num_output_channels, num_output_frames, timestamp);
    return true;
  }
  return false;
}

bool Engine::SetSequenceInstrumentId(Id sequence_id,
                                     Id instrument_id) noexcept {
  if (auto* sequence = FindOrNull(sequences_, sequence_id)) {
    if (auto* instrument = GetInstrument(instrument_id)) {
      sequence->first.SetInstrument(instrument);
      sequence->second = instrument_id;
    } else {
      sequence->first.SetInstrument(nullptr);
      sequence->second = kInvalid;
    }
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

void Engine::Update(double timestamp) noexcept {
  transport_.Update(
      timestamp, [this](double begin_position, double end_position) noexcept {
        for (auto& [sequence_id, sequence] : sequences_) {
          sequence.first.Process(begin_position, end_position);
        }
      });
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
