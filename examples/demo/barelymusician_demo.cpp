#include <conio.h>
#include <cctype>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/sequencer.h"
#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

#include "instruments/basic_synth_instrument.h"

//#include "barelymusician/dsp/dsp_utils.h"
//#include "barelymusician/dsp/one_pole_filter.h"

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::OscillatorType;
using barelyapi::Sequencer;

// using barelyapi::FilterType;
// using barelyapi::OnePoleFilter;

using barelyapi::examples::BasicSynthInstrument;
using barelyapi::examples::PaWrapper;
using barelyapi::examples::WinConsoleInput;

namespace {

// System audio settings.
constexpr int kSampleRate = 48000;
constexpr int kNumChannels = 2;
constexpr int kFramesPerBuffer = 512;

constexpr float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sequencer settings.
constexpr float kBpm = 120.0f;
constexpr int kBeatsPerBar = 4;
constexpr Sequencer::NoteValue kBeatLength = Sequencer::NoteValue::kQuarterNote;

// Metronome settings.
constexpr float kBarFrequency = 440.0f;
constexpr float kBeatFrequency = 220.0f;
constexpr OscillatorType kOscillatorType = OscillatorType::kSquare;
constexpr float kRelease = 0.025f;
constexpr float kMetronomeGain = 0.125f;

// constexpr FilterType kFilterType = FilterType::kLowPass;
// constexpr float kFilterCutoff = 500.0f;

constexpr int kNumVoices = 8;

}  // namespace

int main(int argc, char* argv[]) {
  WinConsoleInput input_manager;
  PaWrapper audio_io;

  Sequencer sequencer(kSampleRate);

  sequencer.SetBpm(kBpm);
  sequencer.SetTimeSignature(kBeatsPerBar, kBeatLength);

  Oscillator oscillator(kSampleInterval);
  Envelope envelope(kSampleInterval);

  oscillator.SetType(kOscillatorType);
  oscillator.SetFrequency(kBarFrequency);
  envelope.SetRelease(kRelease);

  // OnePoleFilter filter;
  // filter.SetType(kFilterType);
  // filter.SetCoefficient(
  //    barelyapi::GetFilterCoefficient(kSampleRate, kFilterCutoff));

  BasicSynthInstrument basic_synth_instrument(kSampleInterval, kNumVoices);

  DCHECK(basic_synth_instrument.SetFloatParam(
      static_cast<int>(BasicSynthInstrument::InstrumentFloatParam::kGain),
      1.0f / static_cast<float>(kNumVoices)));

  const auto process = [&sequencer, &oscillator, &envelope,
                        &basic_synth_instrument](float* output) {
    const int current_bar = sequencer.current_bar();
    const int current_beat = sequencer.current_beat();
    int impulse_sample = -1;
    if (current_bar == 0 && current_beat == 0 &&
        sequencer.sample_offset() == 0) {
      // First tick.
      impulse_sample = 0;
    }
    sequencer.Update(kFramesPerBuffer);
    if (current_bar != sequencer.current_bar()) {
      oscillator.SetFrequency(kBarFrequency);
      impulse_sample = sequencer.sample_offset();
    } else if (current_beat != sequencer.current_beat()) {
      oscillator.SetFrequency(kBeatFrequency);
      impulse_sample = sequencer.sample_offset();
    }
    for (int frame = 0; frame < kFramesPerBuffer; ++frame) {
      if (frame == impulse_sample) {
        oscillator.Reset();
        envelope.Start();
      }

      const float sample = kMetronomeGain * envelope.Next() * oscillator.Next();
      if (frame == impulse_sample) {
        envelope.Stop();
      }

      const auto instrument_output = basic_synth_instrument.Next();
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
        // Add |basic_synth_instrument| output.
        output[kNumChannels * frame + channel] += instrument_output;
      }
    }
  };
  audio_io.SetAudioProcessCallback(process);

  bool quit = false;
  const auto on_key_down = [&quit, &sequencer, &basic_synth_instrument](
                               const WinConsoleInput::Key& key) {
    if (static_cast<int>(key) == 27) {
      // ESC pressed, quit the app.
      quit = true;
      return;
    }

    LOG(INFO) << key << " pressed.";
    switch (std::toupper(key)) {
        // Instrument tests.
      case 'A':
        basic_synth_instrument.NoteOn(69.0f, 1.0f);
        break;
      case 'S':
        basic_synth_instrument.NoteOn(73.0f, 1.0f);
        break;
      case 'D':
        basic_synth_instrument.NoteOn(76.0f, 1.0f);
        break;
      case 'F':
        basic_synth_instrument.NoteOn(81.0f, 1.0f);
        break;
      // Sequencer tests.
      case 'T':
        sequencer.SetBpm(2.0f * kBpm);
        break;
      case 'R':
        sequencer.SetBpm(kBpm);
        break;
    }
  };
  input_manager.SetOnKeyDownCallback(on_key_down);

  const auto on_key_up =
      [&basic_synth_instrument](const WinConsoleInput::Key& key) {
        LOG(INFO) << key << " released.";
        switch (std::toupper(key)) {
            // Instrument tests.
          case 'A':
            basic_synth_instrument.NoteOff(69.0f);
            break;
          case 'S':
            basic_synth_instrument.NoteOff(73.0f);
            break;
          case 'D':
            basic_synth_instrument.NoteOff(76.0f);
            break;
          case 'F':
            basic_synth_instrument.NoteOff(81.0f);
            break;
        }
      };
  input_manager.SetOnKeyUpCallback(on_key_up);

  // Start the demo.
  input_manager.Initialize();
  audio_io.Initialize(kSampleRate, kNumChannels, kFramesPerBuffer);

  while (!quit) {
    input_manager.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  audio_io.Shutdown();
  input_manager.Shutdown();

  return 0;
}
