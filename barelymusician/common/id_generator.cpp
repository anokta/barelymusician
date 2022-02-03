#include "barelymusician/common/id_generator.h"

#include "barelymusician/common/id.h"

namespace barelyapi {

Id IdGenerator::Next() noexcept { return ++counter_; }

}  // namespace barelyapi
