#include "barelymusician/engine/engine.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {

Engine::Engine(int sample_rate)
    : clock_(sample_rate),
      is_playing_(false),
      current_position_(0.0),
      previous_position_(0.0),
      id_counter_(0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  DCHECK_GE(sample_rate, 0);
}

int Engine::Create(std::unique_ptr<Instrument> instrument) {
  const int performer_id = ++id_counter_;
  Performer performer(std::move(instrument));
  performer.SetNoteOffCallback([this, performer_id](float index) {
    if (note_off_callback_ != nullptr) {
      note_off_callback_(performer_id, index);
    }
  });
  performer.SetNoteOnCallback(
      [this, performer_id](float index, float intensity) {
        if (note_on_callback_ != nullptr) {
          note_on_callback_(performer_id, index, intensity);
        }
      });
  performers_.emplace(performer_id, std::move(performer));
  return performer_id;
}

void Engine::Destroy(int performer_id) { performers_.erase(performer_id); }

double Engine::GetPosition() const { return current_position_; }

double Engine::GetTempo() const { return clock_.GetTempo(); }

bool Engine::IsPlaying() const { return is_playing_; }

void Engine::NoteOff(int performer_id, float index) {
  Performer* performer = GetPerformer(performer_id);
  if (performer == nullptr) {
    LOG(WARNING) << "Invalid performer id: " << performer_id;
    return;
  }
  performer->NoteOff(index);
}

void Engine::NoteOn(int performer_id, float index, float intensity) {
  Performer* performer = GetPerformer(performer_id);
  if (performer == nullptr) {
    LOG(WARNING) << "Invalid performer id: " << performer_id;
    return;
  }
  performer->NoteOn(index, intensity);
}

void Engine::Process(int performer_id, float* output, int num_channels,
                     int num_frames) {
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
  Performer* performer = GetPerformer(performer_id);
  if (performer == nullptr) {
    LOG(WARNING) << "Invalid performer id: " << performer_id;
    return;
  }
  performer->Process(output, num_channels, num_frames, previous_position_,
                     current_position_);
}

void Engine::ScheduleNoteOff(int performer_id, float index, double position) {
  DCHECK_GE(position, 0.0);
  Performer* performer = GetPerformer(performer_id);
  if (performer == nullptr) {
    LOG(WARNING) << "Invalid performer id: " << performer_id;
    return;
  }
  performer->ScheduleNoteOff(index, position);
}

void Engine::ScheduleNoteOn(int performer_id, float index, float intensity,
                            double position) {
  DCHECK_GE(position, 0.0);
  Performer* performer = GetPerformer(performer_id);
  if (performer == nullptr) {
    LOG(WARNING) << "Invalid performer id: " << performer_id;
    return;
  }
  performer->ScheduleNoteOn(index, intensity, position);
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
  current_position_ = clock_.GetPosition();
}

void Engine::SetTempo(double tempo) {
  DCHECK_GE(tempo, 0.0);
  clock_.SetTempo(tempo);
}

void Engine::Start() { is_playing_ = true; }

void Engine::Stop() {
  is_playing_ = false;
  for (auto& [id, performer] : performers_) {
    performer.AllScheduledNotesOff();
  }
}

void Engine::Update(int num_frames) {
  DCHECK_GE(num_frames, 0);
  previous_position_ = current_position_;
  if (!is_playing_) {
    return;
  }
  clock_.UpdatePosition(num_frames);
  current_position_ = clock_.GetPosition();
  if (beat_callback_ != nullptr) {
    for (double beat = std::ceil(previous_position_); beat < current_position_;
         ++beat) {
      beat_callback_(static_cast<int>(beat));
    }
  }
}

Performer* Engine::GetPerformer(int performer_id) {
  if (const auto it = performers_.find(performer_id);
      it != performers_.cend()) {
    return &it->second;
  }
  return nullptr;
}

}  // namespace barelyapi