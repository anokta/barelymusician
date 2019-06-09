#include "barelymusician/api/barelymusician.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/transport.h"

namespace barelyapi {

namespace {

// Maximum number of tasks allowed to be added per each run.
const int kNumMaxTasks = 500;

}  // namespace

BarelyMusician::BarelyMusician(int sample_rate, int num_channels,
                               int num_frames)
    : sample_rate_(sample_rate),
      num_channels_(num_channels),
      num_frames_(num_frames),
      id_counter_(0),
      task_runner_(kNumMaxTasks) {
  DCHECK_GE(sample_rate, 0);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);
}

void BarelyMusician::DestroyInstrument(int instrument_id) {
  task_runner_.Add(
      [this, instrument_id]() { performers_.erase(instrument_id); });
}

void BarelyMusician::ClearAllInstrumentNotes(int instrument_id) {
  task_runner_.Add([this, instrument_id]() {
    Performer* performer = GetPerformer(instrument_id);
    if (performer != nullptr) {
      performer->ClearAllNotes();
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::ProcessInstrument(int instrument_id, float* output) {
  Performer* performer = GetPerformer(instrument_id);
  if (performer != nullptr) {
    performer->Process(output, num_channels_, num_frames_);
  } else {
    DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    std::fill_n(output, num_channels_ * num_frames_, 0.0f);
  }
}

void BarelyMusician::RegisterInstrumentNoteOffCallback(
    int instrument_id, NoteOffCallback&& note_off_callback) {
  task_runner_.Add([this, instrument_id, note_off_callback]() mutable {
    Performer* performer = GetPerformer(instrument_id);
    if (performer != nullptr) {
      performer->RegisterNoteOffCallback(std::move(note_off_callback));
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::RegisterInstrumentNoteOnCallback(
    int instrument_id, NoteOnCallback&& note_on_callback) {
  task_runner_.Add([this, instrument_id, note_on_callback]() mutable {
    Performer* performer = GetPerformer(instrument_id);
    if (performer != nullptr) {
      performer->RegisterNoteOnCallback(std::move(note_on_callback));
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::StartInstrumentNote(int instrument_id, float index,
                                         float intensity, int offset_samples) {
  task_runner_.Add([this, instrument_id, index, intensity, offset_samples]() {
    Performer* performer = GetPerformer(instrument_id);
    if (performer != nullptr) {
      performer->StartNote(index, intensity, offset_samples);
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

void BarelyMusician::StopInstrumentNote(int instrument_id, float index,
                                        int offset_samples) {
  task_runner_.Add([this, instrument_id, index, offset_samples]() {
    Performer* performer = GetPerformer(instrument_id);
    if (performer != nullptr) {
      performer->StopNote(index, offset_samples);
    } else {
      DLOG(WARNING) << "Invalid instrument ID: " << instrument_id;
    }
  });
}

int BarelyMusician::CreateSequencer() {
  const int sequencer_id = ++id_counter_;
  task_runner_.Add([this, sequencer_id]() {
    sequencers_.insert(std::make_pair(sequencer_id, Sequencer(sample_rate_)));
  });
  return sequencer_id;
}

void BarelyMusician::DestroySequencer(int sequencer_id) {
  task_runner_.Add([this, sequencer_id]() { sequencers_.erase(sequencer_id); });
}

void BarelyMusician::RegisterSequencerBeatCallback(
    int sequencer_id, BeatCallback&& beat_callback) {
  task_runner_.Add([this, sequencer_id, beat_callback]() mutable {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->RegisterBeatCallback(std::move(beat_callback));
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::SetSequencerNumBars(int sequencer_id, int num_bars) {
  task_runner_.Add([this, sequencer_id, num_bars]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->SetNumBars(num_bars);
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::SetSequencerNumBeats(int sequencer_id, int num_beats) {
  task_runner_.Add([this, sequencer_id, num_beats]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->SetNumBeats(num_beats);
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::SetSequencerPosition(int sequencer_id, int section,
                                          int bar, int beat) {
  task_runner_.Add([this, sequencer_id, section, bar, beat]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->SetPosition(section, bar, beat);
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::SetSequencerTempo(int sequencer_id, float tempo) {
  task_runner_.Add([this, sequencer_id, tempo]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->SetTempo(tempo);
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::StartSequencer(int sequencer_id) {
  task_runner_.Add([this, sequencer_id]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->Start();
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

void BarelyMusician::StopSequencer(int sequencer_id) {
  task_runner_.Add([this, sequencer_id]() {
    Sequencer* sequencer = GetSequencer(sequencer_id);
    if (sequencer != nullptr) {
      sequencer->Stop();
    } else {
      DLOG(WARNING) << "Invalid sequencer ID: " << sequencer_id;
    }
  });
}

Performer* BarelyMusician::GetPerformer(int instrument_id) {
  const auto it = performers_.find(instrument_id);
  if (it != performers_.end()) {
    return &it->second;
  }
  return nullptr;
}

Sequencer* BarelyMusician::GetSequencer(int sequencer_id) {
  const auto it = sequencers_.find(sequencer_id);
  if (it != sequencers_.end()) {
    return &it->second;
  }
  return nullptr;
}

void BarelyMusician::Update() {
  task_runner_.Run();
  for (auto& it : sequencers_) {
    it.second.Update(num_frames_);
  }
}

}  // namespace barelyapi
