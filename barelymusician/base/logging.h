#ifndef BARELYMUSICIAN_BASE_LOGGING_H_
#define BARELYMUSICIAN_BASE_LOGGING_H_

#include <ostream>
#include <sstream>

// Logging macros.
#define LOG(severity) BARELYAPI_LOG(severity)
#define CHECK(expression) BARELYAPI_CHECK(expression)
#define CHECK_EQ(lhs, rhs) BARELYAPI_CHECK_OP(==, lhs, rhs)
#define CHECK_GE(lhs, rhs) BARELYAPI_CHECK_OP(>=, lhs, rhs)
#define CHECK_GT(lhs, rhs) BARELYAPI_CHECK_OP(>, lhs, rhs)
#define CHECK_LE(lhs, rhs) BARELYAPI_CHECK_OP(<=, lhs, rhs)
#define CHECK_LT(lhs, rhs) BARELYAPI_CHECK_OP(<, lhs, rhs)
#define CHECK_NE(lhs, rhs) BARELYAPI_CHECK_OP(!=, lhs, rhs)
#define DLOG(severity) BARELYAPI_DLOG(severity)
#define DCHECK(expression) BARELYAPI_DCHECK(expression)
#define DCHECK_EQ(lhs, rhs) BARELYAPI_DCHECK_OP(==, lhs, rhs)
#define DCHECK_GE(lhs, rhs) BARELYAPI_DCHECK_OP(>=, lhs, rhs)
#define DCHECK_GT(lhs, rhs) BARELYAPI_DCHECK_OP(>, lhs, rhs)
#define DCHECK_LE(lhs, rhs) BARELYAPI_DCHECK_OP(<=, lhs, rhs)
#define DCHECK_LT(lhs, rhs) BARELYAPI_DCHECK_OP(<, lhs, rhs)
#define DCHECK_NE(lhs, rhs) BARELYAPI_DCHECK_OP(!=, lhs, rhs)

namespace barelyapi {
namespace logging {

// Log severity.
enum class LogSeverity {
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  FATAL = 3,
};

// Logging class that wraps assertion and log stream for debugging.
class Logger {
 public:
  // Constructs new |Logger|.
  //
  // @param severity Log severity.
  // @param file File path.
  // @param line Line number.
  Logger(LogSeverity severity, const char* file, int line);

  // Destroys |Logger| after writing the log stream.
  ~Logger();

  // Returns the log stream.
  //
  // @return Log stream.
  std::ostream& GetStream();

  // static void SetWriter(LogWriter* writer);

 private:
  // Log severity.
  LogSeverity severity_;

  // Logging stream.
  std::ostringstream stream_;
};

// Log writer interface.
class LogWriter {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  ~LogWriter() = default;

  // Writes log message.
  virtual void Write(LogSeverity severity, const std::string& message) = 0;
};

// Log writer that outputs to cerr.
class CerrLogWriter : public LogWriter {
 public:
  // Implements |LogWriter|.
  void Write(LogSeverity severity, const std::string& message) override;
};

// Returns default log writer.
//
// @return Default log writer.
LogWriter& GetDefaultLogWriter();

// Returns null stream.
//
// @return Null stream.
std::ostream& GetNullStream();

// Sets log writer.
//
// @param log_writer Log writer.
void SetLogWriter(LogWriter* log_writer);

}  // namespace logging
}  // namespace barelyapi

// Logging macros (internal).
#define BARELYAPI_LOG(severity)                                               \
  ::barelyapi::logging::Logger(::barelyapi::logging::LogSeverity::##severity, \
                               __FILE__, __LINE__)                            \
      .GetStream()
#define BARELYAPI_NULL_LOG ::barelyapi::logging::GetNullStream()

#define BARELYAPI_CHECK(expression)                                       \
  (!(expression)                                                          \
       ? BARELYAPI_LOG(FATAL) << "Check failed: '" << #expression << "' " \
       : BARELYAPI_NULL_LOG)

#define BARELYAPI_CHECK_OP(op, lhs, rhs) \
  BARELYAPI_CHECK(lhs op rhs) << "(" << lhs << " " << #op << " " << rhs << ") "

#if !defined(NDEBUG) || defined(_DEBUG)
#define BARELYAPI_DLOG(severity) BARELYAPI_LOG(severity)
#define BARELYAPI_DCHECK(expression) BARELYAPI_CHECK(expression)
#define BARELYAPI_DCHECK_OP(op, lhs, rhs) BARELYAPI_CHECK_OP(op, lhs, rhs)
#else  // !defined(NDEBUG) || defined(_DEBUG)
#define BARELYAPI_DLOG(severity) BARELYAPI_NULL_LOG
#define BARELYAPI_DCHECK(expression) BARELYAPI_NULL_LOG
#define BARELYAPI_DCHECK_OP(op, lhs, rhs) BARELYAPI_NULL_LOG
#endif  // !defined(NDEBUG) || defined(_DEBUG)

#endif  // BARELYMUSICIAN_BASE_LOGGING_H_
