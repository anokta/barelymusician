#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/find_or_null.h"
#include "barelymusician/instrument.h"
#include "barelymusician/instrument_event.h"
#include "barelymusician/parameter.h"
#include "barelymusician/sequence.h"
#include "barelymusician/transport.h"

namespace {

using ::barelyapi::FindOrNull;
using ::barelyapi::Instrument;
using ::barelyapi::InstrumentEvent;
using ::barelyapi::Sequence;
using ::barelyapi::Transport;

// Converts minutes from seconds.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts seconds from minutes.
constexpr double kSecondsFromMinutes = 60.0;

/// Instrument id-event pair.
using InstrumentIdEventPair = std::pair<BarelyId, InstrumentEvent>;

/// Instrument id-event pair by position map type.
using InstrumentIdEventPairMap = std::multimap<double, InstrumentIdEventPair>;

/// Instrument reference by identifier map.
using InstrumentReferenceMap = std::unordered_map<BarelyId, Instrument*>;

}  // namespace

extern "C" {

/// BarelyMusician C api.
struct BarelyMusician {
  /// Constructs new `BarelyMusician`.
  BarelyMusician()
      : instrument_refs_holder{std::make_unique<InstrumentReferenceMap>()},
        instrument_refs_ptr(instrument_refs_holder.get()) {
    transport.SetUpdateCallback([this](double begin_position,
                                       double end_position) noexcept {
      InstrumentIdEventPairMap id_event_pairs;
      for (auto& sequence_it : sequences) {
        auto& sequence = sequence_it.second;
        const auto instrument_id = sequence.GetInstrument();
        sequence.SetEventCallback([&](double position, InstrumentEvent event) {
          id_event_pairs.emplace(position,
                                 InstrumentIdEventPair{instrument_id, event});
        });
        sequence.Process(begin_position, end_position);
      }
      for (const auto& [position, id_event_pair] : id_event_pairs) {
        const auto& [instrument_id, event] = id_event_pair;
        if (auto* instrument = GetInstrument(instrument_id)) {
          instrument->ProcessEvent(event, transport.GetTimestamp(position));
        }
      }
    });
  }

  Instrument* GetInstrument(BarelyId instrument_id) const {
    if (auto it = instruments.find(instrument_id); it != instruments.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  void UpdateInstrumentMap() {
    auto instrument_refs = std::make_unique<InstrumentReferenceMap>();
    instrument_refs->reserve(instruments.size());
    for (const auto& [instrument_id, instrument] : instruments) {
      instrument_refs->emplace(instrument_id, instrument.get());
    }
    for (auto* expected = instrument_refs_holder.get();
         !instrument_refs_ptr.compare_exchange_strong(expected,
                                                      instrument_refs.get());) {
      expected = instrument_refs_holder.get();
    }
    instrument_refs_holder = std::move(instrument_refs);
  }

  // Monotonic identifier counter.
  BarelyId id_counter = 0;

  // Instrument by identifier map.
  std::unordered_map<BarelyId, std::unique_ptr<Instrument>> instruments;

  // Instrument reference by identifier map.
  std::unique_ptr<InstrumentReferenceMap> instrument_refs_holder;
  std::atomic<InstrumentReferenceMap*> instrument_refs_ptr;

  // List of sequences.
  std::unordered_map<BarelyId, Sequence> sequences;

  // Playback transport.
  Transport transport;

 private:
  // Ensures that the instance can only be destroyed via the api call.
  friend BARELY_EXPORT BarelyStatus BarelyMusician_Destroy(BarelyApi api);
  ~BarelyMusician() = default;

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) = delete;
  BarelyMusician& operator=(const BarelyMusician& other) = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;
};

BarelyStatus BarelyInstrument_Create(BarelyApi api,
                                     BarelyInstrumentDefinition definition,
                                     int32_t sample_rate,
                                     BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  const BarelyId instrument_id = ++api->id_counter;
  *out_instrument_id = instrument_id;

  api->instruments.emplace(
      instrument_id, std::make_unique<Instrument>(definition, sample_rate));
  api->UpdateInstrumentMap();

  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyApi api, BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto it = api->instruments.find(instrument_id);
      it != api->instruments.end()) {
    auto instrument = std::move(it->second);
    for (auto& [sequence_id, sequence] : api->sequences) {
      if (sequence.GetInstrument() == instrument_id) {
        sequence.SetInstrument(BarelyId_kInvalid);
      }
    }
    instrument->StopAllNotes(api->transport.GetTimestamp());

    api->instruments.erase(it);
    api->UpdateInstrumentMap();

    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetGain(BarelyApi api, BarelyId instrument_id,
                                      float* out_gain) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_gain) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = api->GetInstrument(instrument_id)) {
    *out_gain = instrument->GetGain();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetParameter(BarelyApi api,
                                           BarelyId instrument_id,
                                           int32_t index, float* out_value) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = api->GetInstrument(instrument_id)) {
    *out_value = instrument->GetParameter(index)->GetValue();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyApi api, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = api->GetInstrument(instrument_id)) {
    *out_definition = instrument->GetParameter(index)->GetDefinition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_IsMuted(BarelyApi api, BarelyId instrument_id,
                                      bool* out_is_muted) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_muted) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = api->GetInstrument(instrument_id)) {
    *out_is_muted = instrument->IsMuted();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyApi api, BarelyId instrument_id,
                                       float pitch, bool* out_is_note_on) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = api->GetInstrument(instrument_id)) {
    *out_is_note_on = instrument->IsNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_Process(BarelyApi api, BarelyId instrument_id,
                                      double timestamp, float* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames) {
  if (!api) return BarelyStatus_kNotFound;

  auto instrument_refs = api->instrument_refs_ptr.exchange(nullptr);
  auto status = BarelyStatus_kNotFound;
  if (auto* instrument = FindOrNull(*instrument_refs, instrument_id)) {
    (*instrument)
        ->Process(output, num_output_channels, num_output_frames, timestamp);
    status = BarelyStatus_kOk;
  }
  api->instrument_refs_ptr = instrument_refs;
  return status;
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyApi api,
                                                 BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->ResetAllParameters(api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyApi api,
                                             BarelyId instrument_id,
                                             int32_t index) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    if (instrument->ResetParameter(index, api->transport.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetData(BarelyApi api, BarelyId instrument_id,
                                      void* data) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->SetData(data, api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetGain(BarelyApi api, BarelyId instrument_id,
                                      float gain) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->SetGain(gain, api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetMuted(BarelyApi api, BarelyId instrument_id,
                                       bool is_muted) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->SetMuted(is_muted, api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback note_off_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->SetNoteOffCallback(note_off_callback, user_data);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyApi api, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback note_on_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->SetNoteOnCallback(note_on_callback, user_data);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyApi api,
                                           BarelyId instrument_id,
                                           int32_t index, float value) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    if (instrument->SetParameter(index, value, api->transport.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StartNote(BarelyApi api, BarelyId instrument_id,
                                        float pitch, float intensity) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->StartNote(pitch, intensity, api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyApi api,
                                           BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->StopAllNotes(api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopNote(BarelyApi api, BarelyId instrument_id,
                                       float pitch) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* instrument = api->GetInstrument(instrument_id)) {
    instrument->StopNote(pitch, api->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyMusician_Create(BarelyApi* out_api) {
  if (!out_api) return BarelyStatus_kInvalidArgument;

  *out_api = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Destroy(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  delete api;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetNoteDuration(
    BarelyApi api, BarelyNoteDurationDefinition /*definition*/,
    bool /*bypass_adjustment*/, double* out_duration) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_duration) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetNoteIntensity(
    BarelyApi api, BarelyNoteIntensityDefinition /*definition*/,
    bool /*bypass_adjustment*/, float* out_intensity) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_intensity) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetNotePitch(
    BarelyApi api, BarelyNotePitchDefinition /*definition*/,
    bool /*bypass_adjustment*/, float* out_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetPosition(BarelyApi api, double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = api->transport.GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetRootNote(BarelyApi api, float* out_root_pitch) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetScale(BarelyApi api, float** out_scale_pitches,
                                     int32_t* out_num_scale_pitches) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetTempo(BarelyApi api, double* out_tempo) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = api->transport.GetTempo() * kSecondsFromMinutes;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyApi api, double* out_timestamp) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = api->transport.GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestampAtPosition(BarelyApi api,
                                                   double position,
                                                   double* out_timestamp) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = api->transport.GetTimestamp(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_IsPlaying(BarelyApi api, bool* out_is_playing) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = api->transport.IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyApi api, BarelyMusician_BeatCallback beat_callback, void* user_data) {
  if (!api) return BarelyStatus_kNotFound;

  if (beat_callback) {
    api->transport.SetBeatCallback(
        [beat_callback, user_data](double position, double timestamp) {
          beat_callback(position, timestamp, user_data);
        });
  } else {
    api->transport.SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetNoteDurationAdjustmentCallback(
    BarelyApi api,
    BarelyMusician_NoteDurationAdjustmentCallback /*adjustment_callback*/,
    void* /*user_data*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetNoteIntensityAdjustmentCallback(
    BarelyApi api,
    BarelyMusician_NoteIntensityAdjustmentCallback /*adjustment_callback*/,
    void* /*user_data*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetNotePitchAdjustmentCallback(
    BarelyApi api,
    BarelyMusician_NotePitchAdjustmentCallback /*adjustment_callback*/,
    void* /*user_data*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetPosition(BarelyApi api, double position) {
  if (!api) return BarelyStatus_kNotFound;

  api->transport.SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetRootNote(BarelyApi api, float /*root_pitch*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetScale(BarelyApi api, float* /*scale_pitches*/,
                                     int32_t /*num_scale_pitches*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetTempo(BarelyApi api, double tempo) {
  if (!api) return BarelyStatus_kNotFound;

  api->transport.SetTempo(tempo * kMinutesFromSeconds);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTimestamp(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;

  api->transport.SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Start(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  api->transport.Start();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Stop(BarelyApi api) {
  if (!api) return BarelyStatus_kNotFound;

  for (auto& [sequence_id, sequence] : api->sequences) {
    sequence.Stop();
  }
  api->transport.Stop();
  for (auto& [instrument_id, instrument] : api->instruments) {
    instrument->StopAllNotes(api->transport.GetTimestamp());
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyApi api, double timestamp) {
  if (!api) return BarelyStatus_kNotFound;
  api->transport.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_AddNote(BarelyApi api, BarelyId sequence_id,
                                    double position,
                                    BarelyNoteDefinition definition,
                                    BarelyId* out_note_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_note_id = ++api->id_counter;
    sequence->AddNote(*out_note_id, position, std::move(definition));
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_Create(BarelyApi api, BarelyId* out_sequence_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  *out_sequence_id = ++api->id_counter;
  api->sequences.emplace(*out_sequence_id, Sequence{});
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_Destroy(BarelyApi api, BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (const auto sequence_it = api->sequences.find(sequence_id);
      sequence_it != api->sequences.end()) {
    const double timestamp = api->transport.GetTimestamp();
    const auto instrument_id = sequence_it->second.GetInstrument();
    if (auto* instrument = api->GetInstrument(instrument_id)) {
      for (const float pitch : sequence_it->second.GetActiveNotes()) {
        instrument->ProcessEvent(barelyapi::StopNoteEvent{pitch}, timestamp);
      }
    }
    api->sequences.erase(sequence_it);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginOffset(BarelyApi api, BarelyId sequence_id,
                                           double* out_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_begin_offset = sequence->GetBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double* out_begin_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_begin_position = sequence->GetBeginPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double* out_end_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_end_position = sequence->GetEndPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetInstrument(BarelyApi api, BarelyId sequence_id,
                                          BarelyId* out_instrument_id) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_instrument_id = sequence->GetInstrument();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopBeginOffset(BarelyApi api,
                                               BarelyId sequence_id,
                                               double* out_loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_loop_begin_offset = sequence->GetLoopBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double* out_loop_length) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_loop_length = sequence->GetLoopLength();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyApi api, BarelyId /*sequence_id*/, BarelyId /*note_id*/,
    BarelyNoteDefinition* out_definition) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetNotePosition(BarelyApi api,
                                            BarelyId /*sequence_id*/,
                                            BarelyId /*note_id*/,
                                            double* out_position) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_IsEmpty(BarelyApi api, BarelyId sequence_id,
                                    bool* out_is_empty) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_is_empty = sequence->IsEmpty();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_IsLooping(BarelyApi api, BarelyId sequence_id,
                                      bool* out_is_looping) {
  if (!api) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    *out_is_looping = sequence->IsLooping();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotes(BarelyApi api,
                                           BarelyId sequence_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->RemoveAllNotes();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotesAtPosition(BarelyApi api,
                                                     BarelyId /*sequence_id*/,
                                                     double /*position*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllNotesAtRange(BarelyApi api,
                                                  BarelyId sequence_id,
                                                  double begin_position,
                                                  double end_position) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->RemoveAllNotes(begin_position, end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveNote(BarelyApi api, BarelyId sequence_id,
                                       BarelyId note_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->RemoveNote(note_id);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetBeginOffset(BarelyApi api, BarelyId sequence_id,
                                           double begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetBeginOffset(begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetBeginPosition(BarelyApi api,
                                             BarelyId sequence_id,
                                             double begin_position) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetBeginPosition(begin_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetEndPosition(BarelyApi api, BarelyId sequence_id,
                                           double end_position) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetEndPosition(end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetInstrument(BarelyApi api, BarelyId sequence_id,
                                          BarelyId instrument_id) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    if (sequence->GetInstrument() != instrument_id) {
      if (auto* instrument = api->GetInstrument(sequence->GetInstrument())) {
        for (const float pitch : sequence->GetActiveNotes()) {
          instrument->ProcessEvent(barelyapi::StopNoteEvent{pitch},
                                   api->transport.GetTimestamp());
        }
      }
      sequence->SetInstrument(instrument_id);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopBeginOffset(BarelyApi api,
                                               BarelyId sequence_id,
                                               double loop_begin_offset) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetLoopBeginOffset(loop_begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopLength(BarelyApi api, BarelyId sequence_id,
                                          double loop_length) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetLoopLength(loop_length);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLooping(BarelyApi api, BarelyId sequence_id,
                                       bool is_looping) {
  if (!api) return BarelyStatus_kNotFound;

  if (auto* sequence = FindOrNull(api->sequences, sequence_id)) {
    sequence->SetLooping(is_looping);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetNoteDefinition(
    BarelyApi api, BarelyId /*sequence_id*/, BarelyId /*note_id*/,
    BarelyNoteDefinition /*definition*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetNotePosition(BarelyApi api,
                                            BarelyId /*sequence_id*/,
                                            BarelyId /*note_id*/,
                                            double /*position*/) {
  if (!api) return BarelyStatus_kNotFound;

  // TODO(#85): Implement.
  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
