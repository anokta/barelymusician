#include "barelymusician/base/id.h"

namespace barelyapi {

std::int64_t GetNextId() {
  static std::int64_t id_counter = 0;
  return ++id_counter;
}

}  // namespace barelyapi
