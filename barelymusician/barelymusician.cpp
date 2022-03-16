#include "barelymusician/barelymusician.h"

#include <stdbool.h>  // NOLINT(modernize-deprecated-headers)
#include <stdint.h>   // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/conductor/transport.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/mutable_data.h"
#include "barelymusician/performer/find_or_null.h"
#include "barelymusician/performer/sequence.h"

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
      for (const auto& [position, id_event_pair] : id_event_pairs) {
        const auto& [instrument_id, event] = id_event_pair;
        if (auto* instrument = GetInstrument(instrument_id)) {
          instrument->ProcessEvent(event, transport.GetTimestamp(position));
        }
      }
    });
  }

  // Non-copyable and non-movable.
  BarelyMusician(const BarelyMusician& other) = delete;
  BarelyMusician& operator=(const BarelyMusician& other) = delete;
  BarelyMusician(BarelyMusician&& other) noexcept = delete;
  BarelyMusician& operator=(BarelyMusician&& other) noexcept = delete;

  Instrument* GetInstrument(BarelyId instrument_id) const noexcept {
    if (auto it = instruments.find(instrument_id); it != instruments.end()) {
      return it->second.get();
    }
    return nullptr;
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

  const BarelyId instrument_id = ++handle->id_counter;
  *out_instrument_id = instrument_id;

  handle->instruments.emplace(
      instrument_id, std::make_unique<Instrument>(definition, frame_rate));
  handle->UpdateInstrumentMap();

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
  if (!out_value) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->GetInstrument(instrument_id)) {
    *out_value = instrument->GetParameter(index)->GetValue();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_GetParamDefinition(
    BarelyMusicianHandle handle, BarelyId instrument_id, int32_t index,
    BarelyParameterDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  if (const auto* instrument = handle->GetInstrument(instrument_id)) {
    *out_definition = instrument->GetParameter(index)->GetDefinition();
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_IsNoteOn(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch,
                                       bool* out_is_note_on) {
  if (!handle) return BarelyStatus_kNotFound;
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

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->StartNote(pitch, intensity, handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopAllNotes(BarelyMusicianHandle handle,
                                           BarelyId instrument_id) {
  if (!handle) return BarelyStatus_kNotFound;

  if (auto* instrument = handle->GetInstrument(instrument_id)) {
    instrument->StopAllNotes(handle->transport.GetTimestamp());
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyInstrument_StopNote(BarelyMusicianHandle handle,
                                       BarelyId instrument_id, double pitch) {
  if (!handle) return BarelyStatus_kNotFound;

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

BarelyStatus BarelyMusician_Update(BarelyMusicianHandle handle,
                                   double timestamp) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->transport.Update(timestamp);
  return BarelyStatus_kOk;
}

}  // extern "C"
