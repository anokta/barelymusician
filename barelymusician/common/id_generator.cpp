#include "barelymusician/common/id_generator.h"

#include "barelymusician/common/id.h"

namespace barely {

IdGenerator::IdGenerator() : counter_(0) {}

Id IdGenerator::Generate() { return ++counter_; }

}  // namespace barely
