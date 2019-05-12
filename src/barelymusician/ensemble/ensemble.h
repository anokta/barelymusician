#ifndef BARELYMUSICIAN_ENSEMBLE_ENSEMBLE_H_
#define BARELYMUSICIAN_ENSEMBLE_ENSEMBLE_H_

#include <algorithm>
#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/module.h"
#include "barelymusician/composition/bar_composer.h"
#include "barelymusician/composition/section_composer.h"
#include "barelymusician/ensemble/performer.h"
#include "barelymusician/sequencer/sequencer.h"

namespace barelyapi {

class Ensemble : public Module {
 public:
  Ensemble(Sequencer* sequencer, SectionComposer* section_composer,
           BarComposer* bar_composer);

  void AddPerformer(Performer* performer);

  void Reset() override;

 private:
  SectionComposer* const section_composer_;  // not owned.

  BarComposer* const bar_composer_;  // not owned.

  // Current section type.
  int section_type_;

  // Current harmonic.
  int harmonic_;

  // List of performers.
  std::vector<Performer*> performers_;
};

Ensemble::Ensemble(Sequencer* sequencer, SectionComposer* section_composer,
                   BarComposer* bar_composer)
    : section_composer_(section_composer),
      bar_composer_(bar_composer),
      section_type_(0),
      harmonic_(0) {
  DCHECK(sequencer);
  DCHECK(section_composer_);
  DCHECK(bar_composer_);
  sequencer->RegisterBeatCallback([&](const Transport& transport,
                                      int sample_offset,
                                      int num_beats_per_sample) {
    if (transport.beat == 0) {
      // New bar.
      if (transport.bar == 0) {
        // New section.
        section_type_ = section_composer_->GetSectionType(transport.section);
      }
      harmonic_ = bar_composer_->GetHarmonic(section_type_, transport.bar,
                                             transport.num_bars);
    }
    for (auto* performer : performers_) {
      performer->PerformBeat(transport, section_type_, harmonic_, sample_offset,
                             num_beats_per_sample);
    }
  });
}

void Ensemble::Reset() {
  section_type_ = 0;
  harmonic_ = 0;
  for (auto* performer : performers_) {
    performer->Reset();
  }
}

void Ensemble::AddPerformer(Performer* performer) {
  performers_.push_back(performer);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENSEMBLE_ENSEMBLE_H_
