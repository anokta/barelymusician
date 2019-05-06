#ifndef EXAMPLES_COMPOSERS_SIMPLE_CHORDS_BEAT_COMPOSER_H_
#define EXAMPLES_COMPOSERS_SIMPLE_CHORDS_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"

namespace barelyapi {
namespace examples {

// TODO(#26): Root note and scale will be governed by |Conductor|?
class SimpleChordsBeatComposer : public BeatComposer {
 public:
  SimpleChordsBeatComposer(float root_note, const std::vector<float>& scale)
      : root_note_(root_note), scale_(scale) {}

  // Implements |BeatComposer|.
  void Reset() override {}
  std::vector<Note> GetNotes(const Transport& transport, int section_type,
                             int harmonic) override {
    std::vector<Note> notes;
    const float start_note = static_cast<float>(harmonic);
    notes.push_back(BuildNote(start_note, 0.0f, 1.0f));
    notes.push_back(BuildNote(start_note + 2.0f, 0.0f, 1.0f));
    notes.push_back(BuildNote(start_note + 4.0f, 0.0f, 1.0f));
    notes.push_back(BuildNote(start_note + 7.0f, 0.0f, 1.0f));
    notes.push_back(BuildNote(start_note - 7.0f, 0.0f, 1.0f));
    return notes;
  }

 private:
  Note BuildNote(float index, float start_beat, float duration_beats) {
    const float intensity = 0.5f;
    Note note;
    note.index = root_note_ + GetScaledNoteIndex(index, scale_);
    note.intensity = intensity;
    note.start_beat = start_beat;
    note.duration_beats = duration_beats;
    return note;
  }

  // Root note.
  float root_note_;

  // Scale.
  std::vector<float> scale_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_SIMPLE_CHORDS_BEAT_COMPOSER_H_
