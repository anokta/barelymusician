#include "barelymusician/engine/engine.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {

Engine::Engine(int sample_rate)
    : clock_(sample_rate),
      is_playing_(false),
      current_position_(0.0),
      previous_position_(0.0),
      beat_callback_(nullptr),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  DCHECK_GE(sample_rate, 0);
}

bool Engine::Create(int performer_id) {
  if (const auto [it, success] = performers_.emplace(performer_id, Performer());
      success) {
    it->second.SetNoteOffCallback([this, performer_id](float index) {
      if (note_off_callback_ != nullptr) {
        note_off_callback_(performer_id, index);
      }
    });
    it->second.SetNoteOnCallback(
        [this, performer_id](float index, float intensity) {
          if (note_on_callback_ != nullptr) {
            note_on_callback_(performer_id, index, intensity);
          }
        });
    return true;
  }
  return false;
}

bool Engine::Destroy(int performer_id) {
  return performers_.erase(performer_id) > 0;
}

double Engine::GetPosition() const { return current_position_; }

double Engine::GetTempo() const { return clock_.GetTempo(); }

bool Engine::IsPlaying() const { return is_playing_; }

bool Engine::NoteOff(int performer_id, float index) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    performer->NoteOff(index);
    return true;
  }
  return false;
}

bool Engine::NoteOn(int performer_id, float index, float intensity) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    performer->NoteOn(index, intensity);
    return true;
  }
  return false;
}

bool Engine::Process(int performer_id, float* output, int num_channels,
                     int num_frames) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    DCHECK(output);
    DCHECK_GE(num_channels, 0);
    DCHECK_GE(num_frames, 0);
    performer->Process(previous_position_, current_position_, output,
                       num_channels, num_frames);
    return true;
  }
  return false;
}

bool Engine::ScheduleNote(int performer_id, const Note& note) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    DCHECK_GE(note.position, 0.0);
    DCHECK_GE(note.duration, 0.0);
    performer->ScheduleNoteOn(note.position, note.index, note.intensity);
    performer->ScheduleNoteOff(note.position + note.duration, note.index);
    return true;
  }
  return false;
}

bool Engine::ScheduleNoteOff(int performer_id, double position, float index) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    DCHECK_GE(position, 0.0);
    performer->ScheduleNoteOff(position, index);
    return true;
  }
  return false;
}

bool Engine::ScheduleNoteOn(int performer_id, double position, float index,
                            float intensity) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    DCHECK_GE(position, 0.0);
    performer->ScheduleNoteOn(position, index, intensity);
    return true;
  }
  return false;
}

void Engine::SetBeatCallback(BeatCallback&& beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

bool Engine::SetInstrument(int performer_id,
                           std::unique_ptr<Instrument> instrument) {
  if (Performer* performer = GetPerformer(performer_id); performer != nullptr) {
    performer->SetInstrument(std::move(instrument));
    return true;
  }
  return false;
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
  previous_position_ = current_position_;
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
    for (double b = std::ceil(previous_position_); b < current_position_; ++b) {
      beat_callback_(static_cast<int>(b));
    }
  }
}

Performer* Engine::GetPerformer(int performer_id) {
  if (const auto it = performers_.find(performer_id);
      it != performers_.cend()) {
    return &it->second;
  } else {
    DLOG(WARNING) << "Invalid performer id: " << performer_id;
  }
  return nullptr;
}

}  // namespace barelyapi