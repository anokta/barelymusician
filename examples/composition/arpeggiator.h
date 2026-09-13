#ifndef BARELYMUSICIAN_EXAMPLES_COMPOSITION_ARPEGGIATOR_H_
#define BARELYMUSICIAN_EXAMPLES_COMPOSITION_ARPEGGIATOR_H_

#include <barelymusician.h>

#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace barely::examples {

// Class that arpeggiates a sequence of instrument notes.
class Arpeggiator {
 public:
  enum class Mode : uint8_t {
    kUp = 0,
    kDown,
    kRandom,
  };

  Arpeggiator(Engine& engine, Instrument instrument) noexcept;
  ~Arpeggiator() noexcept { performer_.Destroy(); }

  void SetAllNotesOff() noexcept;
  void SetNoteOff(double pitch) noexcept;
  void SetNoteOn(double pitch) noexcept;
  void SetRate(double rate) noexcept;

  void SetGateRatio(double gate_ratio) noexcept { task_.SetDuration(loop_length_ * gate_ratio); }
  void SetMode(Mode mode) noexcept { mode_ = mode; }
  void SetNoteCallback(std::function<void(double)> note_callback) noexcept {
    note_callback_ = std::move(note_callback);
  }
  [[nodiscard]] bool IsPlaying() const noexcept { return index_ != -1; }

 private:
  void Stop() noexcept;
  void Update() noexcept;

  Engine& engine_;
  Instrument instrument_;
  Performer performer_;
  Task task_;

  std::function<void(double)> note_callback_;
  std::vector<double> pitches_;

  double loop_length_ = 1.0;
  double gate_ratio_ = 0.5;
  double pitch_ = 0.0;
  int index_ = -1;
  Mode mode_ = Mode::kUp;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMPOSITION_ARPEGGIATOR_H_
