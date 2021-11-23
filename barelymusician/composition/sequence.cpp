#include "barelymusician/composition/sequence.h"

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
                       double begin_position, double end_position,
                       double position_offset,
                       const Sequence::ProcessCallback& process_callback) {
  const auto begin = notes.lower_bound(std::pair{begin_position, kInvalidId});
  const auto end = notes.lower_bound(std::pair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    process_callback(it->first.first + position_offset, it->second);
  }
}

}  // namespace

Sequence::Sequence()
    : begin_offset_(0.0),
      is_looping_(false),
      loop_begin_offset_(0.0),
      loop_length_(1.0) {}

Status Sequence::Add(Id id, double position, Note note) {
  if (id == kInvalidId) return Status::kInvalidArgument;
  if (positions_.emplace(id, position).second) {
    notes_.emplace(std::pair{position, id}, std::move(note));
    return Status::kOk;
  }
  return Status::kAlreadyExists;
}

double Sequence::GetBeginOffset() const { return begin_offset_; }

double Sequence::GetLoopBeginOffset() const { return loop_begin_offset_; }

double Sequence::GetLoopLength() const { return loop_length_; }

bool Sequence::IsEmpty() const { return notes_.empty(); }

bool Sequence::IsLooping() const { return is_looping_; }

void Sequence::Process(double begin_position, double end_position,
                       ProcessCallback process_callback) const {
  double position_offset = -begin_offset_;
  begin_position -= position_offset;
  end_position -= position_offset;

  if (is_looping_) {
    if (loop_length_ <= 0.0) {
      return;
    }

    if (begin_position > loop_length_ + loop_begin_offset_) {
      const double loop_offset =
          loop_length_ *
          std::floor((begin_position - loop_begin_offset_) / loop_length_);
      begin_position -= loop_offset;
      end_position -= loop_offset;
      position_offset += loop_offset;
    }

    if (begin_position < loop_begin_offset_ + loop_length_) {
      const double loop_end_position =
          std::min(loop_begin_offset_ + loop_length_, end_position);
      ProcessWithOffset(notes_, begin_position, loop_end_position,
                        position_offset, process_callback);
      position_offset += loop_end_position - begin_position;
      begin_position = loop_end_position;
    }

    while (begin_position < end_position) {
      const double loop_end_position =
          loop_begin_offset_ +
          std::min(loop_length_, end_position - begin_position);
      ProcessWithOffset(notes_, loop_begin_offset_, loop_end_position,
                        position_offset + begin_position - loop_begin_offset_,
                        process_callback);
      begin_position += loop_length_;
    }
  } else {
    ProcessWithOffset(notes_, begin_position, end_position, position_offset,
                      process_callback);
  }
}

Status Sequence::Remove(Id id) {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    notes_.erase(std::pair{position_it->second, id});
    positions_.erase(position_it);
    return Status::kOk;
  }
  return Status::kNotFound;
}

void Sequence::RemoveAll() {
  notes_.clear();
  positions_.clear();
}

void Sequence::RemoveAll(double begin_position, double end_position) {
  const auto begin = notes_.lower_bound(std::pair{begin_position, kInvalidId});
  const auto end = notes_.lower_bound(std::pair{end_position, kInvalidId});
  for (auto it = begin; it != end; ++it) {
    positions_.erase(it->first.second);
  }
  notes_.erase(begin, end);
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

}  // namespace barelyapi
