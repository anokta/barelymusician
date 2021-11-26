#ifndef BARELYMUSICIAN_COMMON_LOGGING_H_
#define BARELYMUSICIAN_COMMON_LOGGING_H_

#include <ostream>
#include <sstream>
#include <string>

/// Logging macros.
#define LOG(severity) BARELY_LOG(severity)

namespace barely::logging {

/// Log severity.
enum class LogSeverity {
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
};

/// Logging class that wraps assertion and log stream for debugging.
class Logger {
 public:
  /// Constructs new |Logger|.
  ///
  /// @param severity Log severity.
  /// @param file File path.
  /// @param line Line number.
  Logger(LogSeverity severity, const char* file, int line);

  /// Destroys |Logger| after writing the log stream.
  ~Logger();

  /// Returns the log stream.
  ///
  /// @return Log stream.
  std::ostream& GetStream();

 private:
  // Log severity.
  LogSeverity severity_;

  // Logging stream.
  std::ostringstream stream_;
};

/// Log writer interface.
class LogWriter {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  ~LogWriter() = default;

  /// Writes log message.
  ///
  /// @param severity Log severity.
  /// @param message Log message.
  virtual void Write(LogSeverity severity, const std::string& message) = 0;
};

/// Log writer that outputs to cerr.
class CerrLogWriter : public LogWriter {
 public:
  /// Implements |LogWriter|.
  void Write(LogSeverity severity, const std::string& message) override;
};

/// Returns default log writer.
///
/// @return Default log writer.
LogWriter& GetDefaultLogWriter();

/// Returns null stream.
///
/// @return Null stream.
std::ostream& GetNullStream();

/// Sets log writer.
///
/// @param log_writer Log writer.
void SetLogWriter(LogWriter* log_writer);

}  // namespace barely::logging

// Logging macros (internal).
#define BARELY_LOG(severity)                                          \
  ::barely::logging::Logger(::barely::logging::LogSeverity::severity, \
                            __FILE__, __LINE__)                       \
      .GetStream()
#define BARELY_NULL_LOG ::barely::logging::GetNullStream()

#endif  // BARELYMUSICIAN_COMMON_LOGGING_H_
