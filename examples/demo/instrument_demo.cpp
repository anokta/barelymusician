#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <thread>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "instruments/basic_synth_instrument.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

namespace {

using ::barelyapi::OscillatorType;
using ::barelyapi::examples::BasicSynthInstrument;
using ::barelyapi::examples::BasicSynthInstrumentParam;
using ::barelyapi::examples::PaWrapper;
using ::barelyapi::examples::WinConsoleInput;

// System audio settings.
const int kSampleRate = 48000;
const int kNumChannels = 2;
const int kNumFrames = 512;

const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Instrument settings.
const float kGain = 0.125f;
const int kNumVoices = 16;
const OscillatorType kOscillatorType = OscillatorType::kSaw;
const float kEnvelopeRelease = 0.25f;

// Note settings.
const float kRootNoteIndex = 69.0f;
const float kNoteIntensity = 1.0f;
const char kOctaveKeys[] = {'A', 'W', 'S', 'E', 'D', 'F', 'T',
                            'G', 'Y', 'H', 'U', 'J', 'K'};
const float kMaxOffsetOctaves = 3.0f;

// Returns the note index for the given |key| and |offset_octaves|.
float NoteIndexFromKey(const WinConsoleInput::Key& key, float offset_octaves) {
  const auto it = std::find(std::begin(kOctaveKeys), std::end(kOctaveKeys),
                            std::toupper(key));
  if (it == std::end(kOctaveKeys)) {
    return -1.0f;
  }
  const float distance =
      static_cast<float>(std::distance(std::begin(kOctaveKeys), it));
  return kRootNoteIndex + barelyapi::kNumSemitones * offset_octaves + distance;
}

}  // namespace

int main(int argc, char* argv[]) {
  PaWrapper audio_io;
  WinConsoleInput input_manager;

  BasicSynthInstrument instrument(kSampleInterval, kNumVoices);
  instrument.SetFloatParam(BasicSynthInstrumentParam::kGain, kGain);
  instrument.SetFloatParam(BasicSynthInstrumentParam::kOscillatorType,
                           static_cast<float>(kOscillatorType));
  instrument.SetFloatParam(BasicSynthInstrumentParam::kEnvelopeRelease,
                           kEnvelopeRelease);

  float offset_octaves = 0.0f;

  // Audio process callback.
  const auto audio_process_callback = [&instrument](float* output) {
    instrument.Process(output, kNumChannels, kNumFrames);
  };
  audio_io.SetAudioProcessCallback(audio_process_callback);

  // Key down callback.
  bool quit = false;
  const auto key_down_callback = [&instrument, &offset_octaves,
                                  &quit](const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    // Shift octaves.
    const auto upper_key = std::toupper(key);
    if (upper_key == 'Z' || upper_key == 'X') {
      // Clear current notes first.
      instrument.Reset();
      // Update offset.
      if (upper_key == 'Z') {
        --offset_octaves;
      } else {
        ++offset_octaves;
      }
      offset_octaves = std::min(std::max(offset_octaves, -kMaxOffsetOctaves),
                                kMaxOffsetOctaves);
      LOG(INFO) << "Octave offset set to " << offset_octaves;
      return;
    }

    // Play note.
    const float note_index = NoteIndexFromKey(key, offset_octaves);
    if (note_index < 0.0f) {
      return;
    }
    instrument.NoteOn(note_index, kNoteIntensity);
  };
  input_manager.RegisterKeyDownCallback(key_down_callback);

  // Key up callback.
  const auto key_up_callback =
      [&instrument, &offset_octaves](const WinConsoleInput::Key& key) {
        // Stop note.
        const float note_index = NoteIndexFromKey(key, offset_octaves);
        if (note_index < 0.0f) {
          return;
        }
        instrument.NoteOff(note_index);
      };
  input_manager.RegisterKeyUpCallback(key_up_callback);

  // Start the demo.
  LOG(INFO) << "Starting audio stream";

  input_manager.Initialize();
  audio_io.Initialize(kSampleRate, kNumChannels, kNumFrames);

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
