#include "barelymusician/engine/sequencer.h"

#include <algorithm>

#include "barelymusician/base/constants.h"
#include "barelymusician/engine/engine_utils.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

namespace {

double BeatsFromSeconds(double tempo, double seconds) {
  return tempo * seconds / kSecondsFromMinutes;
}

double SecondsFromBeats(double tempo, double beats) {
  return beats * kSecondsFromMinutes / tempo;
}

}  // namespace

Sequencer::Sequencer(Engine* engine)
    : is_playing_(false),
      position_(0.0),
      tempo_(0.0),
      beat_callback_(nullptr),
      engine_(engine) {}

void Sequencer::Create(int instrument_id) {
  messages_.emplace(instrument_id, MessageQueue());
}

void Sequencer::Destroy(int instrument_id) { messages_.erase(instrument_id); }

double Sequencer::GetPosition() const { return position_; }

double Sequencer::GetTempo() const { return tempo_; }

bool Sequencer::IsPlaying() const { return is_playing_; }

void Sequencer::ScheduleNote(int instrument_id, double position,
                             double duration, float index, float intensity) {
  auto* messages = FindOrNull(messages_, instrument_id);
  if (messages == nullptr) {
    return;
  }
  messages->Push(position, NoteOnData{index, intensity});
  messages->Push(position + duration, NoteOffData{index});
}

void Sequencer::ScheduleNoteOff(int instrument_id, double position,
                                float index) {
  auto* messages = FindOrNull(messages_, instrument_id);
  if (messages == nullptr) {
    return;
  }
  messages->Push(position, NoteOffData{index});
}

void Sequencer::ScheduleNoteOn(int instrument_id, double position, float index,
                               float intensity) {
  auto* messages = FindOrNull(messages_, instrument_id);
  if (messages == nullptr) {
    return;
  }
  messages->Push(position, NoteOnData{index, intensity});
}

void Sequencer::SetBeatCallback(BeatCallback beat_callback) {
  beat_callback_ = std::move(beat_callback);
}

void Sequencer::SetPosition(double position) { position_ = position; }

void Sequencer::SetTempo(double tempo) { tempo_ = tempo; }

void Sequencer::Start(double timestamp) {
  start_timestamp_ = timestamp;
  is_playing_ = true;
}

void Sequencer::Stop() {
  is_playing_ = false;
  for (auto& [id, messages] : messages_) {
    messages.Clear();
  }
  for (const auto& [id, message] : messages_) {
    engine_->AllNotesOff(id);
  }
}

void Sequencer::Update(double timestamp, double lookahead) {
  engine_->Update(timestamp);
  if (!is_playing_ || tempo_ == 0.0) {
    return;
  }
  const double elapsed_seconds = std::min(
      timestamp - last_timestamp_, timestamp + lookahead - start_timestamp_);
  if (elapsed_seconds <= 0.0) {
    return;
  }

  const double elapsed_beats = BeatsFromSeconds(tempo_, elapsed_seconds);

  const double begin_timestamp =
      position_ > 0.0 ? last_timestamp_ : start_timestamp_;

  const double begin_position = position_ + lookahead;
  const double end_position = position_ + elapsed_beats + lookahead;

  if (beat_callback_ != nullptr) {
    for (double beat = std::ceil(begin_position); beat < end_position; ++beat) {
      beat_callback_(static_cast<int>(beat));
    }
  }

  for (const auto& [id, scheduled_messages] : messages_) {
    const auto messages =
        scheduled_messages.GetIterator(begin_position, end_position);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const double message_timestamp =
          begin_timestamp + SecondsFromBeats(tempo_, it->timestamp - position_);
      std::visit(
          MessageVisitor{[](const auto&) {},
                         [this, id = id,
                          message_timestamp](const NoteOffData& note_off_data) {
                           engine_->ScheduleNoteOff(id, message_timestamp,
                                                    note_off_data.index);
                         },
                         [this, id = id,
                          message_timestamp](const NoteOnData& note_on_data) {
                           engine_->ScheduleNoteOn(id, message_timestamp,
                                                   note_on_data.index,
                                                   note_on_data.intensity);
                         }},
          it->data);
    }
  }
  last_timestamp_ = timestamp;
  if (elapsed_beats > 0.0) {
    position_ += elapsed_beats;
  }
}

}  // namespace barelyapi
