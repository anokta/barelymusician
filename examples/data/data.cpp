#include "examples/data/data.h"

#ifdef BARELYMUSICIAN_CMAKE
#include <string>
#include <string_view>
#else  // BARELYMUSICIAN_CMAKE
#include <cassert>
#include <memory>
#include <string>
#include <string_view>

#include "tools/cpp/runfiles/runfiles.h"
#endif  // BARELYMUSICIAN_CMAKE

namespace barely::examples {

std::string GetDataFilePath(std::string_view relative_file_path, [[maybe_unused]] char* argv[]) {
#ifdef BARELYMUSICIAN_CMAKE
  return std::string(BARELYMUSICIAN_EXAMPLES_DATA_DIR).append("/").append(relative_file_path);
#else   // BARELYMUSICIAN_CMAKE
  static auto runfiles = std::unique_ptr<bazel::tools::cpp::runfiles::Runfiles>(
      bazel::tools::cpp::runfiles::Runfiles::Create(argv[0]));
  assert(runfiles);
  return runfiles->Rlocation(
      std::string("barelymusician/examples/data/").append(relative_file_path));
#endif  // BARELYMUSICIAN_CMAKE
}

}  // namespace barely::examples
