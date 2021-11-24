#include "barelymusician/composition/sequence.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

Sequence::Sequence()
    : begin_offset_(0.0),
      is_looping_(false),
      loop_begin_offset_(0.0),
      loop_length_(1.0) {}

Status Sequence::AddNote(Id id, double position, Note note) {
  if (id == kInvalidId) return Status::kInvalidArgument;
  if (positions_.emplace(id, position).second) {
    notes_.emplace(NotePositionIdPair{position, id}, std::move(note));
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

std::vector<Sequence::NoteWithPositionIdPair> Sequence::GetAllNotes() const {
  std::vector<NoteWithPositionIdPair> notes;
  if (!notes_.empty()) {
    notes.reserve(notes_.size());
    for (const auto& [position_id_pair, note] : notes_) {
      notes.emplace_back(position_id_pair, note);
    }
  }
  return notes;
}

double Sequence::GetBeginOffset() const { return begin_offset_; }

double Sequence::GetLoopBeginOffset() const { return loop_begin_offset_; }

double Sequence::GetLoopLength() const { return loop_length_; }

StatusOr<Sequence::NoteWithPosition> Sequence::GetNote(Id id) const {
  if (const auto* position = FindOrNull(positions_, id)) {
    return NoteWithPosition{
        *position, notes_.find(NotePositionIdPair{*position, id})->second};
  }
  return Status::kNotFound;
}

bool Sequence::IsEmpty() const { return notes_.empty(); }

bool Sequence::IsLooping() const { return is_looping_; }

void Sequence::Process(double begin_position, double end_position,
                       double position_offset,
                       const ProcessCallback& process_callback) const {
  position_offset -= begin_offset_;
  begin_position -= position_offset;
  end_position -= position_offset;
  if (is_looping_) {
    if (loop_length_ <= 0.0) {
      return;
    }
    // Move the process position to the first loop iteration.
    if (const double loop_begin_position = begin_position - loop_begin_offset_;
        loop_begin_position > loop_length_) {
      const double loop_offset =
          loop_length_ * std::floor(loop_begin_position / loop_length_);
      begin_position -= loop_offset;
      end_position -= loop_offset;
      position_offset += loop_offset;
    }
    // Process the first loop iteration.
    if (double loop_end_position = loop_begin_offset_ + loop_length_;
        begin_position < loop_end_position) {
      loop_end_position = std::min(loop_end_position, end_position);
      ProcessInternal(begin_position, loop_end_position, position_offset,
                      process_callback);
      begin_position = loop_end_position;
    }
    // Process the rest of the loop iterations.
    position_offset -= loop_begin_offset_;
    while (begin_position < end_position) {
      const double loop_end_position =
          loop_begin_offset_ +
          std::min(loop_length_, end_position - begin_position);
      ProcessInternal(loop_begin_offset_, loop_end_position,
                      position_offset + begin_position, process_callback);
      begin_position += loop_length_;
    }
  } else {
    ProcessInternal(begin_position, end_position, position_offset,
                    process_callback);
  }
}

void Sequence::RemoveAllNotes() {
  notes_.clear();
  positions_.clear();
}

void Sequence::RemoveAllNotes(double begin_position, double end_position) {
  const auto begin =
      notes_.lower_bound(NotePositionIdPair{begin_position, kInvalidId});
  const auto end =
      notes_.lower_bound(NotePositionIdPair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    positions_.erase(it->first.second);
  }
  notes_.erase(begin, end);
}

Status Sequence::RemoveNote(Id id) {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    notes_.erase(NotePositionIdPair{position_it->second, id});
    positions_.erase(position_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Sequence::SetBeginOffset(double begin_offset) {
  begin_offset_ = begin_offset;
}

void Sequence::SetLoopBeginOffset(double loop_begin_offset) {
  loop_begin_offset_ = loop_begin_offset;
}

void Sequence::SetLoopLength(double loop_length) {
  loop_length_ = std::max(loop_length, 0.0);
}

void Sequence::SetLooping(bool is_looping) { is_looping_ = is_looping; }

void Sequence::ProcessInternal(double begin_position, double end_position,
                               double position_offset,
                               const ProcessCallback& process_callback) const {
  const auto begin =
      notes_.lower_bound(NotePositionIdPair{begin_position, kInvalidId});
  const auto end =
      notes_.lower_bound(NotePositionIdPair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    process_callback(it->first.first + position_offset, it->second);
  }
}

}  // namespace barelyapi
