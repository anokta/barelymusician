#include "barelymusician/common/logging.h"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

namespace barelyapi::logging {

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
std::string GetStringFromSeverity(LogSeverity severity) {
  switch (severity) {
    case LogSeverity::INFO:
      return "INFO";
    case LogSeverity::WARNING:
      return "WARNING";
    case LogSeverity::ERROR:
      return "ERROR";
    case LogSeverity::FATAL:
      return "FATAL";
    default:
      break;
  }
  return "";
}

// Custom log writer.
LogWriter* custom_log_writer = nullptr;

}  // namespace

Logger::Logger(LogSeverity severity, const char* file, int line)
    : severity_(severity) {
  stream_ << "[" << GetBaseFilename(file) << ":" << line << "] ";
}

Logger::~Logger() {
  LogWriter& writer =
      custom_log_writer ? *custom_log_writer : GetDefaultLogWriter();
  writer.Write(severity_, stream_.str());
  if (severity_ == LogSeverity::FATAL) {
    // Fatal error received, abort the program.
    std::abort();
  }
}

std::ostream& Logger::GetStream() { return stream_; }

void CerrLogWriter::Write(LogSeverity severity, const std::string& message) {
  std::cerr << GetStringFromSeverity(severity) << " " << message << std::endl;
}

LogWriter& GetDefaultLogWriter() {
  static LogWriter* const default_log_writer = new CerrLogWriter();
  return *default_log_writer;
}

std::ostream& GetNullStream() {
  static std::ostream* const null_stream = new std::ostream(nullptr);
  return *null_stream;
}

void SetLogWriter(LogWriter* log_writer) { custom_log_writer = log_writer; }

}  // namespace barelyapi::logging
