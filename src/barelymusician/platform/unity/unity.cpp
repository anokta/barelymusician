#include "barelymusician/platform/unity/unity.h"

#include <functional>

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
        sequencer(sample_rate) {
    CHECK_GT(sample_rate, 0);
    CHECK_GT(num_channels, 0);
    CHECK_GT(num_frames, 0);
  }

  int sample_rate;
  int num_channels;
  int num_frames;

  Sequencer sequencer;
};

BarelyMusician* barelymusician = nullptr;

}  // namespace

void Initialize(int sample_rate, int num_channels, int num_frames) {
  if (barelymusician == nullptr) {
    barelymusician = new BarelyMusician(sample_rate, num_channels, num_frames);
  }
}

void RegisterBeatCallback(BeatCallback* beat_callback) {
  DCHECK(barelymusician);
  const auto sequencer_beat_callback =
      [beat_callback](const Transport& transport, int, int) {
        beat_callback(transport.section, transport.bar, transport.beat);
      };
  barelymusician->sequencer.RegisterBeatCallback(sequencer_beat_callback);
}

void SetNumBars(int num_bars) {
  DCHECK(barelymusician);
  barelymusician->sequencer.SetNumBars(num_bars);
}

void SetNumBeats(int num_beats) {
  DCHECK(barelymusician);
  barelymusician->sequencer.SetNumBeats(num_beats);
}

void SetTempo(float tempo) {
  DCHECK(barelymusician);
  barelymusician->sequencer.SetTempo(tempo);
}

void SetSequencerTransport(float tempo, int num_bars, int num_beats) {
  DCHECK(barelymusician);
  Sequencer& sequencer = barelymusician->sequencer;
  sequencer.SetTempo(tempo);
  sequencer.SetNumBars(num_bars);
  sequencer.SetNumBeats(num_beats);
}

void Shutdown() {
  if (barelymusician != nullptr) {
    delete barelymusician;
  }
  barelymusician = nullptr;
}

void Update() {
  DCHECK(barelymusician);
  barelymusician->sequencer.Update(barelymusician->num_frames);
}

}  // namespace unity
}  // namespace barelyapi
