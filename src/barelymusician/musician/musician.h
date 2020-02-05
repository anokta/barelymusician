#ifndef BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
#define BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/musician/note.h"

namespace barelyapi {

class Musician {
 public:
  // Bar composer callback signature.
  using BarComposerCallback = std::function<int(int bar, int num_beats)>;

  // Beat composer callback signature.
  using BeatComposerCallback =
      std::function<void(int bar, int beat, int num_beats, int harmonic,
                         std::vector<Note>* notes)>;

  explicit Musician(int sample_rate) : engine_(sample_rate) {
    engine_.SetBeatCallback([&](int beat) { ProcessBeat(beat); });
    engine_.Start();
    //engine_.SetNoteOffCallback([](int performer_id, float index) {
    //  LOG(INFO) << "Performer #" << performer_id << ": NoteOff(" << index
    //            << ")";
    //});
    //engine_.SetNoteOnCallback(
    //    [](int performer_id, float index, float intensity) {
    //      LOG(INFO) << "Performer #" << performer_id << ": NoteOn(" << index
    //                << ", " << intensity << ")";
    //    });
  }

  int AddPerformer(std::unique_ptr<Instrument> instrument,
                   BeatComposerCallback beat_composer_callback) {
    const int performer_id = engine_.Create(std::move(instrument));
    performers_.emplace(performer_id, beat_composer_callback);
    return performer_id;
  }

  void SetBarComposerCallback(BarComposerCallback bar_composer_callback) {
    bar_composer_callback_ = bar_composer_callback;
  }

  void SetNumBeats(int num_beats) { num_beats_ = num_beats; }

  void Process(int performer_id, float* output, int num_channels,
               int num_frames) {
    engine_.Process(performer_id, output, num_channels, num_frames);
  }

  double GetTempo() const { return engine_.GetTempo(); }

  bool IsPlaying() const { return engine_.IsPlaying(); }

  void SetTempo(double tempo) { engine_.SetTempo(tempo); }

  void Start() { engine_.Start(); }

  void Stop() { engine_.Stop(); }

  void Update(int num_frames) { engine_.Update(num_frames); }

 private:
  void ProcessBeat(int beat) {
    // Update transport.
    bar_ = beat / num_beats_;
    beat_ = beat % num_beats_;

    if (beat_ == 0) {
      // Compose next bar.
      if (bar_composer_callback_ != nullptr) {
        harmonic_ = bar_composer_callback_(bar_, num_beats_);
      }
    }
    // Update members.
    for (auto& [id, callback] : performers_) {
      // Compose next beat notes.
      temp_notes_.clear();
      if (callback != nullptr) {
        callback(bar_, beat_, num_beats_, harmonic_, &temp_notes_);
      }
      for (const Note& note : temp_notes_) {
        const double position = static_cast<double>(beat) + note.offset_beats;
        engine_.ScheduleNoteOn(id, note.index, note.intensity, position);
        engine_.ScheduleNoteOff(id, note.index, position + note.duration_beats);
      }
    }
  }

  Engine engine_;

  // Bar composer callback.
  BarComposerCallback bar_composer_callback_;

  int harmonic_;

  // Current bar.
  int bar_;

  // Current beat.
  int beat_;

  // Number of beats per bar.
  int num_beats_;

  std::unordered_map<int, BeatComposerCallback> performers_;

  // TODO: not necessary, use Score instead?
  std::vector<Note> temp_notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
