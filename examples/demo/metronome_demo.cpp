#include <cctype>
#include <chrono>
#include <cmath>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/engine/engine.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/util/task_runner.h"
#include "instruments/basic_synth_instrument.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Engine;
using ::barelyapi::OscillatorType;
using ::barelyapi::TaskRunner;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 2048;

const int kNumMaxTasks = 100;

// Metronome settings.
const int kMetronomeId = 1;

const int kNumVoices = 1;
const float kGain = 0.5f;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kAttack = 0.0f;
const float kRelease = 0.025f;

const double kTickDuration = 0.005f;
const float kBarNoteIndex = barelyapi::kNoteIndexA4;
const float kBeatNoteIndex = barelyapi::kNoteIndexA3;

const int kNumBeats = 4;
const double kInitialTempo = 120.0;
const double kTempoIncrement = 10.0;

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  Engine engine(kSampleRate);
  engine.SetTempo(kInitialTempo);

  auto metronome_instrument =
      std::make_unique<BasicSynthInstrument>(kSampleRate, kNumVoices);
  metronome_instrument->SetParam(
      BasicSynthInstrumentParam::kOscillatorType,
      static_cast<float>(kOscillatorType));
  metronome_instrument->SetParam(
      BasicSynthInstrumentParam::kEnvelopeAttack, kAttack);
  metronome_instrument->SetParam(
      BasicSynthInstrumentParam::kEnvelopeRelease, kRelease);
  metronome_instrument->SetParam(BasicSynthInstrumentParam::kGain, kGain);
  engine.Create(kMetronomeId, std::move(metronome_instrument));

  // Beat callback.
  const auto beat_callback = [&](int beat) {
    const int current_bar = beat / kNumBeats;
    const int current_beat = beat % kNumBeats;
    LOG(INFO) << "Tick " << current_bar << "." << current_beat;
    const double position = static_cast<double>(beat);
    const float index = (current_beat == 0) ? kBarNoteIndex : kBeatNoteIndex;
    engine.ScheduleNote(kMetronomeId, position, kTickDuration, index, kGain);
  };
  engine.SetBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    task_runner.Run();
    engine.Update(kNumFrames);
    engine.Process(kMetronomeId, output, kNumChannels, kNumFrames);
  };
  audio_output.SetProcessCallback(process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }
    // Adjust tempo.
    double tempo = engine.GetTempo();
    switch (std::toupper(key)) {
      case ' ':
        task_runner.Add([&]() {
          if (engine.IsPlaying()) {
            engine.Stop();
            LOG(INFO) << "Stopped playback";
          } else {
            engine.Start();
            LOG(INFO) << "Started playback";
          }
        });
        return;
      case '-':
        tempo -= kTempoIncrement;
        break;
      case '+':
        tempo += kTempoIncrement;
        break;
      case '1':
        tempo *= 0.5;
        break;
      case '2':
        tempo *= 2.0;
        break;
      case 'R':
        tempo = kInitialTempo;
        break;
      default:
        return;
    }
    task_runner.Add([&, tempo]() { engine.SetTempo(tempo); });
    LOG(INFO) << "Tempo set to " << tempo;
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  engine.Start();

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  engine.Stop();

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
