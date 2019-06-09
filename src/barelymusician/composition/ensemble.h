#ifndef BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
#define BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/performer.h"
#include "barelymusician/composition/scale.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

class Ensemble {
 public:
  // Section composer callback signature.
  using SectionComposerCallback = std::function<int(const Transport&)>;

  // Bar composer callback signature.
  using BarComposerCallback = std::function<int(const Transport&, int)>;

  // Beat composer callback signature.
  using BeatComposerCallback = std::function<void(
      float, const Scale&, const Transport&, int, int, std::vector<Note>*)>;

  explicit Ensemble(Sequencer* sequencer, const Scale& scale);

  void AddPerformer(Performer* performer,
                    BeatComposerCallback&& beat_composer_callback);

  // Sets the root note (key) of score.
  //
  // @param index Root note index.
  void SetRootNote(float index) { root_note_index_ = index; }

  void SetSectionComposerCallback(
      SectionComposerCallback&& section_composer_callback);

  void SetBarComposerCallback(BarComposerCallback&& bar_composer_callback);

 private:
  // Root note index.
  float root_note_index_;

  // Musical scale.
  Scale scale_;

  // Section composer callback.
  SectionComposerCallback section_composer_callback_;

  // Bar composer callback.
  BarComposerCallback bar_composer_callback_;

  // Current section type.
  int section_type_;

  // Current harmonic.
  int harmonic_;

  // List of performers.
  std::vector<std::pair<Performer*, BeatComposerCallback>> performers_;

  std::vector<Note> temp_beat_notes_;
};

Ensemble::Ensemble(Sequencer* sequencer, const Scale& scale)
    : root_note_index_(kNoteIndexC3),
      scale_(scale),
      section_composer_callback_(nullptr),
      bar_composer_callback_(nullptr),
      section_type_(0),
      harmonic_(0) {
  DCHECK(sequencer);
  sequencer->RegisterBeatCallback([&](const Transport& transport,
                                      int start_sample,
                                      int num_samples_per_beat) {
    if (transport.beat == 0) {
      // New bar.
      if (transport.bar == 0) {
        // New section.
        if (section_composer_callback_ != nullptr) {
          section_type_ = section_composer_callback_(transport);
        }
      }
      if (bar_composer_callback_ != nullptr) {
        harmonic_ = bar_composer_callback_(transport, section_type_);
      }
    }
    for (auto& performer : performers_) {
      temp_beat_notes_.clear();
      performer.second(root_note_index_, scale_, transport, section_type_,
                       harmonic_, &temp_beat_notes_);
      for (const Note& note : temp_beat_notes_) {
        const int start_offset_samples =
            start_sample +
            SamplesFromBeats(note.start_beat, num_samples_per_beat);
        performer.first->StartNote(note.index, note.intensity,
                                   start_offset_samples);
        const int stop_offset_samples =
            start_offset_samples +
            SamplesFromBeats(note.duration_beats, num_samples_per_beat);
        performer.first->StopNote(note.index, stop_offset_samples);
      }
    }
  });
}

void Ensemble::AddPerformer(Performer* performer,
                            BeatComposerCallback&& beat_composer_callback) {
  performers_.emplace_back(performer, std::move(beat_composer_callback));
}

void Ensemble::SetSectionComposerCallback(
    SectionComposerCallback&& section_composer_callback) {
  section_composer_callback_ = std::move(section_composer_callback);
}

void Ensemble::SetBarComposerCallback(
    BarComposerCallback&& bar_composer_callback) {
  bar_composer_callback_ = std::move(bar_composer_callback);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
