#ifndef BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
#define BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/base/clock.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "barelymusician/musician/score.h"

namespace barelyapi {

class Musician {
 public:
  struct Ensemble {
    // Section composer callback signature.
    using SectionComposerCallback = std::function<int(int section)>;

    // Bar composer callback signature.
    using BarComposerCallback =
        std::function<int(int bar, int num_bars, int section_type)>;

    // Beat composer callback signature.
    using BeatComposerCallback = std::function<void(
        int bar, int beat, int num_bars, int num_beats, int section_type,
        int harmonic, std::vector<Note>* notes)>;

    // Section composer callback.
    SectionComposerCallback section_composer_callback;

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

  // TODO: get num_bars from section composer.
  explicit Musician(int sample_rate) : clock_(sample_rate), bar_(0), beat_(0) {
    clock_.SetBeatCallback([this](int beat, int leftover_samples) {
      // Update transport.
      beat_ = beat % num_beats_;
      bar_ = beat / num_beats_;
      section_ = bar_ / num_bars_;
      bar_ %= num_bars_;

      if (beat_ == 0) {
        if (bar_ == 0) {
          // Compose next section.
          section_type_ = ensemble_.section_composer_callback(section_);
        }
        // Compose next bar.
        harmonic_ =
            ensemble_.bar_composer_callback(bar_, num_bars_, section_type_);
      }
      // Update performers.
      for (Ensemble::Performer& performer : ensemble_.performers) {
        // Compose next beat notes.
        temp_notes_.clear();
        performer.beat_composer_callback(bar_, beat_, num_bars_, num_beats_,
                                         section_type_, harmonic_,
                                         &temp_notes_);
        for (Note& note : temp_notes_) {
          // TODO: inefficient?!
          note.start_beat = beat;
          performer.score.AddNote(note);
        }
      }
    });
  }

  // TODO: get this from section type.
  void SetNumBars(int num_bars) { num_bars_ = num_bars; }

  void SetNumBeats(int num_beats) { num_beats_ = num_beats; }

  void SetTempo(float tempo) { tempo_ = tempo; }

  // TODO: timestamp not necessary?
  void Update(int num_samples, int timestamp) {
    // TODO: is this efficient?
    if (clock_.GetTempo() != tempo_) {
      clock_.SetTempo(tempo_);
    }

    const float start_position = clock_.GetPosition();
    const int start_beat = clock_.GetBeat();
    const int leftover_samples = clock_.GetLeftoverSamples();
    clock_.Update(num_samples);
    const float end_position = clock_.GetPosition();

    const int num_samples_per_beat = clock_.GetNumSamplesPerBeat();
    for (Ensemble::Performer& performer : ensemble_.performers) {
      // TODO: store note events somewhere to avoid timestamp.
      const auto notes =
          performer.score.GetIterator(start_position, end_position);
      for (auto it = notes.begin; it != notes.end; ++it) {
        const int note_on_timestamp =
            timestamp + (it->start_beat - start_beat) * num_samples_per_beat +
            SamplesFromBeats(it->offset_beats, num_samples_per_beat) -
            leftover_samples;
        performer.instrument->NoteOnScheduled(it->index, it->intensity,
                                              note_on_timestamp);
        const int note_off_timestamp =
            note_on_timestamp +
            SamplesFromBeats(it->duration_beats, num_samples_per_beat);
        performer.instrument->NoteOffScheduled(it->index, note_off_timestamp);
      }
      // TODO: clear |notes|?
    }
  }

  Ensemble& ensemble() { return ensemble_; }
  const Ensemble& ensemble() const { return ensemble_; }

 private:
  Clock clock_;
  Ensemble ensemble_;

  int section_;
  int bar_;
  int beat_;

  int num_bars_;
  int num_beats_;

  int section_type_;
  int harmonic_;

  float tempo_;

  // TODO: not necessary, use Score instead?
  std::vector<Note> temp_notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_MUSICIAN_H_
