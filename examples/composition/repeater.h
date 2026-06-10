#ifndef BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
#define BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_

#include <barelymusician.h>

#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace barely::examples {

// Class that plays a repeating sequence of instrument notes.
class Repeater {
 public:
  enum class Mode : uint8_t {
    kForward = 0,
    kBackward,
    kRandom,
  };

  Repeater(Engine& engine, Instrument instrument) noexcept;
  ~Repeater() noexcept { performer_.Destroy(); }

  void Clear() noexcept;
  void Pop() noexcept;
  void Push(std::optional<float> pitch_or, int length = 1) noexcept;

  void SetRate(double rate) noexcept;
  void Start(float pitch_offset = 0.0) noexcept;
  void Stop() noexcept;

  void SetMode(Mode mode) noexcept { mode_ = mode; }
  void SetNoteCallback(std::function<void(float)> note_callback) noexcept {
    note_callback_ = std::move(note_callback);
  }

  [[nodiscard]] bool IsPlaying() const noexcept { return index_ != -1; }

 private:
  void OnBeat() noexcept;
  [[nodiscard]] bool Update() noexcept;

  Engine& engine_;
  Instrument instrument_;
  Performer performer_;
  Task task_;

  std::function<void(float)> note_callback_;
  std::vector<std::pair<std::optional<float>, int>> pitches_;

  Mode mode_ = Mode::kForward;
  int index_ = -1;
  float pitch_offset_ = 0.0;
  int remaining_length_ = 0;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
