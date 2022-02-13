#include "barelymusician/composition/sequence.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/id.h"

namespace barelyapi {

bool Sequence::AddNote(Id id, double position,
                       BarelyNoteDefinition note) noexcept {
  if (positions_.emplace(id, position).second) {
    notes_.emplace(NotePositionIdPair{position, id}, std::move(note));
    return true;
  }
  return false;
}

double Sequence::GetBeginOffset() const noexcept { return begin_offset_; }

double Sequence::GetBeginPosition() const noexcept { return begin_position_; }

double Sequence::GetEndPosition() const noexcept { return end_position_; }

double Sequence::GetLoopBeginOffset() const noexcept {
  return loop_begin_offset_;
}

double Sequence::GetLoopLength() const noexcept { return loop_length_; }

bool Sequence::IsEmpty() const noexcept { return notes_.empty(); }

bool Sequence::IsLooping() const noexcept { return loop_; }

void Sequence::Process(double begin_position, double end_position,
                       const ProcessCallback& process_callback) const noexcept {
  if (notes_.empty()) return;

  begin_position = std::max(begin_position, begin_position_);
  end_position = std::min(end_position, end_position_);
  if (begin_position >= end_position) return;

  double position_offset = begin_position_ - begin_offset_;
  begin_position -= position_offset;
  end_position -= position_offset;
  if (loop_) {
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

void Sequence::RemoveAllNotes() noexcept {
  notes_.clear();
  positions_.clear();
}

void Sequence::RemoveAllNotes(double begin_position,
                              double end_position) noexcept {
  if (begin_position < end_position) {
    const auto begin =
        notes_.lower_bound(NotePositionIdPair{begin_position, kInvalidId});
    const auto end =
        notes_.lower_bound(NotePositionIdPair{end_position, kInvalidId});
    for (auto it = begin; it != end; ++it) {
      positions_.erase(it->first.second);
    }
    notes_.erase(begin, end);
  }
}

bool Sequence::RemoveNote(Id id) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    notes_.erase(NotePositionIdPair{position_it->second, id});
    positions_.erase(position_it);
    return true;
  }
  return false;
}

void Sequence::SetBeginOffset(double begin_offset) noexcept {
  begin_offset_ = begin_offset;
}

void Sequence::SetBeginPosition(double begin_position) noexcept {
  begin_position_ = begin_position;
}

void Sequence::SetEndPosition(double end_position) noexcept {
  end_position_ = end_position;
}

void Sequence::SetLoop(bool loop) noexcept { loop_ = loop; }

void Sequence::SetLoopBeginOffset(double loop_begin_offset) noexcept {
  loop_begin_offset_ = loop_begin_offset;
}

void Sequence::SetLoopLength(double loop_length) noexcept {
  loop_length_ = std::max(loop_length, 0.0);
}

bool Sequence::SetNoteDefinition(Id id,
                                 BarelyNoteDefinition definition) noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    auto* note = FindOrNull(notes_, NotePositionIdPair{*position, id});
    *note = std::move(definition);
    return true;
  }
  return false;
}

bool Sequence::SetNotePosition(Id id, double position) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    if (position_it->second != position) {
      const auto note_it =
          notes_.find(NotePositionIdPair{position_it->second, id});
      auto note_node = notes_.extract(note_it);
      note_node.key().first = position;
      notes_.insert(std::move(note_node));
      position_it->second = position;
    }
    return true;
  }
  return false;
}

void Sequence::ProcessInternal(
    double begin_position, double end_position, double position_offset,
    const ProcessCallback& process_callback) const noexcept {
  if (!process_callback) return;
  const auto begin =
      notes_.lower_bound(NotePositionIdPair{begin_position, kInvalidId});
  const auto end =
      notes_.lower_bound(NotePositionIdPair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    process_callback(it->first.first + position_offset, it->second);
  }
}

}  // namespace barelyapi
