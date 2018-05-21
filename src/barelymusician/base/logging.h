#ifndef BARELYMUSICIAN_BASE_LOGGING_H_
#define BARELYMUSICIAN_BASE_LOGGING_H_

#include <iostream>

#if !defined(BARELYAPI_LOG_OUTPUT)
// Logs are sent to the terminal output by default.
#define BARELYAPI_LOG_OUTPUT std::cout
#endif  // !defined(BARELYAPI_LOG_OUTPUT)

// Logging macros.
#define LOG(severity) BARELYAPI_LOG(severity)
#define CHECK(expression) BARELYAPI_CHECK(expression)
#define CHECK_EQ(val1, val2) BARELYAPI_CHECK_OP(==, val1, val2)
#define CHECK_GE(val1, val2) BARELYAPI_CHECK_OP(>=, val1, val2)
#define CHECK_GT(val1, val2) BARELYAPI_CHECK_OP(>, val1, val2)
#define CHECK_LE(val1, val2) BARELYAPI_CHECK_OP(<=, val1, val2)
#define CHECK_LT(val1, val2) BARELYAPI_CHECK_OP(<, val1, val2)
#define CHECK_NE(val1, val2) BARELYAPI_CHECK_OP(!=, val1, val2)
#define DLOG(severity) BARELYAPI_DLOG(severity)
#define DCHECK(expression) BARELYAPI_DCHECK(expression)
#define DCHECK_EQ(val1, val2) BARELYAPI_DCHECK_OP(==, val1, val2)
#define DCHECK_GE(val1, val2) BARELYAPI_DCHECK_OP(>=, val1, val2)
#define DCHECK_GT(val1, val2) BARELYAPI_DCHECK_OP(>, val1, val2)
#define DCHECK_LE(val1, val2) BARELYAPI_DCHECK_OP(<=, val1, val2)
#define DCHECK_LT(val1, val2) BARELYAPI_DCHECK_OP(<, val1, val2)
#define DCHECK_NE(val1, val2) BARELYAPI_DCHECK_OP(!=, val1, val2)

namespace barelyapi {
namespace logging {

// Logging class that wraps assertion and output log stream for debugging.
class Logger {
 public:
  // Log severity.
  enum Severity {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    FATAL = 3,
  };

  // Constructs new |Logger| for the given output.
  //
  // @param out Output stream.
  // @param severity Log severity.
  // @param file File path.
  // @param line Line number.
  Logger(std::ostream& out, Severity severity, const char* file, int line);

  // Destroys |Logger| after logging a new line.
  ~Logger();

  // Returns the log output stream.
  //
  // @return Output stream.
  std::ostream& GetStream();

  // Returns the null output stream.
  //
  // @return Null stream.
  static std::ostream& NullStream();

 private:
  // Logging (output) stream.
  std::ostream& out_;

  // Log severity.
  Severity severity_;
};

}  // namespace logging
}  // namespace barelyapi

// Logging macros (internal).
#define BARELYAPI_NULL_LOG ::barelyapi::logging::Logger::NullStream()

#define BARELYAPI_LOG(severity)                                               \
  ::barelyapi::logging::Logger(                                               \
      BARELYAPI_LOG_OUTPUT,                                                   \
      ::barelyapi::logging::Logger::Severity::##severity, __FILE__, __LINE__) \
      .GetStream()

#define BARELYAPI_CHECK(expression)                                       \
  (!(expression)                                                          \
       ? BARELYAPI_LOG(FATAL) << "Check failed: '" << #expression << "' " \
       : BARELYAPI_NULL_LOG)

#define BARELYAPI_CHECK_OP(op, val1, val2) \
  BARELYAPI_CHECK(val1 op val2)            \
      << "(" << val1 << " " << #op << " " << val2 << ") "

#if !defined(NDEBUG) || defined(_DEBUG)
#define BARELYAPI_DLOG(severity) BARELYAPI_LOG(severity)
#define BARELYAPI_DCHECK(expression) BARELYAPI_CHECK(expression)
#define BARELYAPI_DCHECK_OP(op, val1, val2) BARELYAPI_CHECK_OP(op, val1, val2)
#else  // !defined(NDEBUG) || defined(_DEBUG)
#define BARELYAPI_DLOG(severity) BARELYAPI_NULL_LOG
#define BARELYAPI_DCHECK(expression) BARELYAPI_NULL_LOG
#define BARELYAPI_DCHECK_OP(op, val1, val2) BARELYAPI_NULL_LOG
#endif  // !defined(NDEBUG) || defined(_DEBUG)

#endif  // BARELYMUSICIAN_BASE_LOGGING_H_
