#include "data/data.h"

#include <string>
#include <string_view>

namespace barely::examples {

std::string GetDataFilePath(std::string_view relative_file_path, [[maybe_unused]] char* argv[]) {
  return std::string(BARELY_EXAMPLES_DATA_DIR).append("/").append(relative_file_path);
}

}  // namespace barely::examples
