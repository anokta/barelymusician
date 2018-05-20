#ifndef BARELYMUSICIAN_BASE_LOGGING_H_
#define BARELYMUSICIAN_BASE_LOGGING_H_

#include <cassert>
#include <iostream>
#include <string>

#if defined(_DEBUG) || defined(DEBUG)
// Enable logging in debug mode.
#define ENABLE_LOGGING 1
#endif  // defined(_DEBUG) || defined(DEBUG)

#if !defined(LOG_OUTPUT)
// Output logs to terminal as default.
#define LOG_OUTPUT std::cout
#endif  // !defined(LOG_OUTPUT)

// Log output stream.
#define DLOG BARELYAPI_LOG

namespace barelyapi {

// Logging class that wraps output log stream for debugging.
// TODO(#4): Refactor this implementation to include asserts etc.
class Logger {
 public:
  // Constructs new |Logger| for the given output.
  //
  // @param out Output stream.
  // @param file File path.
  // @param line Line number.
  Logger(std::ostream& out, const char* file, int line);

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
};

}  // namespace barelyapi

// Strips file path to show file name only.
#if defined(WIN32) || defined(_WIN32) || defined(X64) || defined(_X64)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif  // defined(WIN32) || defined(_WIN32) || defined(X64) || defined(_X64)
#define __FILENAME__                                                         \
  (strrchr(__FILE__, PATH_SEPARATOR) ? strrchr(__FILE__, PATH_SEPARATOR) + 1 \
                                     : __FILE__)

// Defines the log stream provided with the line number and file name it gets
// called from.
#if defined(ENABLE_LOGGING)
#define BARELYAPI_LOG \
  ::barelyapi::Logger(LOG_OUTPUT, __FILENAME__, __LINE__).GetStream()
#else
#define BARELYAPI_LOG ::barelyapi::Logger::NullStream()
#endif  // defined(ENABLE_LOGGING)

#endif  // BARELYMUSICIAN_BASE_LOGGING_H_
