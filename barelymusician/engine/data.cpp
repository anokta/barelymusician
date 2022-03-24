#include "barelymusician/engine/data.h"

#include <utility>

#include "barelymusician/barelymusician.h"

namespace barelyapi {

using ::barely::DataDefinition;

Data::Data(DataDefinition definition) noexcept
    : move_callback_(definition.move_callback),
      destroy_callback_(definition.destroy_callback) {
  if (move_callback_) {
    move_callback_(definition.data, &data_);
  }
}

Data::~Data() noexcept {
  if (data_ && destroy_callback_) {
    destroy_callback_(data_);
  }
  data_ = nullptr;
}

Data::Data(Data&& other) noexcept
    : move_callback_(std::exchange(other.move_callback_, nullptr)),
      destroy_callback_(std::exchange(other.destroy_callback_, nullptr)),
      data_(std::exchange(other.data_, nullptr)) {}

Data& Data::operator=(Data&& other) noexcept {
  if (this != &other) {
    if (data_ && destroy_callback_) {
      destroy_callback_(data_);
    }
    move_callback_ = std::exchange(other.move_callback_, nullptr);
    destroy_callback_ = std::exchange(other.destroy_callback_, nullptr);
    data_ = std::exchange(other.data_, nullptr);
  }
  return *this;
}

void* Data::GetMutable() const noexcept { return data_; }

}  // namespace barelyapi
