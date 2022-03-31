
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

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

}  // namespace

Engine::~Engine() { instrument_refs_.Update({}); }

Id Engine::AddSequenceNote(Sequence* sequence, Note::Definition definition,
                           double position) noexcept {
  // TODO: Refactor this? Currently discards the return value of `AddNote`.
  const Id note_id = ++id_counter_;
  sequence->AddNote(note_id, definition, position);
  return note_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Id Engine::CreateInstrument(Instrument::Definition definition,
                            int frame_rate) noexcept {
  const Id instrument_id = ++id_counter_;
  instruments_.emplace(instrument_id,
                       std::make_unique<Instrument>(definition, frame_rate));
  UpdateInstrumentReferenceMap();
  return instrument_id;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
Id Engine::CreateSequence() noexcept {
  const Id sequence_id = ++id_counter_;
  sequences_.emplace(sequence_id,
                     std::pair{Sequence(conductor_, transport_), kInvalid});
  return sequence_id;
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

double Engine::GetPlaybackTempo() const noexcept { return tempo_; }

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

double Engine::GetTimestamp() const noexcept {
  return transport_.GetTimestamp();
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

void Engine::SetPlaybackTempo(double tempo) noexcept {
  if (tempo_ != tempo) {
    tempo_ = tempo;
    transport_.SetTempo(tempo_ * kMinutesFromSeconds);
  }
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

void Engine::StartPlayback() noexcept { transport_.Start(); }

void Engine::StopPlayback() noexcept {
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
