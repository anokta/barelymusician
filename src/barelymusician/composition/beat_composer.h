#ifndef BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_
#define BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/module.h"
#include "barelymusician/composition/note.h"

namespace barelyapi {

// Beat composer interface to generate notes of a beat.
class BeatComposer : public Module {
 public:
  // Returns notes for the given |beat| index.
  //
  // @param section_type Section type.
  // @param bar Bar index.
  // @param num_bars Number of bars in section.
  // @param harmonic Harmonic progression of bar.
  // @param beat Beat Index.
  // @param num_beats Number of beats in bar.
  // @return List of notes in beat.
  virtual std::vector<Note> GetNotes(int section_type, int bar, int num_bars,
                                     int harmonic, int beat, int num_beats) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_
