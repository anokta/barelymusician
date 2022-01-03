
#include "examples/common/console_log.h"
#include "platforms/api/barelymusician.h"

namespace {

using ::barely::Api;
using ::barely::Instrument;
using ::barely::InstrumentDefinition;
using ::barely::ParamDefinition;
using ::barely::ParamId;
using ::barely::Sequence;
using ::barely::Status;
using ::barely::Transport;
using ::barely::examples::ConsoleLog;

constexpr int kSampleRate = 48000;
constexpr double kTempo = 120.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  Api api(kSampleRate);

  const auto sample_rate_or = api.GetSampleRate();
  assert(sample_rate_or.IsOk());
  ConsoleLog() << "Sample Rate: " << sample_rate_or.GetValue();

  Transport transport(api);
  assert(transport.SetTempo(kTempo) == Status::kOk);

  const auto tempo_or = transport.GetTempo();
  assert(tempo_or.IsOk());
  ConsoleLog() << "Tempo: " << tempo_or.GetValue();

  Instrument instrument(
      api, InstrumentDefinition{
               .param_definitions = {ParamDefinition(ParamId{1}, 5)}});

  const auto gain_or = instrument.GetGain();
  assert(gain_or.IsOk());
  ConsoleLog() << "Instrument Gain: " << gain_or.GetValue();

  Sequence sequence(api);
  assert(sequence.SetBeginOffset(3.25) == Status::kOk);

  const auto begin_offset_or = sequence.GetBeginOffset();
  assert(begin_offset_or.IsOk());
  ConsoleLog() << "Sequence Begin Offset: " << begin_offset_or.GetValue();

  return 0;
}
