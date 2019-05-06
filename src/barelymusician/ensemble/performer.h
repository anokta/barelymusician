#ifndef BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
#define BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_

#include "barelymusician/base/logging.h"
#include "barelymusician/base/module.h"
#include "barelymusician/composition/beat_composer.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/sequencer/transport.h"

namespace barelyapi {

class Performer : public Module {
 public:
  Performer(Instrument* instrument, BeatComposer* composer);

  // Implements |Module|.
  void Reset() override;

  void PerformBeat(const Transport& transport, int section_type, int harmonic,
                   int sample_offset, int num_samples_per_beat);

 private:
  void PlayNote(float index, float intensity, int start_sample,
                int duration_samples);

  Instrument* const instrument_;  // not owned.

  BeatComposer* const composer_;  // not owned.
};

Performer::Performer(Instrument* instrument, BeatComposer* composer)
    : instrument_(instrument), composer_(composer) {
  DCHECK(instrument_);
  DCHECK(composer_);
}

void Performer::Reset() {
  composer_->Reset();
  instrument_->Reset();
}

void Performer::PerformBeat(const Transport& transport, int section_type,
                            int harmonic, int sample_offset,
                            int num_samples_per_beat) {
  const float num_samples_per_beat_float =
      static_cast<float>(num_samples_per_beat);
  const auto notes = composer_->GetNotes(transport, section_type, harmonic);
  for (const auto& note : notes) {
    const float index = note.index;
    const float intensity = note.intensity;
    const int start_sample =
        sample_offset +
        static_cast<int>(note.start_beat * num_samples_per_beat_float);
    const int duration_samples =
        static_cast<int>(note.duration_beats * num_samples_per_beat_float);
    PlayNote(index, intensity, start_sample, duration_samples);
  }
}

void Performer::PlayNote(float index, float intensity, int start_sample,
                         int duration_samples) {
  instrument_->StartNote(index, intensity, start_sample);
  instrument_->StopNote(index, start_sample + duration_samples);
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
