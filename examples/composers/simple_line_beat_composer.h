#ifndef EXAMPLES_COMPOSERS_SIMPLE_LINE_BEAT_COMPOSER_H_
#define EXAMPLES_COMPOSERS_SIMPLE_LINE_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/random.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"

namespace barelyapi {
namespace examples {

// TODO(#26): Root note and scale will be governed by |Conductor|?
class SimpleLineBeatComposer : public BeatComposer {
 public:
  SimpleLineBeatComposer(float root_note, const std::vector<float>& scale)
      : root_note_(root_note), scale_(scale) {}

  // Implements |BeatComposer|.
  void Reset() override {}
  std::vector<Note> GetNotes(const Transport& transport, int section_type,
                             int harmonic) override {
    const float start_note = static_cast<float>(harmonic);
    std::vector<Note> notes;
    if (transport.beat % 2 == 1) {
      notes.push_back(BuildNote(start_note, 0.0f, 0.25f));
      notes.push_back(BuildNote(start_note - static_cast<float>(transport.beat),
                                0.33f, 0.25f));
      notes.push_back(BuildNote(start_note, 0.66f, 0.25f));
    } else {
      notes.push_back(BuildNote(start_note + static_cast<float>(transport.beat),
                                0.0f, 0.25f));
    }
    if (transport.beat % 2 == 0) {
      notes.push_back(BuildNote(start_note - static_cast<float>(transport.beat),
                                0.0f, 0.05f));
      notes.push_back(BuildNote(
          start_note - static_cast<float>(2.0f * transport.beat), 0.5f, 0.05f));
    }
    if (transport.beat + 1 == transport.num_beats && transport.bar % 2 == 1) {
      notes.push_back(
          BuildNote(start_note + static_cast<float>(2.0f * transport.beat),
                    0.25f, 0.125f));
      // notes.push_back(BuildNote(start_note -
      // static_cast<float>(transport.beat),
      //                          0.5f, 0.25f));
      notes.push_back(
          BuildNote(start_note - static_cast<float>(2.0f * transport.beat),
                    0.75f, 0.125f));
      notes.push_back(BuildNote(
          start_note + static_cast<float>(2.0f * transport.beat), 0.5f, 0.25f));
    }
    return notes;
  }

 private:
  Note BuildNote(float index, float start_beat, float duration_beats) {
    const float intensity = Random::Uniform(0.5f, 1.0f);
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

#endif  // EXAMPLES_COMPOSERS_SIMPLE_LINE_BEAT_COMPOSER_H_
