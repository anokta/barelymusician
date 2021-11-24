#include "platforms/unity/native/unity.h"

#include <cstdint>
#include <memory>

#include "barelymusician/common/logging.h"
#include "platforms/capi/barelymusician.h"
#include "platforms/unity/native/unity_log_writer.h"

namespace {

// Unity log writer.
std::unique_ptr<barelyapi::unity::UnityLogWriter> writer;

}  // namespace

BarelyId BarelyAddSynthInstrument(BarelyHandle handle) {
  return BarelyAddInstrument(handle, kBarelySynthInstrument);
}

BarelyHandle BarelyCreateUnity(std::int32_t sample_rate,
                               DebugCallback debug_callback_ptr) {
  if (debug_callback_ptr) {
    const auto debug_callback = [debug_callback_ptr](int severity,
                                                     const char* message) {
      debug_callback_ptr(severity, message);
    };
    writer = std::make_unique<barelyapi::unity::UnityLogWriter>();
    writer->SetDebugCallback(debug_callback);
    barelyapi::logging::SetLogWriter(writer.get());
  }
  return BarelyCreate(sample_rate);
}

BarelyStatus BarelyDestroyUnity(BarelyHandle handle) {
  const BarelyStatus status = BarelyDestroy(handle);
  if (status == kBarelyOk) {
    barelyapi::logging::SetLogWriter(nullptr);
    writer.reset();
  }
  return status;
}
