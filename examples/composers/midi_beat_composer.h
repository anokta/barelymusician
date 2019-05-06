#ifndef EXAMPLES_COMPOSERS_MIDI_BEAT_COMPOSER_H_
#define EXAMPLES_COMPOSERS_MIDI_BEAT_COMPOSER_H_

#include <iterator>
#include <vector>

#include "MidiFile.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {
namespace examples {

class MidiBeatComposer : public BeatComposer {
 public:
  MidiBeatComposer(const smf::MidiEventList& midi_events,
                   int ticks_per_quarter) {
    const float ticks_per_beat = static_cast<float>(ticks_per_quarter);
    const float max_velocity = 128.0f;
    for (int i = 0; i < midi_events.size(); ++i) {
      const auto& midi_event = midi_events[i];
      if (midi_event.isNoteOn()) {
        Note note;
        note.index = static_cast<float>(midi_event.getKeyNumber());
        note.intensity =
            static_cast<float>(midi_event.getVelocity()) / max_velocity;
        note.start_beat = static_cast<float>(midi_event.tick) / ticks_per_beat;
        note.duration_beats =
            static_cast<float>(midi_event.getTickDuration()) / ticks_per_beat;
        score_.push_back(note);
      }
    }
  }

  // Implements |BeatComposer|.
  void Reset() override {}
  std::vector<Note> GetNotes(const Transport& transport, int section_type,
                             int harmonic) override {
    std::vector<Note> notes;

    const auto compare_beat = [](const Note& note, float start_beat) {
      return note.start_beat < start_beat;
    };
    const float beat = static_cast<float>(
        (transport.section * transport.num_bars + transport.bar) *
            transport.num_beats +
        transport.beat);
    const auto begin =
        std::lower_bound(score_.begin(), score_.end(), beat, &compare_beat);
    const auto end =
        std::lower_bound(begin, score_.end(), beat + 1.0f, &compare_beat);
    for (auto it = begin; it != end; ++it) {
      notes.push_back(*it);
    }

    return notes;
  }

 private:
  std::vector<Note> score_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_COMPOSERS_MIDI_BEAT_COMPOSER_H_
