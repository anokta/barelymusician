#include "barelymusician/api/barelymusician.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Maximum number of tasks allowed to be added per each run.
constexpr int kNumMaxTasks = 500;

}  // namespace

BarelyMusician::BarelyMusician(int sample_rate, int num_channels,
                               int num_frames)
    : sample_rate_(sample_rate),
      num_channels_(num_channels),
      num_frames_(num_frames),
      id_counter_(0),
      task_runner_(kNumMaxTasks),
      clock_(sample_rate),
      is_playing_(false),
      timestamp_(0) {
  DCHECK_GE(sample_rate, 0);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
}

void BarelyMusician::Reset() {
  task_runner_.Add([this]() {
    clock_.Reset();
    timestamp_ = 0;
  });
}

void BarelyMusician::Start() {
  task_runner_.Add([this]() { is_playing_ = true; });
}

void BarelyMusician::Stop() {
  task_runner_.Add([this]() { is_playing_ = false; });
}

void BarelyMusician::Update() {
  task_runner_.Run();
  if (is_playing_) {
    clock_.Update(num_frames_);
    timestamp_ += num_frames_;
  }
}

void BarelyMusician::SetTempo(float tempo) {
  task_runner_.Add([this, tempo]() { clock_.SetTempo(tempo); });
}

void BarelyMusician::DestroyInstrument(int instrument_id) {
  task_runner_.Add(
      [this, instrument_id]() { instruments_.erase(instrument_id); });
}

void BarelyMusician::ProcessInstrument(int instrument_id, float* output) {
  Instrument* instrument = GetInstrument(instrument_id);
  if (instrument != nullptr) {
    instrument->ProcessScheduled(output, num_channels_, num_frames_,
                                 timestamp_);
  } else {
    DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    std::fill_n(output, num_channels_ * num_frames_, 0.0f);
  }
}

void BarelyMusician::SetInstrumentAllNotesOff(int instrument_id) {
  task_runner_.Add([this, instrument_id]() {
    Instrument* instrument = GetInstrument(instrument_id);
    if (instrument != nullptr) {
      instrument->AllNotesOff();
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::SetInstrumentNoteOff(int instrument_id, float index) {
  task_runner_.Add([this, instrument_id, index]() {
    Instrument* instrument = GetInstrument(instrument_id);
    if (instrument != nullptr) {
      instrument->NoteOff(index);
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::SetInstrumentNoteOn(int instrument_id, float index,
                                         float intensity) {
  task_runner_.Add([this, instrument_id, index, intensity]() {
    Instrument* instrument = GetInstrument(instrument_id);
    if (instrument != nullptr) {
      instrument->NoteOn(index, intensity);
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

Instrument* BarelyMusician::GetInstrument(int instrument_id) {
  const auto it = instruments_.find(instrument_id);
  if (it != instruments_.cend()) {
    return it->second.get();
  }
  return nullptr;
}

}  // namespace barelyapi
