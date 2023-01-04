#include "barelymusician/engine/event.h"

namespace barely::internal {

Event::Event(const Definition& definition, void* user_data) noexcept
    : destroy_callback_(definition.destroy_callback),
      process_callback_(definition.process_callback) {
  if (definition.create_callback) {
    definition.create_callback(&state_, user_data);
  }
}

Event::~Event() noexcept {
  if (destroy_callback_) {
    destroy_callback_(&state_);
  }
}

void Event::Process() noexcept {
  if (process_callback_) {
    process_callback_(&state_);
  }
}

}  // namespace barely::internal
