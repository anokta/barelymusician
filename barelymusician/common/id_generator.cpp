#include "barelymusician/common/id_generator.h"

namespace barelyapi {

IdGenerator::IdGenerator() : counter_(0) {}

Id IdGenerator::Generate() { return ++counter_; }

}  // namespace barelyapi
