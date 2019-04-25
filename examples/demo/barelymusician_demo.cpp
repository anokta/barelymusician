#include <chrono>
#include <iomanip>
#include <thread>

#include "barelymusician/base/logging.h"
#include "barelymusician/sequencer/sequencer.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

using barelyapi::Sequencer;
using barelyapi::Transport;
using barelyapi::examples::PaWrapper;
using barelyapi::examples::WinConsoleInput;

namespace {

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kFramesPerBuffer = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 120.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

}  // namespace

int main(int argc, char* argv[]) {
  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  PaWrapper audio_io;
  WinConsoleInput input_manager;

  // Beat callback.
  const auto beat_callback = [](const Transport& transport, int start_sample) {
    LOG(INFO) << "Transport position " << transport.section << "."
              << transport.bar << "." << transport.beat << ":" << std::fixed
              << std::setprecision(2) << transport.offset_beats << " ("
              << start_sample << ")";
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  const auto audio_process_callback = [&sequencer](float* output) {
    sequencer.Update(kFramesPerBuffer);
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = 0.0f;
      }
    }
  };
  audio_io.SetAudioProcessCallback(audio_process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&quit](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    LOG(INFO) << "Pressed " << key;
  };
  input_manager.RegisterKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [](const WinConsoleInput::Key& key) {
    LOG(INFO) << "Released " << key;
  };
  input_manager.RegisterKeyUpCallback(key_up_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_io.Initialize(kSampleRate, kNumChannels, kFramesPerBuffer);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_io.Shutdown();
  input_manager.Shutdown();

  return 0;
}
