#include <chrono>
#include <iomanip>
#include <memory>
#include <thread>

#include "audio_output/pa_audio_output.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/base/transport.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::Sequencer;
using ::barelyapi::Transport;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
const float kTempo = 120.0f;
const int kNumBars = 4;
const int kNumBeats = 4;

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  Sequencer sequencer(kSampleRate);
  sequencer.SetTempo(kTempo);
  sequencer.SetNumBars(kNumBars);
  sequencer.SetNumBeats(kNumBeats);

  // Beat callback.
  const auto beat_callback = [](const Transport& transport, int start_sample,
                                int num_samples_per_beat) {
    LOG(INFO) << "Transport position " << transport.section << "."
              << transport.bar << "." << transport.beat << " (" << start_sample
              << ", " << num_samples_per_beat << ")";
  };
  sequencer.RegisterBeatCallback(beat_callback);

  // Audio process callback.
  const auto process_callback = [&sequencer](float* output) {
    sequencer.Update(kNumFrames);
    std::fill_n(output, kNumChannels * kNumFrames, 0.0f);
  };
  audio_output.SetProcessCallback(process_callback);

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
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
