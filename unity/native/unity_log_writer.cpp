#include "unity/native/unity_log_writer.h"

#include <string>
#include <utility>

#include "barelymusician/common/logging.h"

namespace barelyapi::unity {

void UnityLogWriter::Write(logging::LogSeverity severity,
                           const std::string& message) {
  if (debug_callback_) {
    debug_callback_(static_cast<int>(severity), message.c_str());
  }
}

void UnityLogWriter::SetDebugCallback(DebugCallback&& debug_callback) {
  debug_callback_ = std::move(debug_callback);
}

}  // namespace barelyapi::unity
