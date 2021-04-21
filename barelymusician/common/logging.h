#ifndef BARELYMUSICIAN_COMMON_LOGGING_H_
#define BARELYMUSICIAN_COMMON_LOGGING_H_

#include <ostream>
#include <sstream>
#include <string>

/// Logging macros.
#define LOG(severity) BARELY_LOG(severity)
#define CHECK(expression) BARELY_CHECK(expression)
#define CHECK_EQ(lhs, rhs) BARELY_CHECK_OP(==, lhs, rhs)
#define CHECK_GE(lhs, rhs) BARELY_CHECK_OP(>=, lhs, rhs)
#define CHECK_GT(lhs, rhs) BARELY_CHECK_OP(>, lhs, rhs)
#define CHECK_LE(lhs, rhs) BARELY_CHECK_OP(<=, lhs, rhs)
#define CHECK_LT(lhs, rhs) BARELY_CHECK_OP(<, lhs, rhs)
#define CHECK_NE(lhs, rhs) BARELY_CHECK_OP(!=, lhs, rhs)
#define DLOG(severity) BARELY_DLOG(severity)
#define DCHECK(expression) BARELY_DCHECK(expression)
#define DCHECK_EQ(lhs, rhs) BARELY_DCHECK_OP(==, lhs, rhs)
#define DCHECK_GE(lhs, rhs) BARELY_DCHECK_OP(>=, lhs, rhs)
#define DCHECK_GT(lhs, rhs) BARELY_DCHECK_OP(>, lhs, rhs)
#define DCHECK_LE(lhs, rhs) BARELY_DCHECK_OP(<=, lhs, rhs)
#define DCHECK_LT(lhs, rhs) BARELY_DCHECK_OP(<, lhs, rhs)
#define DCHECK_NE(lhs, rhs) BARELY_DCHECK_OP(!=, lhs, rhs)

namespace barelyapi::logging {

/// Log severity.
enum class LogSeverity {
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  FATAL = 3,
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

}  // namespace barelyapi::logging

// Logging macros (internal).
#define BARELY_LOG(severity)                                                \
  ::barelyapi::logging::Logger(::barelyapi::logging::LogSeverity::severity, \
                               __FILE__, __LINE__)                          \
      .GetStream()
#define BARELY_NULL_LOG ::barelyapi::logging::GetNullStream()

#define BARELY_CHECK(expression)                                       \
  (!(expression)                                                       \
       ? BARELY_LOG(FATAL) << "Check failed: '" << #expression << "' " \
       : BARELY_NULL_LOG)

#define BARELY_CHECK_OP(op, lhs, rhs) \
  BARELY_CHECK(lhs op rhs) << "(" << lhs << " " << #op << " " << rhs << ") "

#if !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)
#define BARELY_DLOG(severity) BARELY_LOG(severity)
#define BARELY_DCHECK(expression) BARELY_CHECK(expression)
#define BARELY_DCHECK_OP(op, lhs, rhs) BARELY_CHECK_OP(op, lhs, rhs)
#else  // !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)
#define BARELY_DLOG(severity) BARELY_NULL_LOG
#define BARELY_DCHECK(expression) BARELY_NULL_LOG
#define BARELY_DCHECK_OP(op, lhs, rhs) BARELY_NULL_LOG
#endif  // !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)

#endif  // BARELYMUSICIAN_COMMON_LOGGING_H_
