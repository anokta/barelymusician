#ifndef BARELYMUSICIAN_ENGINE_CMD_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_CMD_QUEUE_H_

#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <cstdint>
#include <utility>

#include "core/arena.h"
#include "engine/cmd.h"

namespace barely {

// Single-consumer single-producer command queue.
class CmdQueue {
 public:
  CmdQueue(Arena& arena, uint32_t max_cmd_count) noexcept
      : cmds_(arena.AllocArray<std::pair<int64_t, Cmd>>(max_cmd_count)),
        bit_mask_(max_cmd_count - 1) {
    assert(max_cmd_count > 0);
    assert(std::has_single_bit(max_cmd_count));
  }

  bool Add(int64_t cmd_frame, Cmd cmd) noexcept {
    const uint32_t index = write_index_.load(std::memory_order_relaxed);
    const uint32_t next_index = (index + 1) & bit_mask_;
    if (next_index == read_index_.load(std::memory_order_acquire)) {
      return false;
    }
    cmds_[index] = {cmd_frame, cmd};
    write_index_.store(next_index, std::memory_order_release);
    return true;
  }

  std::pair<int64_t, Cmd>* GetNext(int64_t end_frame) noexcept {
    const uint32_t index = read_index_.load(std::memory_order_relaxed);
    if (index == write_index_.load(std::memory_order_acquire) || cmds_[index].first >= end_frame) {
      return nullptr;
    }
    read_index_.store((index + 1) & bit_mask_, std::memory_order_release);
    return &cmds_[index];
  }

 private:
  // Array of commands with their timestamps in frames.
  std::pair<int64_t, Cmd>* cmds_ = nullptr;

  std::atomic<uint32_t> read_index_ = 0;
  std::atomic<uint32_t> write_index_ = 0;

  uint32_t bit_mask_ = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_CMD_QUEUE_H_
