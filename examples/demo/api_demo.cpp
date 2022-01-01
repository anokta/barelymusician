
#include "examples/common/console_log.h"
#include "platforms/api/barelymusician.h"

namespace {

using ::barely::Api;
using ::barely::examples::ConsoleLog;

constexpr int kSampleRate = 48000;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  Api api(kSampleRate);

  const auto sample_rate_or = api.GetSampleRate();
  assert(sample_rate_or.IsOk());
  ConsoleLog() << "Sample Rate: " << sample_rate_or.GetValue();

  return 0;
}
