#ifndef EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_
#define EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/random.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {
namespace examples {

class SimpleDrumkitBeatComposer : public BeatComposer {
 public:
  // Implements |BeatComposer|.
  void Reset() override {}
  std::vector<Note> GetNotes(const Transport& transport, int section_type,
                             int harmonic) override {
    std::vector<Note> notes;

    // Kick.
    if (transport.beat % 2 == 0) {
      notes.push_back({kNoteIndexKick, 1.0f, 0.0f, 1.0f});
      if (transport.bar % 2 == 1 && transport.beat == 0) {
        notes.push_back({kNoteIndexKick, 1.0f, 0.5f, 0.5f});
      }
    }

    // Snare.
    if (transport.beat % 2 == 1) {
      notes.push_back({kNoteIndexSnare, 1.0f, 0.0f, 1.0f});
    }
    if (transport.beat + 1 == transport.num_beats) {
      notes.push_back({kNoteIndexSnare, 0.75f, 0.5f, 0.5f});
      if (transport.bar + 1 == transport.num_bars) {
        notes.push_back({kNoteIndexSnare, 1.0f, 0.25f, 0.25f});
        notes.push_back({kNoteIndexSnare, 0.75f, 0.75f, 0.25f});
      }
    }

    // Hihat Closed.
    notes.push_back(
        {kNoteIndexHihatClosed, Random::Uniform(0.5f, 0.75f), 0.0f, 0.5f});
    notes.push_back(
        {kNoteIndexHihatClosed, Random::Uniform(0.25f, 0.75f), 0.5f, 0.5f});

    // Hihat Open.
    if (transport.beat + 1 == transport.num_beats) {
      if (transport.bar + 1 == transport.num_bars) {
        notes.push_back({kNoteIndexHihatOpen, 0.75f, 0.25f, 0.25f});
      } else if (transport.bar % 2 == 0) {
        notes.push_back({kNoteIndexHihatOpen, 0.75f, 0.75f, 0.25f});
      }
    }
    if (transport.beat == 0 && transport.bar == 0) {
      notes.push_back({kNoteIndexHihatOpen, 1.0f, 0.0f, 0.5f});
    }

    return notes;
  }
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_SIMPLE_DRUMKIT_BEAT_COMPOSER_H_
