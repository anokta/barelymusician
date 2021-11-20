#include "barelymusician/composition/note_sequence.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

namespace {

// Processes the sequence with a position offset.
void ProcessWithOffset(const std::map<std::pair<double, Id>, Note>& notes,
                       double position_offset, double begin_position,
                       double end_position,
                       const NoteSequence::NoteCallback& note_callback) {
  const auto begin = notes.lower_bound(std::pair{begin_position, kInvalidId});
  const auto end = notes.lower_bound(std::pair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    note_callback(it->first.first + position_offset, it->second);
  }
}

}  // namespace

NoteSequence::NoteSequence()
    : is_looping_(false),
      loop_length_(1.0),
      loop_start_offset_(0.0),
      start_offset_(0.0),
      start_position_(std::nullopt),
      end_position_(std::nullopt) {}

Status NoteSequence::Add(Id id, double position, Note note) {
  if (id == kInvalidId) return Status::kInvalidArgument;
  if (positions_.emplace(id, position).second) {
    notes_.emplace(std::pair{position, id}, std::move(note));
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

double NoteSequence::GetLoopLength() const { return loop_length_; }

double NoteSequence::GetLoopStartOffset() const { return loop_start_offset_; }

double NoteSequence::GetStartOffset() const { return start_offset_; }

std::optional<double> NoteSequence::GetStartPosition() const {
  return start_position_;
}

std::optional<double> NoteSequence::GetEndPosition() const {
  return end_position_;
}

bool NoteSequence::IsEmpty() const { return notes_.empty(); }

bool NoteSequence::IsLooping() const { return is_looping_; }

void NoteSequence::Process(double begin_position, double end_position,
                           const NoteCallback& note_callback) const {
  if (begin_position >= end_position || !note_callback) return;

  double position_offset = -start_offset_;
  if (start_position_) {
    begin_position = std::max(begin_position, *start_position_);
    position_offset += *start_position_;
  }
  if (end_position_) {
    end_position = std::min(end_position, *end_position_);
  }
  if (begin_position >= end_position) return;

  begin_position -= position_offset;
  end_position -= position_offset;

  if (is_looping_) {
    if (loop_length_ <= 0.0) return;

    if (begin_position > loop_length_ + loop_start_offset_) {
      const double loop_offset =
          loop_length_ *
          std::floor((begin_position - loop_start_offset_) / loop_length_);
      begin_position -= loop_offset;
      end_position -= loop_offset;
      position_offset += loop_offset;
    }

    if (begin_position < loop_start_offset_ + loop_length_) {
      const double loop_end_position =
          std::min(loop_start_offset_ + loop_length_, end_position);
      ProcessWithOffset(notes_, position_offset, begin_position,
                        loop_end_position, note_callback);
      position_offset += loop_end_position - begin_position;
      begin_position = loop_end_position;
    }

    while (begin_position < end_position) {
      const double loop_end_position =
          loop_start_offset_ +
          std::min(loop_length_, end_position - begin_position);
      ProcessWithOffset(notes_,
                        position_offset + begin_position - loop_start_offset_,
                        loop_start_offset_, loop_end_position, note_callback);
      begin_position += loop_length_;
    }
  } else {
    ProcessWithOffset(notes_, position_offset, begin_position, end_position,
                      note_callback);
  }
}

Status NoteSequence::Remove(Id id) {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    notes_.erase(std::pair{position_it->second, id});
    positions_.erase(position_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void NoteSequence::SetLoopLength(double loop_length) {
  loop_length_ = std::max(loop_length, 0.0);
}

void NoteSequence::SetLoopStartOffset(double loop_start_offset) {
  loop_start_offset_ = loop_start_offset;
}

void NoteSequence::SetLooping(bool is_looping) { is_looping_ = is_looping; }

void NoteSequence::SetStartOffset(double start_offset) {
  start_offset_ = start_offset;
}

void NoteSequence::SetStartPosition(std::optional<double> start_position) {
  start_position_ = std::move(start_position);
}

void NoteSequence::SetEndPosition(std::optional<double> end_position) {
  end_position_ = std::move(end_position);
}

}  // namespace barelyapi
