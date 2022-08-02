
#include "barelymusician/engine/engine.h"

#include <cassert>
#include <limits>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/sequencer.h"

namespace barely::internal {

namespace {

// Converts seconds to minutes.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts minutes to seconds.
constexpr double kSecondsFromMinutes = 60.0;

}  // namespace

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
bool Engine::CreateSequencer(Id sequencer_id, int priority) noexcept {
  assert(sequencer_id > kInvalid);
  if (const auto [it, success] =
          sequencers_.emplace(std::pair{priority, sequencer_id}, Sequencer{});
      success) {
    sequencer_refs_.emplace(sequencer_id, std::pair{priority, &it->second});
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

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Engine::DestroySequencer(Id sequencer_id) noexcept {
  if (const auto it = sequencer_refs_.find(sequencer_id);
      it != sequencer_refs_.end()) {
    sequencers_.erase(std::pair{it->second.first, sequencer_id});
    sequencer_refs_.erase(it);
    return true;
  }
  return false;
}

double Engine::GetBeats(double seconds) const noexcept {
  return tempo_ * seconds * kMinutesFromSeconds;
}

Instrument* Engine::GetInstrument(Id instrument_id) noexcept {
  if (auto* instrument = FindOrNull(instruments_, instrument_id)) {
    return instrument->get();
  }
  return nullptr;
}

Sequencer* Engine::GetSequencer(Id sequencer_id) noexcept {
  if (auto* sequencer_ref = FindOrNull(sequencer_refs_, sequencer_id)) {
    return sequencer_ref->second;
  }
  return nullptr;
}

double Engine::GetSeconds(double beats) const noexcept {
  return (tempo_ > 0.0)  ? beats * kSecondsFromMinutes / tempo_
         : (beats > 0.0) ? std::numeric_limits<double>::max()
         : (beats < 0.0) ? std::numeric_limits<double>::lowest()
                         : 0.0;
}

double Engine::GetTempo() const noexcept { return tempo_; }

void Engine::SetTempo(double tempo) noexcept {
  assert(tempo_ >= 0.0);
  tempo_ = tempo;
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
  while (timestamp_ < timestamp) {
    double update_duration = GetBeats(timestamp - timestamp_);
    bool has_events_to_trigger = false;
    for (const auto& [priority_id_pair, sequencer] : sequencers_) {
      if (const double duration = sequencer.GetDurationToNextEvent();
          duration < update_duration) {
        update_duration = duration;
        has_events_to_trigger = true;
      }
    }

    timestamp_ += GetSeconds(update_duration);
    for (auto& [priority_id_pair, sequencer] : sequencers_) {
      sequencer.Update(update_duration);
    }

    if (has_events_to_trigger) {
      for (auto& [priority_id_pair, sequencer] : sequencers_) {
        if (update_duration + sequencer.GetDurationToNextEvent() <=
            sequencer.GetPosition()) {
          sequencer.TriggerAllEventsAtCurrentPosition();
        }
      }
    }
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
