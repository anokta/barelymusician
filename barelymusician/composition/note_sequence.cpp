#include "barelymusician/composition/note_sequence.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

namespace {

// Dummy note callback that does nothing.
void NoopNoteCallback(double /*position*/, const Note& /*note*/) {}

// Processes the sequence with an offset.
void ProcessWithOffset(const std::map<std::pair<double, Id>, Note>& notes,
                       double offset_position, double begin_position,
                       double end_position,
                       const NoteSequence::NoteCallback& note_callback) {
  const auto begin = notes.lower_bound(std::pair{begin_position, kInvalidId});
  const auto end = notes.lower_bound(std::pair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    note_callback(offset_position + it->first.first, it->second);
  }
}

}  // namespace

NoteSequence::NoteSequence()
    : is_looping_(false),
      loop_length_(1.0),
      start_offset_(0.0),
      note_callback_(&NoopNoteCallback) {}

Status NoteSequence::Add(Id id, double position, Note note) {
  if (id == kInvalidId) {
    return Status::kInvalidArgument;
  }
  if (positions_.emplace(id, position).second) {
    notes_.emplace(std::pair{position, id}, std::move(note));
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

double NoteSequence::GetLoopLength() const { return loop_length_; }

double NoteSequence::GetStartOffset() const { return start_offset_; }

bool NoteSequence::IsEmpty() const { return notes_.empty(); }

bool NoteSequence::IsLooping() const { return is_looping_; }

void NoteSequence::Process(double begin_position, double end_position) {
  double offset = start_offset_;
  if (is_looping_ && loop_length_ > 0.0) {
  }

  const auto begin =
      notes_.lower_bound(std::pair{begin_position + offset, kInvalidId});
  const auto end =
      notes_.lower_bound(std::pair{end_position + offset, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    note_callback_(it->first.first - offset, it->second);
  }

  // double offset = 0.0;
  // if (is_looping_ && loop_length_ > 0.0) {
  //   for (double position = begin_position; position < end_position;
  //        position += loop_length_) {
  //     const double current = loop_length_ * std::floor(position / loop_length_);
  //     offset += current;
  //   }
  // }

  // while (begin_position < end_position) {
  //   if (!is_looping_) {
  //     ProcessWithOffset(notes_, 0.0, begin_position, end_position,
  //                       note_callback_);
  //   }
  // }
  // const auto begin = notes_.lower_bound(std::pair{begin_position, kInvalidId});
  // const auto end = notes_.lower_bound(std::pair{end_position, kInvalidId});
  // for (auto it = begin; it != end; ++it) {
  //   note_callback_(it->first.first, it->second);
  // }
}

Status NoteSequence::Remove(Id id) {
  if (id == kInvalidId) {
    return Status::kInvalidArgument;
  }
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

void NoteSequence::SetLooping(bool is_looping) { is_looping_ = is_looping; }

void NoteSequence::SetNoteCallback(NoteCallback note_callback) {
  note_callback_ = note_callback ? std::move(note_callback) : &NoopNoteCallback;
}

void NoteSequence::SetStartOffset(double start_offset) {
  start_offset_ = start_offset;
}

}  // namespace barelyapi
