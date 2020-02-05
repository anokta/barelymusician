#include "barelymusician/engine/performer.h"

#include <variant>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/message.h"

namespace barelyapi {

Performer::Performer(std::unique_ptr<Instrument> instrument)
    : instrument_(std::move(instrument)),
      note_off_callback_(nullptr),
      note_on_callback_(nullptr) {
  DCHECK(instrument_);
}

void Performer::AllScheduledNotesOff() {
  for (const float index : scheduled_note_indices_) {
    NoteOff(index);
  }
  scheduled_note_indices_.clear();
}

void Performer::NoteOff(float index) {
  instrument_->NoteOff(index);
  if (note_off_callback_ != nullptr) {
    note_off_callback_(index);
  }
}

void Performer::NoteOn(float index, float intensity) {
  instrument_->NoteOn(index, intensity);
  if (note_on_callback_ != nullptr) {
    note_on_callback_(index, intensity);
  }
}

void Performer::Process(float* output, int num_channels, int num_frames,
                        double start_position, double end_position) {
  DCHECK(output);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);

  int frame = 0;
  // Process mmessages.
  if (start_position < end_position) {
    const auto messages = messages_.GetIterator(start_position, end_position);
    const double frames_per_beat =
        static_cast<double>(num_frames) / (end_position - start_position);
    for (auto it = messages.cbegin; it != messages.cend; ++it) {
      const int message_frame =
          static_cast<int>(frames_per_beat * (it->position - start_position));
      if (frame < message_frame) {
        instrument_->Process(&output[num_channels * frame], num_channels,
                             message_frame - frame);
        frame = message_frame;
      }
      std::visit(MessageVisitor{[&](const NoteOffData& data) {
                                  NoteOff(data.index);
                                  scheduled_note_indices_.erase(data.index);
                                },
                                [&](const NoteOnData& data) {
                                  NoteOn(data.index, data.intensity);
                                  scheduled_note_indices_.insert(data.index);
                                }},
                 it->data);
    }
    messages_.Clear(messages);
  }
  // Process the rest of the buffer.
  if (frame < num_frames) {
    instrument_->Process(&output[num_channels * frame], num_channels,
                         num_frames - frame);
  }
}

void Performer::ScheduleNoteOff(float index, double position) {
  messages_.Push({NoteOffData{index}, position});
}

void Performer::ScheduleNoteOn(float index, float intensity, double position) {
  messages_.Push({NoteOnData{index, intensity}, position});
}

void Performer::SetNoteOffCallback(NoteOffCallback&& note_off_callback) {
  note_off_callback_ = std::move(note_off_callback);
}

void Performer::SetNoteOnCallback(NoteOnCallback&& note_on_callback) {
  note_on_callback_ = std::move(note_on_callback);
}

}  // namespace barelyapi