#include "barelymusician/performer/sequence.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <utility>

#include "barelymusician/api/performer.h"
#include "barelymusician/performer/find_or_null.h"

namespace barelyapi {

bool Sequence::AddNote(BarelyId id, double position,
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

BarelyId Sequence::GetInstrument() const noexcept { return instrument_id_; }

double Sequence::GetLoopBeginOffset() const noexcept {
  return loop_begin_offset_;
}

double Sequence::GetLoopLength() const noexcept { return loop_length_; }

bool Sequence::IsEmpty() const noexcept { return notes_.empty(); }

bool Sequence::IsLooping() const noexcept { return loop_; }

void Sequence::Process(double begin_position, double end_position) noexcept {
  // if (instrument_id_ == BarelyId_kInvalid) return;

  // Perform active note events.
  for (auto it = active_notes_.begin(); it != active_notes_.end();) {
    const auto& [note_begin_position, active_note] = *it;
    double note_end_position = active_note.end_position;
    if (note_end_position < end_position) {
      note_end_position = std::max(begin_position, note_end_position);
    } else if (begin_position < note_begin_position) {
      note_end_position = begin_position;
    } else {
      ++it;
      continue;
    }
    // Perform note off event.
    if (event_callback_) {
      event_callback_(note_end_position, SetNoteOffEvent{active_note.pitch});
    }
    it = active_notes_.erase(it);
  }

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
      ProcessInternal(begin_position, loop_end_position, position_offset);
      begin_position = loop_end_position;
    }
    // Process the rest of the loop iterations.
    position_offset -= loop_begin_offset_;
    while (begin_position < end_position) {
      const double loop_end_position =
          loop_begin_offset_ +
          std::min(loop_length_, end_position - begin_position);
      ProcessInternal(loop_begin_offset_, loop_end_position,
                      position_offset + begin_position);
      begin_position += loop_length_;
    }
  } else {
    ProcessInternal(begin_position, end_position, position_offset);
  }
}

void Sequence::RemoveAllNotes() noexcept {
  notes_.clear();
  positions_.clear();
}

void Sequence::RemoveAllNotes(double begin_position,
                              double end_position) noexcept {
  // TODO: Remove and send note off event for active notes.
  if (begin_position < end_position) {
    const auto begin = notes_.lower_bound(
        NotePositionIdPair{begin_position, BarelyId_kInvalid});
    const auto end =
        notes_.lower_bound(NotePositionIdPair{end_position, BarelyId_kInvalid});
    for (auto it = begin; it != end; ++it) {
      positions_.erase(it->first.second);
    }
    notes_.erase(begin, end);
  }
}

bool Sequence::RemoveNote(BarelyId id) noexcept {
  // TODO: Remove and send note off event if active note.
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

void Sequence::SetEventCallback(EventCallback event_callback) noexcept {
  event_callback_ = std::move(event_callback);
}

void Sequence::SetInstrument(BarelyId instrument_id) noexcept {
  // TODO: Send note off event for active notes.
  active_notes_.clear();
  instrument_id_ = instrument_id;
}

void Sequence::SetLoopBeginOffset(double loop_begin_offset) noexcept {
  loop_begin_offset_ = loop_begin_offset;
}

void Sequence::SetLoopLength(double loop_length) noexcept {
  loop_length_ = std::max(loop_length, 0.0);
}

void Sequence::SetLooping(bool is_looping) noexcept { loop_ = is_looping; }

bool Sequence::SetNoteDefinition(BarelyId id,
                                 BarelyNoteDefinition definition) noexcept {
  if (const auto* position = FindOrNull(positions_, id)) {
    auto* note = FindOrNull(notes_, NotePositionIdPair{*position, id});
    *note = std::move(definition);
    return true;
  }
  return false;
}

bool Sequence::SetNotePosition(BarelyId id, double position) noexcept {
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

void Sequence::ProcessInternal(double begin_position, double end_position,
                               double position_offset) noexcept {
  const auto begin =
      notes_.lower_bound(NotePositionIdPair{begin_position, BarelyId_kInvalid});
  const auto end =
      notes_.lower_bound(NotePositionIdPair{end_position, BarelyId_kInvalid});
  for (auto it = begin; it != end; ++it) {
    const double position = it->first.first + position_offset;
    const auto& note = it->second;
    // TODO: Include note adjustments.
    const double pitch = note.pitch.absolute_pitch;
    const double intensity = note.intensity;
    const double duration = note.duration;
    const double note_end_position =
        std::min(position + std::max(duration, 0.0), end_position_);
    // Perform note on event.
    if (event_callback_) {
      event_callback_(position, SetNoteOnEvent{pitch, intensity});
    }
    // Perform note off event.
    if (note_end_position >= end_position) {
      active_notes_.emplace(position, ActiveNote{note_end_position, pitch});
    } else if (event_callback_) {
      event_callback_(note_end_position, SetNoteOffEvent{pitch});
    }
  }
}

}  // namespace barelyapi
