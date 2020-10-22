#include "barelymusician/platform/unity/unity_log_writer.h"

namespace barelyapi {
namespace unity {

void UnityLogWriter::Write(logging::LogSeverity severity,
                           const std::string& message) {
  if (debug_callback_) {
    debug_callback_(static_cast<int>(severity), message.c_str());
  }
}

void UnityLogWriter::SetDebugCallback(DebugCallback&& debug_callback) {
  debug_callback_ = std::move(debug_callback);
}

}  // namespace unity
}  // namespace barelyapi
