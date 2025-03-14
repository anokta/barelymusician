#ifndef BARELYMUSICIAN_EXAMPLES_DATA_DATA_H_
#define BARELYMUSICIAN_EXAMPLES_DATA_DATA_H_

#include <string>
#include <string_view>

namespace barely::examples {

/// Returns the absolute data file path for a given relative file path.
///
/// @param relative_file_path Relative file path.
/// @param argv Array of command line arguments.
/// @return Absolute data file path.
std::string GetDataFilePath(std::string_view relative_file_path, char* argv[]);

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_DATA_DATA_H_
