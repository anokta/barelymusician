#ifndef UNITY_NATIVE_UNITY_LOG_WRITER_H_
#define UNITY_NATIVE_UNITY_LOG_WRITER_H_

#include <functional>

#include "barelymusician/common/logging.h"

namespace barelyapi {
namespace unity {

/// Unity log writer.
class UnityLogWriter : public logging::LogWriter {
 public:
  /// Debug callback signature.
  using DebugCallback = std::function<void(int severity, const char* message)>;

  /// Implements |LogWriter|.
  void Write(logging::LogSeverity severity,
             const std::string& message) override;

  /// Sets debug callback.
  ///
  /// @param debug_callback Debug callback.
  void SetDebugCallback(DebugCallback&& debug_callback);

 private:
  // Debug callback.
  DebugCallback debug_callback_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // UNITY_NATIVE_UNITY_LOG_WRITER_H_
