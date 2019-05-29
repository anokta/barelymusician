#include "barelymusician/platform/unity/unity.h"

#include <functional>
#include <unordered_map>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/sequencer/sequencer.h"

namespace barelyapi {
namespace unity {

namespace {

struct BarelyMusician {
  BarelyMusician(int sample_rate, int num_channels, int num_frames)
      : sample_rate(sample_rate),
        num_channels(num_channels),
        num_frames(num_frames),
        id_counter(0) {
    CHECK_GT(sample_rate, 0);
    CHECK_GT(num_channels, 0);
    CHECK_GT(num_frames, 0);
  }

  int sample_rate;
  int num_channels;
  int num_frames;

  int id_counter;

  double dsp_time;

  std::unordered_map<int, Sequencer> sequencers;

  int AddSequencer(BeatCallback* beat_callback) {
    const int sequencer_id = ++id_counter;
    Sequencer sequencer(sample_rate);
    const auto sequencer_beat_callback = [beat_callback, this](
                                             const Transport& transport,
                                             int start_sample, int) {
      beat_callback(transport.section, transport.bar, transport.beat,
                    dsp_time + static_cast<double>(start_sample) / sample_rate);
    };
    sequencer.RegisterBeatCallback(sequencer_beat_callback);
    sequencers.insert({sequencer_id, std::move(sequencer)});
    return sequencer_id;
  }

  Sequencer* GetSequencer(int sequencer_id) {
    const auto it = sequencers.find(sequencer_id);
    if (it != sequencers.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void RemoveSequencer(int sequencer_id) { sequencers.erase(sequencer_id); }
};

BarelyMusician* barelymusician = nullptr;

}  // namespace

void Initialize(int sample_rate, int num_channels, int num_frames) {
  if (barelymusician == nullptr) {
    barelymusician = new BarelyMusician(sample_rate, num_channels, num_frames);
  }
}

void Shutdown() {
  if (barelymusician != nullptr) {
    delete barelymusician;
  }
  barelymusician = nullptr;
}

int CreateSequencer(BeatCallback* beat_callback) {
  DCHECK(barelymusician);
  return barelymusician->AddSequencer(beat_callback);
}

void DestroySequencer(int sequencer_id) {
  DCHECK(barelymusician);
  barelymusician->RemoveSequencer(sequencer_id);
}

void ProcessSequencer(int sequencer_id, double dsp_time) {
  DCHECK(barelymusician);
  barelymusician->dsp_time = dsp_time;
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  DCHECK(sequencer);
  sequencer->Update(barelymusician->num_frames);
}

void SetSequencerNumBars(int sequencer_id, int num_bars) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  DCHECK(sequencer);
  sequencer->SetNumBars(num_bars);
}

void SetSequencerNumBeats(int sequencer_id, int num_beats) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  DCHECK(sequencer);
  sequencer->SetNumBeats(num_beats);
}

void SetSequencerTempo(int sequencer_id, float tempo) {
  DCHECK(barelymusician);
  Sequencer* sequencer = barelymusician->GetSequencer(sequencer_id);
  DCHECK(sequencer);
  sequencer->SetTempo(tempo);
}

}  // namespace unity
}  // namespace barelyapi
