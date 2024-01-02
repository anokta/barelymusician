#ifndef BARELYMUSICIAN_INTERNAL_ENGINE_H_
#define BARELYMUSICIAN_INTERNAL_ENGINE_H_

#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/mutable.h"
#include "barelymusician/internal/observable.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Internal engine.
class Engine {
 public:
  /// Default constructor.
  Engine() = default;

  /// Destroys `Engine`.
  ~Engine() noexcept = default;

  // Non-copyable and non-movable.
  Engine(const Engine& other) noexcept = delete;
  Engine& operator=(const Engine& other) noexcept = delete;
  Engine(Engine&& other) noexcept = delete;
  Engine& operator=(Engine&& other) noexcept = delete;

  /// Creates a new instrument.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @return Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Observer<Instrument> CreateInstrument(InstrumentDefinition definition, int frame_rate) noexcept;

  /// Creates a new performer.
  ///
  /// @return Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Observer<Performer> CreatePerformer() noexcept;

  /// Creates a new performer task.
  ///
  /// @param performer Pointer to performer.
  /// @param definition Task definition.
  /// @param is_one_off True if one-off task, false otherwise.
  /// @param position Task position in beats.
  /// @param process_order Task process order.
  /// @param user_data Pointer to user data.
  /// @return Optional task identifier.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  std::optional<Id> CreatePerformerTask(Performer* performer, TaskDefinition definition,
                                        bool is_one_off, double position, int process_order,
                                        void* user_data) noexcept;

  /// Destroys instrument.
  ///
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyInstrument(Instrument& instrument) noexcept;

  /// Destroys performer.
  ///
  /// @param performer Performer.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyPerformer(Performer& performer) noexcept;

  /// Returns tempo.
  ///
  /// @return Tempo in beats per minute.
  [[nodiscard]] double GetTempo() const noexcept;

  /// Returns timestamp.
  ///
  /// @return Timestamp in seconds.
  [[nodiscard]] double GetTimestamp() const noexcept;

  /// Processes instrument at timestamp.
  ///
  /// @param instrument Pointer to instrument.
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ProcessInstrument(Id instrument_id, double* output_samples, int output_channel_count,
                         int output_frame_count, double timestamp) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the engine at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Update(double timestamp) noexcept;

 private:
  // Generates the next identifier to use.
  Id GenerateNextId() noexcept;

  // Monotonic identifier counter.
  Id id_counter_ = 0;

  // Map of instruments by pointers.
  std::unordered_map<Instrument*, Observable<Instrument>> instruments_;

  // Map of performers by pointers.
  std::unordered_map<Performer*, Observable<Performer>> performers_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_ENGINE_H_
