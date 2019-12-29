#ifndef BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
#define BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/composition/note.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"

namespace barelyapi {

struct Ensemble {
  // Section composer callback signature.
  using SectionComposerCallback = std::function<int(int section)>;

  // Bar composer callback signature.
  using BarComposerCallback =
      std::function<int(int bar, int num_bars, int section_type)>;

  // Beat composer callback signature.
  using BeatComposerCallback =
      std::function<void(int bar, int beat, int section_type, int harmonic,
                         std::vector<Note>* notes)>;

  // Section composer callback.
  SectionComposerCallback section_composer_callback;

  // Bar composer callback.
  BarComposerCallback bar_composer_callback;

  // List of performers.
  std::vector<std::pair<Instrument*, BeatComposerCallback>> performers;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
