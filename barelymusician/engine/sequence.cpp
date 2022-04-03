#include "barelymusician/engine/sequence.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <utility>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/transport.h"

namespace barelyapi {

Sequence::Sequence(const Conductor& conductor,
                   const Transport& transport) noexcept
    : conductor_(conductor), transport_(transport) {}

// NOLINTNEXTLINE(bugprone-exception-escape)
bool Sequence::CreateNote(Id id, Note::Definition definition,
                          double position) noexcept {
  assert(id > kInvalid);
  assert(position >= 0.0);
  if (positions_.emplace(id, position).second) {
    notes_.emplace(std::pair{position, id}, definition);
    return true;
  }
  return false;
}

bool Sequence::DestroyNote(Id id) noexcept {
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    notes_.erase(std::pair{position_it->second, id});
    positions_.erase(position_it);
    return true;
  }
  return false;
}

double Sequence::GetBeginOffset() const noexcept { return begin_offset_; }

double Sequence::GetBeginPosition() const noexcept { return begin_position_; }

double Sequence::GetEndPosition() const noexcept { return end_position_; }

Instrument* Sequence::GetInstrument() const noexcept { return instrument_; }

double Sequence::GetLoopBeginOffset() const noexcept {
  return loop_begin_offset_;
}

double Sequence::GetLoopLength() const noexcept { return loop_length_; }

const Note::Definition* Sequence::GetNoteDefinition(Id id) const noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    return FindOrNull(notes_, std::pair{*position, id});
  }
  return nullptr;
}

const double* Sequence::GetNotePosition(Id id) const noexcept {
  return FindOrNull(positions_, id);
}

bool Sequence::IsEmpty() const noexcept { return notes_.empty(); }

bool Sequence::IsLooping() const noexcept { return is_looping_; }

bool Sequence::IsSkippingAdjustments() const noexcept {
  return is_skipping_adjustments_;
}

void Sequence::Process(double begin_position, double end_position) noexcept {
  assert(begin_position >= 0.0 && begin_position <= end_position);
  if (!instrument_) {
    return;
  }
  // Process active notes.
  for (auto it = active_notes_.begin(); it != active_notes_.end();) {
    const auto& [note_begin_position, active_note] = *it;
    double note_end_position = active_note.end_position;
    if (begin_position < note_begin_position) {
      note_end_position = begin_position;
    } else if (note_end_position < end_position) {
      note_end_position = std::max(begin_position, note_end_position);
    } else {
      ++it;
      continue;
    }
    instrument_->StopNote(active_note.pitch,
                          transport_.GetTimestamp(note_end_position));
    it = active_notes_.erase(it);
  }
  // Process sequence notes.
  if (notes_.empty()) {
    return;
  }
  const double process_end_position = end_position;
  begin_position = std::max(begin_position, begin_position_);
  end_position = std::min(end_position, end_position_);
  if (begin_position >= end_position) {
    return;
  }
  double position_offset = begin_position_ - begin_offset_;
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
                      process_end_position);
      begin_position = loop_end_position;
    }
    // Process the rest of the loop iterations.
    position_offset -= loop_begin_offset_;
    while (begin_position < end_position) {
      const double loop_end_position =
          loop_begin_offset_ +
          std::min(loop_length_, end_position - begin_position);
      ProcessInternal(loop_begin_offset_, loop_end_position,
                      position_offset + begin_position, process_end_position);
      begin_position += loop_length_;
    }
  } else {
    ProcessInternal(begin_position, end_position, position_offset,
                    process_end_position);
  }
}

void Sequence::SetBeginOffset(double begin_offset) noexcept {
  begin_offset_ = begin_offset;
}

void Sequence::SetBeginPosition(double begin_position) noexcept {
  assert(begin_position >= 0.0);
  begin_position_ = begin_position;
}

void Sequence::SetEndPosition(double end_position) noexcept {
  assert(end_position >= 0.0);
  end_position_ = end_position;
}

void Sequence::SetInstrument(Instrument* instrument) noexcept {
  if (instrument_ != instrument) {
    if (instrument_ && !active_notes_.empty()) {
      for (const auto& [position, active_note] : active_notes_) {
        instrument_->StopNote(active_note.pitch, transport_.GetTimestamp());
      }
    }
    active_notes_.clear();
    instrument_ = instrument;
  }
}

void Sequence::SetLoopBeginOffset(double loop_begin_offset) noexcept {
  loop_begin_offset_ = loop_begin_offset;
}

void Sequence::SetLoopLength(double loop_length) noexcept {
  assert(loop_length >= 0.0);
  loop_length_ = loop_length_;
}

void Sequence::SetLooping(bool is_looping) noexcept {
  is_looping_ = is_looping;
}

bool Sequence::SetNoteDefinition(Id id, Note::Definition definition) noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    *FindOrNull(notes_, std::pair{*position, id}) = definition;
    return true;
  }
  return false;
}

bool Sequence::SetNotePosition(Id id, double position) noexcept {
  assert(position >= 0.0);
  if (const auto position_it = positions_.find(id);
      position_it != positions_.end()) {
    if (position_it->second != position) {
      const auto note_it = notes_.find(std::pair{position_it->second, id});
      auto note_node = notes_.extract(note_it);
      note_node.key().first = position;
      notes_.insert(std::move(note_node));
      position_it->second = position;
    }
    return true;
  }
  return false;
}

void Sequence::SetSkippingAdjustments(bool is_skipping_adjustments) noexcept {
  is_skipping_adjustments_ = is_skipping_adjustments;
}

void Sequence::Stop() noexcept {
  if (!active_notes_.empty()) {
    if (instrument_) {
      for (const auto& [position, active_note] : active_notes_) {
        instrument_->StopNote(active_note.pitch, transport_.GetTimestamp());
      }
    }
    active_notes_.clear();
  }
}

void Sequence::ProcessInternal(double begin_position, double end_position,
                               double position_offset,
                               double process_end_position) noexcept {
  assert(begin_position >= 0.0 && begin_position <= end_position);
  assert(process_end_position >= 0.0);
  const auto begin = notes_.lower_bound(std::pair{begin_position, kInvalid});
  const auto end = notes_.lower_bound(std::pair{end_position, kInvalid});
  for (auto it = begin; it != end; ++it) {
    const double note_begin_position = it->first.first + position_offset;
    const Note note =
        conductor_.TransformNote(it->second, is_skipping_adjustments_);
    const double note_end_position = std::min(
        note_begin_position + std::max(note.duration, 0.0), end_position_);
    instrument_->StartNote(note.pitch, note.intensity,
                           transport_.GetTimestamp(note_begin_position));
    if (note_end_position < process_end_position) {
      instrument_->StopNote(note.pitch,
                            transport_.GetTimestamp(note_end_position));
    } else {
      active_notes_.emplace(note_begin_position,
                            ActiveNote{note_end_position, note.pitch});
    }
  }
}

}  // namespace barelyapi
