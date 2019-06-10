#ifndef BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
#define BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/transport.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/performer.h"
#include "barelymusician/dsp/dsp_utils.h"

namespace barelyapi {

class Ensemble {
 public:
  // Section composer callback signature.
  using SectionComposerCallback = std::function<int(const Transport&)>;

  // Bar composer callback signature.
  using BarComposerCallback = std::function<int(const Transport&, int)>;

  // Beat composer callback signature.
  using BeatComposerCallback =
      std::function<void(const Transport&, int, int, std::vector<Note>*)>;

  Ensemble(Sequencer* sequencer,
           SectionComposerCallback&& section_composer_callback,
           BarComposerCallback&& bar_composer_callback);

  void Add(Performer* performer, BeatComposerCallback&& beat_composer_callback);

 private:
  // Section composer callback.
  SectionComposerCallback section_composer_callback_;

  // Bar composer callback.
  BarComposerCallback bar_composer_callback_;

  // Current section type.
  int section_type_;

  // Current harmonic.
  int harmonic_;

  // List of performers with their corresponding composers.
  std::unordered_map<Performer*, BeatComposerCallback> performers_;

  std::vector<Note> temp_beat_notes_;
};

Ensemble::Ensemble(Sequencer* sequencer,
                   SectionComposerCallback&& section_composer_callback,
                   BarComposerCallback&& bar_composer_callback)
    : section_composer_callback_(std::move(section_composer_callback)),
      bar_composer_callback_(std::move(bar_composer_callback)),
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
    for (auto& it : performers_) {
      temp_beat_notes_.clear();
      it.second(transport, section_type_, harmonic_, &temp_beat_notes_);
      for (const Note& note : temp_beat_notes_) {
        const int start_offset_samples =
            start_sample +
            SamplesFromBeats(note.start_beat, num_samples_per_beat);
        it.first->StartNote(note.index, note.intensity, start_offset_samples);
        const int stop_offset_samples =
            start_offset_samples +
            SamplesFromBeats(note.duration_beats, num_samples_per_beat);
        it.first->StopNote(note.index, stop_offset_samples);
      }
    }
  });
}

void Ensemble::Add(Performer* performer,
                   BeatComposerCallback&& beat_composer_callback) {
  performers_.insert(
      std::make_pair(performer, std::move(beat_composer_callback)));
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
