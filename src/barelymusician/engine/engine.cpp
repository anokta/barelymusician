#include "barelymusician/engine/engine.h"

#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

Engine::Engine(int sample_rate)
    : clock_(sample_rate),
      is_playing_(false),
      last_position_(0.0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  DCHECK_GE(sample_rate, 0);
}

void Engine::Create(int instrument_id, std::unique_ptr<Instrument> instrument) {
  const auto [it, success] = instruments_.insert_or_assign(
      instrument_id, InstrumentData{std::move(instrument)});
  if (!success) {
    DLOG(WARNING) << "Overwriting existing instrument id: " << instrument_id;
  }
}

void Engine::Destroy(int instrument_id) {
  if (instruments_.erase(instrument_id) == 0) {
    DLOG(WARNING) << "Instrument id does not exist: " << instrument_id;
  }
}

double Engine::GetPosition() const { return clock_.GetPosition(); }

double Engine::GetTempo() const { return clock_.GetTempo(); }

bool Engine::IsPlaying() const { return is_playing_; }

void Engine::NoteOff(int instrument_id, float index) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  instrument_data->instrument->NoteOff(index);
  if (note_off_callback_ != nullptr) {
    note_off_callback_(instrument_id, index);
  }
}

void Engine::NoteOn(int instrument_id, float index, float intensity) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  instrument_data->instrument->NoteOn(index, intensity);
  if (note_on_callback_ != nullptr) {
    note_on_callback_(instrument_id, index, intensity);
  }
}

void Engine::Process(int instrument_id, float* output, int num_channels,
                     int num_frames) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  int frame = 0;
  // Process mmessages.
  const double position = clock_.GetPosition();
  if (last_position_ < position) {
    const auto messages =
        instrument_data->messages.GetIterator(last_position_, position);
    const double frames_per_beat =
        static_cast<double>(num_frames) / (position - last_position_);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const int message_frame =
          static_cast<int>(frames_per_beat * (it->position - last_position_));
      if (frame < message_frame) {
        instrument_data->instrument->Process(
            &output[num_channels * frame], num_channels, message_frame - frame);
        frame = message_frame;
      }
      std::visit(
          MessageVisitor{
              [&](const NoteOffData& data) {
                instrument_data->instrument->NoteOff(data.index);
                if (note_off_callback_ != nullptr) {
                  note_off_callback_(instrument_id, data.index);
                }
                instrument_data->scheduled_note_indices.erase(data.index);
              },
              [&](const NoteOnData& data) {
                instrument_data->instrument->NoteOn(data.index, data.intensity);
                if (note_on_callback_ != nullptr) {
                  note_on_callback_(instrument_id, data.index, data.intensity);
                }
                instrument_data->scheduled_note_indices.insert(data.index);
              }},
          it->data);
    }
    instrument_data->messages.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument_data->instrument->Process(&output[num_channels * frame],
                                         num_channels, num_frames - frame);
  }
}

void Engine::ScheduleNote(int instrument_id, double position, double duration,
                          float index, float intensity) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  DCHECK_GE(position, 0.0);
  DCHECK_GE(duration, 0.0);
  if (position < last_position_) {
    DLOG(WARNING) << "Playback is ahead of scheduled position: "
                  << last_position_ << " > " << position;
  }
  instrument_data->messages.Push(position, NoteOnData{index, intensity});
  instrument_data->messages.Push(position + duration, NoteOffData{index});
}

void Engine::ScheduleNoteOff(int instrument_id, double position, float index) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  DCHECK_GE(position, 0.0);
  if (position < last_position_) {
    DLOG(WARNING) << "Playback is ahead of scheduled position: "
                  << last_position_ << " > " << position;
  }
  instrument_data->messages.Push(position, NoteOffData{index});
}

void Engine::ScheduleNoteOn(int instrument_id, double position, float index,
                            float intensity) {
  InstrumentData* instrument_data = GetInstrumentData(instrument_id);
  if (instrument_data == nullptr) {
    return;
  }
  DCHECK_GE(position, 0.0);
  if (position < last_position_) {
    DLOG(WARNING) << "Playback is ahead of scheduled position: "
                  << last_position_ << " > " << position;
  }
  instrument_data->messages.Push(position, NoteOnData{index, intensity});
}

void Engine::SetBeatCallback(BeatCallback&& beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Engine::SetNoteOffCallback(NoteOffCallback&& note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void Engine::SetNoteOnCallback(NoteOnCallback&& note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

void Engine::SetPosition(double position) {
  DCHECK_GE(position, 0.0);
  clock_.SetPosition(position);
}

void Engine::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  clock_.SetTempo(tempo);
}

void Engine::Start() { is_playing_ = true; }

void Engine::Stop() {
  is_playing_ = false;
  for (auto& [instrument_id, instrument_data] : instruments_) {
    for (const float index : instrument_data.scheduled_note_indices) {
      instrument_data.instrument->NoteOff(index);
      if (note_off_callback_ != nullptr) {
        note_off_callback_(instrument_id, index);
      }
    }
    instrument_data.scheduled_note_indices.clear();
  }
}

void Engine::Update(int num_frames) {
  DCHECK_GE(num_frames, 0);
  last_position_ = clock_.GetPosition();
  if (!is_playing_) {
    return;
  }
  clock_.UpdatePosition(num_frames);
  if (beat_callback_ != nullptr) {
    const double position = clock_.GetPosition();
    for (double beat = std::ceil(last_position_); beat < position; ++beat) {
      beat_callback_(static_cast<int>(beat));
    }
  }
}

Engine::InstrumentData* Engine::GetInstrumentData(int instrument_id) {
  const auto it = instruments_.find(instrument_id);
  if (it == instruments_.cend()) {
    DLOG(ERROR) << "Invalid instrument id: " << instrument_id;
    return nullptr;
  }
  return &it->second;
}

}  // namespace barelyapi