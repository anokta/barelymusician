#ifndef BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
#define BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/module.h"
#include "barelymusician/composition/conductor.h"
#include "barelymusician/composition/performer.h"
#include "barelymusician/sequencer/sequencer.h"
#include "barelymusician/sequencer/transport.h"

namespace barelyapi {

class Ensemble : public Module {
 public:
  // Section composer callback signature.
  using SectionComposerCallback = std::function<int(const Transport&)>;

  // Bar composer callback signature.
  using BarComposerCallback = std::function<int(const Transport&, int)>;

  // Beat composer callback signature.
  using BeatComposerCallback = std::function<std::vector<Note>(
      const Conductor& conductor, const Transport&, int, int)>;

  explicit Ensemble(Sequencer* sequencer, const Scale& scale);

  // Implements |Module|.
  void Reset() override;

  void AddPerformer(Performer* performer,
                    BeatComposerCallback&& beat_composer_callback);

  void SetSectionComposerCallback(
      SectionComposerCallback&& section_composer_callback);

  void SetBarComposerCallback(BarComposerCallback&& bar_composer_callback);

  Conductor& conductor() { return conductor_; }

 private:
  Conductor conductor_;

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
};

Ensemble::Ensemble(Sequencer* sequencer, const Scale& scale)
    : conductor_(scale),
      section_composer_callback_(nullptr),
      bar_composer_callback_(nullptr),
      section_type_(0),
      harmonic_(0) {
  DCHECK(sequencer);
  sequencer->RegisterBeatCallback([&](const Transport& transport,
                                      int start_sample,
                                      int num_beats_per_sample) {
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
      const auto& notes =
          performer.second(conductor_, transport, section_type_, harmonic_);
      performer.first->Perform(notes, start_sample, num_beats_per_sample);
    }
  });
}

void Ensemble::Reset() {
  section_type_ = 0;
  harmonic_ = 0;
  for (auto& performer : performers_) {
    performer.first->Reset();
  }
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
