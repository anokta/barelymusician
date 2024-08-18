#include "barelymusician/composition/scale.h"

#include <cassert>
#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"

// Scale.
struct BarelyScale {
  BarelyScale(const BarelyScaleDefinition& definition, double root_note) noexcept
      : ratios_(definition.ratios, definition.ratios + definition.ratio_count),
        root_note_(root_note) {
    assert(definition.ratio_count == 0 || definition.ratios != nullptr);
  }

  // Returns the corresponding note for a given scale degree.
  double GetNote(int degree) const noexcept {
    if (ratios_.empty()) {
      return 0.0;
    }
    const int note_count = GetNoteCount();
    const int octave =
        static_cast<int>(std::floor(static_cast<double>(degree) / static_cast<double>(note_count)));
    const int index = degree - octave * note_count;
    assert(index >= 0 && index < note_count);
    return root_note_ * std::pow(ratios_[note_count - 1], octave) *
           (index > 0 ? ratios_[index - 1] : 1.0);
  }

  // Returns number of notes.
  int GetNoteCount() const noexcept { return static_cast<int>(ratios_.size()); }

 private:
  // Array of note ratios.
  std::vector<double> ratios_;

  // Root note.
  double root_note_ = 0.0;
};

bool BarelyScale_Create(BarelyScaleDefinition definition, double root_note,
                        BarelyScale** out_scale) {
  if (out_scale == nullptr) return false;

  *out_scale = new BarelyScale(definition, root_note);
  return true;
}

bool BarelyScale_Destroy(BarelyScale* scale) {
  if (scale == nullptr) return false;

  delete scale;
  return true;
}

bool BarelyScale_GetNote(const BarelyScale* scale, int32_t degree, double* out_note) {
  if (out_note == nullptr) return false;

  *out_note = scale->GetNote(degree);
  return true;
}

bool BarelyScale_GetNoteCount(const BarelyScale* scale, int32_t* out_note_count) {
  if (out_note_count == nullptr) return false;

  *out_note_count = scale->GetNoteCount();
  return true;
}
