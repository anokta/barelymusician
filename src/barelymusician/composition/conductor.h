#ifndef BARELYMUSICIAN_COMPOSITION_CONDUCTOR_H_
#define BARELYMUSICIAN_COMPOSITION_CONDUCTOR_H_

#include <algorithm>
#include <vector>

#include "barelymusician/base/module.h"
#include "barelymusician/base/random.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/composition/note_utils.h"

namespace barelyapi {

enum class NoteType {
  kRaw,
  kInKey,
  kChromatic,
};

// Conductor interface that transforms given notes with respect to its set of
// configuration.
class Conductor : public Module {
 public:
  Conductor() : root_note_index_(0.0f), energy_(0.5f), stress_(0.5f) {}

  float tempo_multiplier() const { return tempo_multiplier_; }

  // Implements |Module|.
  void Reset() override {
    energy_ = 0.5f;
    stress_ = 0.5f;
  }

  Note BuildNote(float index, float intensity, float start_beat,
                 float duration_beats, NoteType note_type) const {
    Note note;

    const float relative_index =
        std::copysign(index, std::round(harmonic_curve_)) +
        std::round(harmonic_height_) * scale_.size();

    switch (note_type) {
      case NoteType::kInKey:
        note.index =
            root_note_index_ + GetScaledNoteIndex(relative_index, scale_);
        break;
      case NoteType::kChromatic:
        note.index = root_note_index_ + index;
        break;
      case NoteType::kRaw:
      default:
        note.index = index;
        break;
    }

    const float duration_mean = duration_beats * articulation_multiplier_;
    const float duration_variance = duration_mean * articulation_variance_;
    const float loudness_mean = intensity * loudness_multiplier_;
    const float loudness_variance = loudness_mean * loudness_variance_;
    note.intensity =
        std::max(0.0f, loudness_variance > 0.0f
                           ? Random::Normal(loudness_mean, loudness_variance)
                           : loudness_mean);
    note.start_beat = start_beat;
    note.duration_beats =
        std::max(0.0f, duration_variance > 0.0f
                           ? Random::Normal(duration_mean, duration_variance)
                           : duration_mean);

    return note;
  }

  // Sets the energy (arousal) of score.
  //
  // @param energy Energy in range [0, 1].
  void SetEnergy(float energy) {
    energy_ = std::min(std::max(energy, 0.0f), 1.0f);
    SetParameters(energy_, stress_);
  }

  // Sets the root note (key) of score.
  //
  // @param index Root note index.
  void SetRootNote(float index) { root_note_index_ = index; }

  void SetScale(const std::vector<float>& scale) { scale_ = scale; }

  // Sets the stress (valence) of score.
  //
  // @param stress Stress in range [0, 1].
  void SetStress(float stress) {
    stress_ = std::min(std::max(stress, 0.0f), 1.0f);
    SetParameters(energy_, stress_);
  }

 private:
  // TODO(#45): The values should be justified!
  void SetParameters(float energy, float stress) {
    tempo_multiplier_ = 0.85f + 0.3f * energy;
    articulation_multiplier_ = 0.25f + 1.75f * (1.0f - energy);
    loudness_multiplier_ = 0.4f + 0.6f * energy;
    articulation_variance_ = 0.15f * energy;

    loudness_variance_ = 0.125f * (energy + stress);
    harmonic_curve_ = (stress > 0.5f)
                          ? (1.5f * (1.0f - stress) + 0.5f * (1.0f - energy))
                          : 1.0f;
    harmonic_height_ = 3.0f * (energy * 0.25f + (1.0f - stress) * 0.75f) - 2.0f;
  }

  float root_note_index_;

  std::vector<float> scale_;

  float energy_;

  float stress_;

  // Sequencer tempo (BPM) adjustment.
  float tempo_multiplier_;

  // Articulation adjustment for musical notes.
  float articulation_multiplier_;

  // Articulation variance for musical notes.
  float articulation_variance_;

  // Loudness adjustment for musical notes.
  float loudness_multiplier_;

  // Loudness variance for musical notes.
  float loudness_variance_;

  // Harmonic curve for musical phrases.
  float harmonic_curve_;

  // Harmonic (pitch) height of musical notes.
  float harmonic_height_;

  // TODO(anokta): Add harmonic complexity as a parameter?
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_CONDUCTOR_H_
