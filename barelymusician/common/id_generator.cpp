#include "barelymusician/common/id_generator.h"

namespace barelyapi {

IdGenerator::IdGenerator() : counter_(0) {}

int IdGenerator::Next() { return ++counter_; }

}  // namespace barelyapi
