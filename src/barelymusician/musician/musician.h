#ifndef BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
#define BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_

#include <algorithm>
#include <utility>
#include <vector>

#include "barelymusician/base/clock.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/musician/note_utils.h"
#include "barelymusician/musician/score.h"

namespace barelyapi {

class Musician {
 public:
  // Musical ensemble.
  struct Ensemble {
    // Bar composer callback signature.
    using BarComposerCallback = std::function<int(int bar, int num_beats)>;

    // Beat composer callback signature.
    using BeatComposerCallback =
        std::function<void(int bar, int beat, int num_beats, int harmonic,
                           std::vector<Note>* notes)>;

    // Bar composer callback.
    BarComposerCallback bar_composer_callback;

    struct Performer {
      Performer(Instrument* instrument,
                BeatComposerCallback beat_composer_callback)
          : instrument(instrument),
            beat_composer_callback(beat_composer_callback) {}

      Instrument* instrument;
      BeatComposerCallback beat_composer_callback;
      Score score;
    };

    // List of performers.
    std::vector<Performer> performers;
  };

  explicit Musician(int sample_rate) : clock_(sample_rate) {}

  void SetNumBeats(int num_beats) { num_beats_ = num_beats; }

  void SetTempo(float tempo) { tempo_ = tempo; }

  // TODO: timestamp not necessary?
  void Update(int num_samples, int timestamp) {
    // TODO: is this efficient?
    if (clock_.GetTempo() != tempo_) {
      clock_.SetTempo(tempo_);
    }

    const int start_beat = clock_.GetBeat();
    const int start_leftover_samples = clock_.GetLeftoverSamples();
    clock_.Update(num_samples);
    const int end_beat = clock_.GetBeat();
    const int end_leftover_samples = clock_.GetLeftoverSamples();

    const int num_samples_per_beat = clock_.GetNumSamplesPerBeat();
    const float start_leftover_beats =
        BeatsFromSamples(start_leftover_samples, num_samples_per_beat);
    const float end_leftover_beats =
        BeatsFromSamples(end_leftover_samples, num_samples_per_beat);
    int beat_timestamp = timestamp - start_leftover_samples;
    for (int beat = start_beat; beat <= end_beat; ++beat) {
      if ((beat != start_beat || start_leftover_samples == 0) &&
          (beat != end_beat || end_leftover_samples > 0)) {
        ProcessBeat(beat);
      }
      for (Ensemble::Performer& performer : ensemble_.performers) {
        const auto* notes = performer.score.GetNotes(beat);
        if (notes == nullptr) {
          // TODO: this does not make sense - fill empty vector there.
          continue;
        }
        auto cbegin = notes->cbegin();
        auto cend = notes->cend();
        if (beat == start_beat) {
          cbegin = std::lower_bound(cbegin, cend, start_leftover_beats,
                                    &CompareOffsetBeats);
        }
        if (beat == end_beat) {
          cend = std::lower_bound(cbegin, cend, end_leftover_beats,
                                  &CompareOffsetBeats);
        }
        for (auto it = cbegin; it != cend; ++it) {
          const int note_on_timestamp =
              beat_timestamp +
              SamplesFromBeats(it->offset_beats, num_samples_per_beat);
          performer.instrument->NoteOnScheduled(it->index, it->intensity,
                                                note_on_timestamp);
          const int note_off_timestamp =
              note_on_timestamp +
              SamplesFromBeats(it->duration_beats, num_samples_per_beat);
          performer.instrument->NoteOffScheduled(it->index, note_off_timestamp);
        }
        // TODO: clear |notes|?
      }
      beat_timestamp += num_samples_per_beat;
    }
  }

  Ensemble& ensemble() { return ensemble_; }
  const Ensemble& ensemble() const { return ensemble_; }

 private:
  void ProcessBeat(int beat) {
    // Update transport.
    bar_ = beat / num_beats_;
    beat_ = beat % num_beats_;

    if (beat_ == 0) {
      // Compose next bar.
      harmonic_ = ensemble_.bar_composer_callback(bar_, num_beats_);
    }
    // Update performers.
    for (Ensemble::Performer& performer : ensemble_.performers) {
      // Compose next beat notes.
      temp_notes_.clear();
      performer.beat_composer_callback(bar_, beat_, num_beats_, harmonic_,
                                       &temp_notes_);
      for (const Note& note : temp_notes_) {
        performer.score.AddNote(beat, note);
      }
    }
  }

  Clock clock_;

  Ensemble ensemble_;

  int harmonic_;

  float tempo_;

  // Current bar.
  int bar_;

  // Current beat.
  int beat_;

  // Number of beats per bar.
  int num_beats_;

  // TODO: not necessary, use Score instead?
  std::vector<Note> temp_notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
