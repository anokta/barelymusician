#ifndef EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_
#define EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/random.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {
namespace examples {

enum DrumkitIndices { kKick = 0, kSnare = 2, kHihatClosed = 4, kHihatOpen = 5 };

class SimpleDrumkitBeatComposer : public BeatComposer {
 public:
  // Implements |BeatComposer|.
  void Reset() override {}
  std::vector<Note> GetNotes(const Transport& transport, int section_type,
                             int harmonic) override {
    const float kKickIndex = static_cast<float>(DrumkitIndices::kKick);
    const float kSnareIndex = static_cast<float>(DrumkitIndices::kSnare);
    const float kHihatClosedIndex =
        static_cast<float>(DrumkitIndices::kHihatClosed);
    const float kHihatOpenIndex =
        static_cast<float>(DrumkitIndices::kHihatOpen);

    std::vector<Note> notes;

    // Kick.
    if (transport.beat % 2 == 0) {
      notes.push_back({kKickIndex, 1.0f, 0.0f, 1.0f});
      if (transport.bar % 2 == 1 && transport.beat == 0) {
        notes.push_back({kKickIndex, 1.0f, 0.5f, 0.5f});
      }
    }
    // Snare.
    if (transport.beat % 2 == 1) {
      notes.push_back({kSnareIndex, 1.0f, 0.0f, 1.0f});
    }
    if (transport.beat + 1 == transport.num_beats) {
      notes.push_back({kSnareIndex, 0.75f, 0.5f, 0.5f});
      if (transport.bar + 1 == transport.num_bars) {
        notes.push_back({kSnareIndex, 1.0f, 0.25f, 0.25f});
        notes.push_back({kSnareIndex, 0.75f, 0.75f, 0.25f});
      }
    }
    // Hihat Closed.
    notes.push_back(
        {kHihatClosedIndex, Random::Uniform(0.5f, 0.75f), 0.0f, 0.5f});
    notes.push_back(
        {kHihatClosedIndex, Random::Uniform(0.25f, 0.75f), 0.5f, 0.5f});
    // Hihat Open.
    if (transport.beat + 1 == transport.num_beats) {
      if (transport.bar + 1 == transport.num_bars) {
        notes.push_back({kHihatOpenIndex, 0.75f, 0.25f, 0.25f});
      } else if (transport.bar % 2 == 0) {
        notes.push_back({kHihatOpenIndex, 0.75f, 0.75f, 0.25f});
      }
    }
    if (transport.beat == 0 && transport.bar == 0) {
      notes.push_back({kHihatOpenIndex, 1.0f, 0.0f, 0.5f});
    }

    return notes;
  }
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_
