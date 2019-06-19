#ifndef BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
#define BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/base/transport.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"

namespace barelyapi {

struct Ensemble {
  // Section composer callback signature.
  using SectionComposerCallback = std::function<int(const Transport&)>;

  // Bar composer callback signature.
  using BarComposerCallback = std::function<int(const Transport&, int)>;

  // Beat composer callback signature.
  using BeatComposerCallback =
      std::function<void(const Transport&, int, int, std::vector<Note>*)>;

  struct Performer {
    Performer(BeatComposerCallback beat_composer_callback)
        : beat_composer_callback(std::move(beat_composer_callback)) {}
    BeatComposerCallback beat_composer_callback;
    MessageBuffer messages;
  };

  // Section composer callback.
  SectionComposerCallback section_composer_callback;

  // Bar composer callback.
  BarComposerCallback bar_composer_callback;

  // List of performers with their corresponding composers.
  std::unordered_map<Instrument*, Performer> performers;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_ENSEMBLE_H_
