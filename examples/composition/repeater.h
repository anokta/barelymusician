#ifndef BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
#define BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_

#include <barelymusician.h>

#include <optional>
#include <utility>
#include <vector>

namespace barely::examples {

enum class RepeaterMode {
  kForward = 0,
  kBackward,
  kRandom,
};

// Class that plays a repeating sequence of instrument notes.
struct Repeater {
 public:
  Repeater(Engine& engine, Instrument instrument) noexcept;
  ~Repeater() noexcept { engine_.DestroyPerformer(performer_); }

  void Clear() noexcept;
  void Pop() noexcept;
  void Push(std::optional<float> pitch_or, int length = 1) noexcept;

  void Start(float pitch_offset = 0.0) noexcept;
  void Stop() noexcept;

  void SetRate(double rate) noexcept;
  void SetStyle(RepeaterMode style) noexcept;

  bool IsPlaying() const noexcept { return performer_.IsPlaying(); }

 private:
  void OnBeat() noexcept;
  [[nodiscard]] bool Update() noexcept;

  Engine& engine_;
  Instrument instrument_;
  Performer performer_;
  Task task_;

  std::vector<std::pair<std::optional<float>, int>> pitches_;

  RepeaterMode mode_ = RepeaterMode::kForward;
  int index_ = -1;
  float pitch_offset_ = 0.0;
  int remaining_length_ = 0;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
