
#include "examples/common/console_log.h"
#include "platforms/api/barelymusician.h"

namespace {

using ::barely::Api;
using ::barely::Instrument;
using ::barely::InstrumentDefinition;
using ::barely::IsOk;
using ::barely::NoteDefinition;
using ::barely::ParamDefinition;
using ::barely::Sequence;
using ::barely::Status;
using ::barely::ToString;
using ::barely::Transport;
using ::barely::examples::ConsoleLog;

constexpr int kSampleRate = 48000;
constexpr double kTempo = 120.0;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  Api api(kSampleRate);
  // ConsoleLog() << "Sample rate: " << api.GetSampleRate();

  auto& transport = api.GetTransport();
  if (const auto status = transport.SetTempo(kTempo); !IsOk(status)) {
    ConsoleLog() << "Failed to set tempo: " << ToString(status);
    return -1;
  }
  ConsoleLog() << "Tempo: " << transport.GetTempo();

  Instrument instrument1 = api.CreateInstrument(
      InstrumentDefinition{.param_definitions = {ParamDefinition(5)}});
  // ConsoleLog() << "Instrument 1 gain: " << instrument1.GetGain();

  // Instrument instrument2 = instrument1;
  // ConsoleLog() << "Instrument 2 gain: " << instrument2.GetGain();

  Sequence sequence = api.CreateSequence(&instrument1);
  if (const auto status = sequence.SetBeginOffset(3.25); !IsOk(status)) {
    ConsoleLog() << "Failed to set begin offset: " << ToString(status);
    return -1;
  }
  // ConsoleLog() << "Sequence begin offset: " << sequence.GetBeginOffset();

  const auto note = sequence.AddNote(2.0, NoteDefinition(1.0, 0.0f));
  assert(IsOk(sequence.RemoveNote(note)));

  return 0;
}
