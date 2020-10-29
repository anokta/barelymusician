#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/util/task_runner.h"
#include "examples/audio_output/pa_audio_output.h"
#include "examples/input_manager/win_console_input.h"
#include "examples/instruments/basic_synth_instrument.h"

namespace {

using ::barelyapi::OscillatorType;
using ::barelyapi::TaskRunner;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaAudioOutput;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const int kNumMaxTasks = 100;

// Instrument settings.
const float kGain = 0.125f;
const int kNumVoices = 16;
const OscillatorType kOscillatorType = OscillatorType::kSaw;
const float kEnvelopeAttack = 0.05f;
const float kEnvelopeRelease = 0.125f;

// Note settings.
const float kRootNoteIndex = barelyapi::kNoteIndexC3;
const float kNoteIntensity = 1.0f;
const char kOctaveKeys[] = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                            'G', 'Y', 'H', 'U', 'J', 'K'};
const float kMaxOffsetOctaves = 3.0f;

// Returns the note index for the given |key| and |offset_octaves|.
float NoteIndexFromKey(const WinConsoleInput::Key& key, float offset_octaves) {
  const auto it = std::find(std::cbegin(kOctaveKeys), std::cend(kOctaveKeys),
                            std::toupper(key));
  if (it == std::cend(kOctaveKeys)) {
    return -1.0f;
  }
  const float distance =
      static_cast<float>(std::distance(std::cbegin(kOctaveKeys), it));
  return kRootNoteIndex + barelyapi::kNumSemitones * offset_octaves + distance;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaAudioOutput audio_output;
  WinConsoleInput input_manager;

  TaskRunner task_runner(kNumMaxTasks);

  BasicSynthInstrument instrument(kSampleRate);
  instrument.Control(BasicSynthInstrumentParam::kNumVoices,
                     static_cast<float>(kNumVoices));
  instrument.Control(BasicSynthInstrumentParam::kGain, kGain);
  instrument.Control(BasicSynthInstrumentParam::kOscillatorType,
                     static_cast<float>(kOscillatorType));
  instrument.Control(BasicSynthInstrumentParam::kEnvelopeAttack,
                     kEnvelopeAttack);
  instrument.Control(BasicSynthInstrumentParam::kEnvelopeRelease,
                     kEnvelopeRelease);

  float offset_octaves = 0.0f;

  // Audio process callback.
  const auto process_callback = [&](float* output) {
    task_runner.Run();
    instrument.Process(output, kNumChannels, kNumFrames);
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

    // Shift octaves.
    const auto upper_key = std::toupper(key);
    if (upper_key == 'Z' || upper_key == 'X') {
      // Stop current notes first.
      task_runner.Add([&, offset_octaves]() {
        for (const char key : kOctaveKeys) {
          instrument.NoteOff(NoteIndexFromKey(key, offset_octaves));
        }
      });
      // Update offset.
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves =
          std::clamp(offset_octaves, -kMaxOffsetOctaves, kMaxOffsetOctaves);
      LOG(INFO) << "Octave offset set to " << offset_octaves;
      return;
    }

    // Play note.
    const float note_index = NoteIndexFromKey(key, offset_octaves);
    if (note_index < 0.0f) {
      return;
    }
    task_runner.Add(
        [&, note_index]() { instrument.NoteOn(note_index, kNoteIntensity); });
    LOG(INFO) << "NoteOn(" << note_index << ", " << kNoteIntensity << ")";
  };
  input_manager.SetKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback = [&](const WinConsoleInput::Key& key) {
    // Stop note.
    const float note_index = NoteIndexFromKey(key, offset_octaves);
    if (note_index < 0.0f) {
      return;
    }
    task_runner.Add([&, note_index]() { instrument.NoteOff(note_index); });
    LOG(INFO) << "NoteOff(" << note_index << ")";
  };
  input_manager.SetKeyUpCallback(key_up_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_output.Start(kSampleRate, kNumChannels, kNumFrames);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Stop the demo.
  LOG(INFO) << "Stopping audio stream";

  audio_output.Stop();
  input_manager.Shutdown();

  return 0;
}
