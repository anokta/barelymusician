
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"

namespace barely::internal {

Engine::~Engine() noexcept {
  for (auto& [instrument_id, instrument] : instruments_) {
    instrument->StopAllNotes(timestamp_);
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
bool Engine::DestroyInstrument(Id instrument_id) noexcept {
  if (const auto it = instruments_.find(instrument_id);
      it != instruments_.end()) {
    auto instrument = std::move(it->second);
    instrument->StopAllNotes(timestamp_);
    instruments_.erase(it);
    UpdateInstrumentReferenceMap();
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

double Engine::GetTimestamp() const noexcept { return timestamp_; }

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
    assert(*instrument_ref);
    (*instrument_ref)
        ->Process(output, num_output_channels, num_output_frames, timestamp);
    return true;
  }
  return false;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void Engine::Update(double timestamp) noexcept {
  assert(timestamp >= 0.0);
  if (timestamp_ < timestamp) {
    timestamp_ = timestamp;
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

}  // namespace barely::internal
