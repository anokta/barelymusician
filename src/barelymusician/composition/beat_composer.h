#ifndef BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_
#define BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_

#include <vector>

#include "barelymusician/base/module.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/sequencer/transport.h"

namespace barelyapi {

// Beat composer interface to generate notes of a beat.
class BeatComposer : public Module {
 public:
  // Returns notes for the given playback |transport|.
  //
  // @param transport Playback transport.
  // @param section_type Section type.
  // @param harmonic Harmonic progression of bar.
  // @return List of notes in beat.
  virtual std::vector<Note> GetNotes(const Transport& transport,
                                     int section_type, int harmonic) = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_BEAT_COMPOSER_H_
