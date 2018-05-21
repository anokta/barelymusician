#include "barelymusician/base/logging.h"

#include <cstdlib>
#include <string>

namespace barelyapi {
namespace logging {

namespace {

// Returns base filename of the given |filepath|.
//
// @param filepath Full file path.
// @return Base filename with extension.
std::string GetBaseFilename(const std::string& filepath) {
  return filepath.substr(filepath.find_last_of("/\\") + 1);
}

// Returns a string representation of the given log |severity|.
//
// @param severity Log severity.
// @return Log severity in string format.
std::string GetStringFromSeverity(Logger::Severity severity) {
  switch (severity) {
    case Logger::Severity::INFO:
      return "INFO";
    case Logger::Severity::WARNING:
      return "WARNING";
    case Logger::Severity::ERROR:
      return "ERROR";
    case Logger::Severity::FATAL:
      return "FATAL";
    default:
      break;
  }
  return "";
}

}  // namespace

Logger::Logger(std::ostream& out, Severity severity, const char* file, int line)
    : out_(out), severity_(severity) {
  out_ << GetStringFromSeverity(severity) << " [" << GetBaseFilename(file)
       << ":" << line << "] ";
}

Logger::~Logger() {
  out_ << std::endl;
  if (severity_ == Severity::FATAL) {
    // Fatal error received, abort the program.
    std::abort();
  }
}

std::ostream& Logger::GetStream() { return out_; }

std::ostream& Logger::NullStream() {
  static std::ostream null_out(nullptr);
  return null_out;
}

}  // namespace logging
}  // namespace barelyapi
