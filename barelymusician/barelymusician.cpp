#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/mutable_data.h"
#include "barelymusician/engine/sequence.h"
#include "barelymusician/engine/transport.h"
#include "barelymusician/instruments/percussion_instrument.h"
#include "barelymusician/instruments/synth_instrument.h"

namespace {

using ::barelyapi::Event;
using ::barelyapi::FindOrNull;
using ::barelyapi::Instrument;
using ::barelyapi::MutableData;
using ::barelyapi::Sequence;
using ::barelyapi::Transport;

// Converts minutes from seconds.
constexpr double kMinutesFromSeconds = 1.0 / 60.0;

// Converts seconds from minutes.
constexpr double kSecondsFromMinutes = 60.0;

/// Instrument id-event pair.
using InstrumentIdEventPair = std::pair<BarelyId, Event>;

/// Instrument id-event pair by position map type.
using InstrumentIdEventPairMap = std::multimap<double, InstrumentIdEventPair>;

/// Instrument reference by identifier map.
using InstrumentReferenceMap = std::unordered_map<BarelyId, Instrument*>;

}  // namespace

extern "C" {

/// Musician.
struct BarelyMusician {
  /// Constructs new `BarelyMusician`.
  BarelyMusician() noexcept {
    transport.SetUpdateCallback([this](double begin_position,
                                       double end_position) noexcept {
      InstrumentIdEventPairMap id_event_pairs;
      for (auto& sequence_it : sequences) {
        auto& sequence = sequence_it.second;
        const auto instrument_id = sequence.GetInstrument();
        sequence.SetEventCallback([&](double position, Event event) {
          id_event_pairs.emplace(
              position, InstrumentIdEventPair{instrument_id, std::move(event)});
        });
        sequence.Process(begin_position, end_position);
      }
      for (auto& [position, id_event_pair] : id_event_pairs) {
        auto& [instrument_id, event] = id_event_pair;
        if (auto* instrument = GetInstrument(instrument_id)) {
          instrument->ProcessEvent(std::move(event),
                                   transport.GetTimestamp(position));
        }
      }
    });
  }

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) = delete;
  BarelyMusician& operator=(const BarelyMusician& other) = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;

  BarelyId CreateInstrument(BarelyInstrumentDefinition definition,
                            int32_t frame_rate) noexcept {
    const BarelyId instrument_id = ++id_counter;
    instruments.emplace(instrument_id,
                        std::make_unique<Instrument>(definition, frame_rate));
    UpdateInstrumentMap();
    return instrument_id;
  }

  Instrument* GetInstrument(BarelyId instrument_id) const noexcept {
    if (auto it = instruments.find(instrument_id); it != instruments.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  void Stop() noexcept {
    for (auto& [sequence_id, sequence] : sequences) {
      sequence.Stop();
    }
    transport.Stop();
    for (auto& [instrument_id, instrument] : instruments) {
      instrument->StopAllNotes(transport.GetTimestamp());
    }
  }

  void UpdateInstrumentMap() noexcept {
    InstrumentReferenceMap new_instrument_refs;
    new_instrument_refs.reserve(instruments.size());
    for (const auto& [instrument_id, instrument] : instruments) {
      new_instrument_refs.emplace(instrument_id, instrument.get());
    }
    instrument_refs.Update(std::move(new_instrument_refs));
  }

  // Monotonic identifier counter.
  BarelyId id_counter = 0;

  // Instrument by identifier map.
  std::unordered_map<BarelyId, std::unique_ptr<Instrument>> instruments;

  // Instrument reference by identifier map.
  MutableData<InstrumentReferenceMap> instrument_refs;

  // List of sequences.
  std::unordered_map<BarelyId, Sequence> sequences;

  // Playback transport.
  Transport transport;

 private:
  // Ensures that the instance can only be destroyed via explicit destroy call.
  friend BARELY_EXPORT BarelyStatus
  BarelyMusician_Destroy(BarelyMusicianHandle handle);
  ~BarelyMusician() = default;
};

BarelyStatus BarelyInstrument_Create(BarelyMusicianHandle handle,
                                     BarelyInstrumentDefinition definition,
                                     int32_t frame_rate,
                                     BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  *out_instrument_id = handle->CreateInstrument(definition, frame_rate);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_CreateOfType(BarelyMusicianHandle handle,
                                           BarelyInstrumentType type,
                                           int32_t frame_rate,
                                           BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  switch (type) {
    case BarelyInstrumentType_kSynth:
      *out_instrument_id = handle->CreateInstrument(
          barelyapi::SynthInstrument::GetDefinition(), frame_rate);
      break;
    case BarelyInstrumentType_kPercussion:
      *out_instrument_id = handle->CreateInstrument(
          barelyapi::PercussionInstrument::GetDefinition(), frame_rate);
      break;
    default:
      return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyInstrument_Destroy(BarelyMusicianHandle handle,
                                      BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto it = handle->instruments.find(instrument_id);
      it != handle->instruments.end()) {
    auto instrument = std::move(it->second);
    instrument->StopAllNotes(handle->transport.GetTimestamp());
    for (auto& [sequence_id, sequence] : handle->sequences) {
      if (sequence.GetInstrument() == instrument_id) {
        sequence.SetInstrument(BarelyId_kInvalid);
      }
    }
    handle->instruments.erase(it);
    handle->UpdateInstrumentMap();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetParameter(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index, double* out_value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->GetInstrument(instrument_id)) {
    if (const auto* parameter = instrument->GetParameter(index)) {
      *out_value = parameter->GetValue();
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->GetInstrument(instrument_id)) {
    if (const auto* parameter = instrument->GetParameter(index)) {
      *out_definition = parameter->GetDefinition();
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch,
                                       bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_note_on) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->GetInstrument(instrument_id)) {
    *out_is_note_on = instrument->IsNoteOn(pitch);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_Process(BarelyMusicianHandle handle,
                                      BarelyId instrument_id, double* output,
                                      int32_t num_output_channels,
                                      int32_t num_output_frames,
                                      double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  auto instrument_refs = handle->instrument_refs.GetScopedView();
  if (auto* instrument = FindOrNull(*instrument_refs, instrument_id)) {
    (*instrument)
        ->Process(output, num_output_channels, num_output_frames, timestamp);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetAllParameters(BarelyMusicianHandle handle,
                                                 BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->ResetAllParameters(handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_ResetParameter(BarelyMusicianHandle handle,
                                             BarelyId instrument_id,
                                             int32_t index) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    if (instrument->ResetParameter(index, handle->transport.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetData(BarelyMusicianHandle handle,
                                      BarelyId instrument_id,
                                      BarelyDataDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->SetData(definition, handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOffCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOffCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOffCallback(
          [callback, user_data](double pitch, double timestamp) {
            callback(pitch, timestamp, user_data);
          });
    } else {
      instrument->SetNoteOffCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetNoteOnCallback(
    BarelyMusicianHandle handle, BarelyId instrument_id,
    BarelyInstrument_NoteOnCallback callback, void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    if (callback) {
      instrument->SetNoteOnCallback([callback, user_data](double pitch,
                                                          double intensity,
                                                          double timestamp) {
        callback(pitch, intensity, timestamp, user_data);
      });
    } else {
      instrument->SetNoteOnCallback(nullptr);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_SetParameter(BarelyMusicianHandle handle,
                                           BarelyId instrument_id,
                                           int32_t index, double value) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    if (instrument->SetParameter(index, value, 0.0,
                                 handle->transport.GetTimestamp())) {
      return BarelyStatus_kOk;
    }
    return BarelyStatus_kInvalidArgument;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StartNote(BarelyMusicianHandle handle,
                                        BarelyId instrument_id, double pitch,
                                        double intensity) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->StartNote(pitch, intensity, handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyMusicianHandle handle,
                                           BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->StopAllNotes(handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopNote(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (instrument_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->StopNote(pitch, handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyMusician_Create(BarelyMusicianHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyMusician();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Destroy(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->instrument_refs.Update({});
  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetNote(BarelyMusicianHandle handle,
                                    BarelyNoteDefinition_Pitch pitch,
                                    double* out_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_pitch) return BarelyStatus_kInvalidArgument;

  pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetPosition(BarelyMusicianHandle handle,
                                        double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  *out_position = handle->transport.GetPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetRootNote(BarelyMusicianHandle handle,
                                        double* out_root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_root_pitch) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetScale(BarelyMusicianHandle handle,
                                     double** out_scale_pitches,
                                     int32_t* out_num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_scale_pitches) return BarelyStatus_kInvalidArgument;
  if (!out_num_scale_pitches) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_GetTempo(BarelyMusicianHandle handle,
                                     double* out_tempo) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_tempo) return BarelyStatus_kInvalidArgument;

  *out_tempo = handle->transport.GetTempo() * kSecondsFromMinutes;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestamp(BarelyMusicianHandle handle,
                                         double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->transport.GetTimestamp();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_GetTimestampAtPosition(BarelyMusicianHandle handle,
                                                   double position,
                                                   double* out_timestamp) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_timestamp) return BarelyStatus_kInvalidArgument;

  *out_timestamp = handle->transport.GetTimestamp(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_IsPlaying(BarelyMusicianHandle handle,
                                      bool* out_is_playing) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_playing) return BarelyStatus_kInvalidArgument;

  *out_is_playing = handle->transport.IsPlaying();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetAdjustNoteCallback(
    BarelyMusicianHandle handle, BarelyMusician_AdjustNoteCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetAdjustParameterAutomationCallback(
    BarelyMusicianHandle handle,
    BarelyMusician_AdjustParameterAutomationCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetAdjustTempoCallback(
    BarelyMusicianHandle handle, BarelyMusician_AdjustTempoCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  callback;
  user_data;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetBeatCallback(
    BarelyMusicianHandle handle, BarelyMusician_BeatCallback callback,
    void* user_data) {
  if (!handle) return BarelyStatus_kNotFound;

  if (callback) {
    handle->transport.SetBeatCallback(
        [callback, user_data](double position, double timestamp) {
          callback(position, timestamp, user_data);
        });
  } else {
    handle->transport.SetBeatCallback(nullptr);
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetPosition(BarelyMusicianHandle handle,
                                        double position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.SetPosition(position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetRootNote(BarelyMusicianHandle handle,
                                        double root_pitch) {
  if (!handle) return BarelyStatus_kNotFound;

  root_pitch;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetScale(BarelyMusicianHandle handle,
                                     double* scale_pitches,
                                     int32_t num_scale_pitches) {
  if (!handle) return BarelyStatus_kNotFound;

  scale_pitches;
  num_scale_pitches;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyMusician_SetTempo(BarelyMusicianHandle handle,
                                     double tempo) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.SetTempo(tempo * kMinutesFromSeconds);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_SetTimestamp(BarelyMusicianHandle handle,
                                         double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.SetTimestamp(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Start(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.Start();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Stop(BarelyMusicianHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->Stop();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.Update(timestamp);
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_AddNote(BarelyMusicianHandle handle,
                                    BarelyId sequence_id,
                                    BarelyNoteDefinition definition,
                                    double position, BarelyId* out_note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_note_id = ++handle->id_counter;
    sequence->AddNote(*out_note_id, position, definition);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_AddParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyParameterAutomationDefinition definition, double position,
    BarelyId* out_note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_note_id) return BarelyStatus_kInvalidArgument;

  definition, position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_Create(BarelyMusicianHandle handle,
                                   BarelyId* out_sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_sequence_id) return BarelyStatus_kInvalidArgument;

  *out_sequence_id = ++handle->id_counter;
  handle->sequences.emplace(*out_sequence_id, Sequence{});
  return BarelyStatus_kOk;
}

BarelyStatus BarelySequence_Destroy(BarelyMusicianHandle handle,
                                    BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (const auto sequence_it = handle->sequences.find(sequence_id);
      sequence_it != handle->sequences.end()) {
    const double timestamp = handle->transport.GetTimestamp();
    const auto instrument_id = sequence_it->second.GetInstrument();
    if (auto* instrument = handle->GetInstrument(instrument_id)) {
      for (const double pitch : sequence_it->second.GetActiveNotes()) {
        instrument->ProcessEvent(barelyapi::SetNoteOffEvent{pitch}, timestamp);
      }
    }
    handle->sequences.erase(sequence_it);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginOffset(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double* out_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_begin_offset = sequence->GetBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetBeginPosition(BarelyMusicianHandle handle,
                                             BarelyId sequence_id,
                                             double* out_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_begin_position = sequence->GetBeginPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetEndPosition(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double* out_end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_end_position = sequence->GetEndPosition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetInstrument(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          BarelyId* out_instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_instrument_id) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_instrument_id = sequence->GetInstrument();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopBeginOffset(BarelyMusicianHandle handle,
                                               BarelyId sequence_id,
                                               double* out_loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_loop_begin_offset = sequence->GetLoopBeginOffset();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetLoopLength(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_loop_length = sequence->GetLoopLength();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_GetNoteDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetNotePosition(BarelyMusicianHandle handle,
                                            BarelyId sequence_id,
                                            BarelyId note_id,
                                            double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_GetParameterAutomationPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }
  if (!out_position) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_IsEmpty(BarelyMusicianHandle handle,
                                    BarelyId sequence_id, bool* out_is_empty) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_is_empty = sequence->IsEmpty();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_IsLooping(BarelyMusicianHandle handle,
                                      BarelyId sequence_id,
                                      bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  if (const auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    *out_is_looping = sequence->IsLooping();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotes(BarelyMusicianHandle handle,
                                           BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->RemoveAllNotes();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllNotesAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllNotesAtRange(BarelyMusicianHandle handle,
                                                  BarelyId sequence_id,
                                                  double begin_position,
                                                  double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->RemoveAllNotes(begin_position, end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomations(
    BarelyMusicianHandle handle, BarelyId sequence_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id, double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveAllParameterAutomationsAtRange(
    BarelyMusicianHandle handle, BarelyId sequence_id, double begin_position,
    double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  // TODO(#85): Implement.
  begin_position, end_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_RemoveNote(BarelyMusicianHandle handle,
                                       BarelyId sequence_id, BarelyId note_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    if (sequence->RemoveNote(note_id)) {
      return BarelyStatus_kOk;
    }
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_RemoveParameterAutomation(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetBeginOffset(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetBeginOffset(begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetBeginPosition(BarelyMusicianHandle handle,
                                             BarelyId sequence_id,
                                             double begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetBeginPosition(begin_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetEndPosition(BarelyMusicianHandle handle,
                                           BarelyId sequence_id,
                                           double end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetEndPosition(end_position);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetInstrument(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    if (sequence->GetInstrument() != instrument_id) {
      if (auto* instrument = handle->GetInstrument(sequence->GetInstrument())) {
        for (const double pitch : sequence->GetActiveNotes()) {
          instrument->ProcessEvent(barelyapi::SetNoteOffEvent{pitch},
                                   handle->transport.GetTimestamp());
        }
      }
      sequence->SetInstrument(instrument_id);
    }
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopBeginOffset(BarelyMusicianHandle handle,
                                               BarelyId sequence_id,
                                               double loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetLoopBeginOffset(loop_begin_offset);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLoopLength(BarelyMusicianHandle handle,
                                          BarelyId sequence_id,
                                          double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetLoopLength(loop_length);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetLooping(BarelyMusicianHandle handle,
                                       BarelyId sequence_id, bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid) return BarelyStatus_kInvalidArgument;

  if (auto* sequence = FindOrNull(handle->sequences, sequence_id)) {
    sequence->SetLooping(is_looping);
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelySequence_SetNoteDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id, BarelyId note_id,
    BarelyNoteDefinition /*definition*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetNotePosition(BarelyMusicianHandle handle,
                                            BarelyId sequence_id,
                                            BarelyId note_id,
                                            double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid || note_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetParameterAutomationDefinition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id,
    BarelyParameterAutomationDefinition /*definition*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelySequence_SetParameterAutomationPosition(
    BarelyMusicianHandle handle, BarelyId sequence_id,
    BarelyId parameter_automation_id, double /*position*/) {
  if (!handle) return BarelyStatus_kNotFound;
  if (sequence_id == BarelyId_kInvalid ||
      parameter_automation_id == BarelyId_kInvalid) {
    return BarelyStatus_kInvalidArgument;
  }

  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
