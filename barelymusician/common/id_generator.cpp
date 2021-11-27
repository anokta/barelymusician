#include "barelymusician/common/id_generator.h"

#include "barelymusician/common/id.h"

namespace barely {

IdGenerator::IdGenerator() noexcept : counter_(0) {}

Id IdGenerator::Next() noexcept { return ++counter_; }

}  // namespace barely
