#ifndef EXAMPLES_COMMON_CONSOLE_LOG_H_
#define EXAMPLES_COMMON_CONSOLE_LOG_H_

#include <iostream>
#include <ostream>
#include <sstream>

namespace barely::examples {

/// Simple helper class that logs to the console output with a newline.
class ConsoleLog {
 public:
  /// Default constructor.
  ConsoleLog() = default;

  /// Destroys `ConsoleLog`.
  ~ConsoleLog() noexcept { std::cout << stream_.str() << std::endl; }

  /// Non-copyable and non-movable.
  ConsoleLog(const ConsoleLog& other) noexcept = delete;
  ConsoleLog& operator=(const ConsoleLog& other) noexcept = delete;
  ConsoleLog(ConsoleLog&& other) noexcept = delete;
  ConsoleLog& operator=(ConsoleLog&& other) noexcept = delete;

  /// Overloads output stream operator.
  ///
  /// @param value Value to output.
  /// @return Output stream.
  template <typename Type>
  std::ostream& operator<<(const Type& value) noexcept {
    stream_ << value;
    return stream_;
  }

 private:
  // Logging stream.
  std::ostringstream stream_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_CONSOLE_LOG_H_
