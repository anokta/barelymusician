#ifndef BARELYMUSICIAN_PLATFORM_UNITY_UNITY_LOG_WRITER_H_
#define BARELYMUSICIAN_PLATFORM_UNITY_UNITY_LOG_WRITER_H_

#include <functional>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace unity {

// Unity log writer.
class UnityLogWriter : public logging::LogWriter {
 public:
  // Debug callback signature.
  using DebugCallback = std::function<void(int severity, const char* message)>;

  // Implements |LogWriter|.
  void Write(logging::LogSeverity severity,
             const std::string& message) override;

  // Sets debug callback.
  //
  // @param Debug callback.
  void SetDebugCallback(DebugCallback&& debug_callback);

 private:
  // Debug callback.
  DebugCallback debug_callback_;
};

}  // namespace unity
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PLATFORM_UNITY_UNITY_LOG_WRITER_H_
