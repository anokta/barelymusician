#include "barelymusician/base/id.h"

namespace barelyapi {

namespace {

// Id counter.
int id_counter = 0;

}  // namespace

int GetNextId() { return ++id_counter; }

void ResetIdCount() { id_counter = 0; }

}  // namespace barelyapi
