#include <conio.h>

#include <cctype>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/dsp/envelope.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/sequencer/sequencer.h"
#include "instruments/basic_synth_instrument.h"
#include "util/audio_io/pa_wrapper.h"
#include "util/input_manager/win_console_input.h"

//#include "barelymusician/dsp/dsp_utils.h"
//#include "barelymusician/dsp/one_pole_filter.h"

using barelyapi::Envelope;
using barelyapi::Oscillator;
using barelyapi::OscillatorType;
using barelyapi::Sequencer;

// using barelyapi::FilterType;
// using barelyapi::OnePoleFilter;

using barelyapi::examples::BasicSynthInstrument;
using barelyapi::examples::BasicSynthInstrumentFloatParam;
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
const int kNumBeats = 4;

// Metronome settings.
const float kBarFrequency = 440.0f;
const float kBeatFrequency = 220.0f;
const OscillatorType kOscillatorType = OscillatorType::kSquare;
const float kRelease = 0.025f;
const float kMetronomeGain = 0.125f;

// const FilterType kFilterType = FilterType::kLowPass;
// const float kFilterCutoff = 500.0f;

const int kNumVoices = 8;

}  // namespace

int main(int argc, char* argv[]) {
  WinConsoleInput input_manager;
  PaWrapper audio_io;

  Sequencer sequencer(kSampleRate);

  sequencer.SetTempo(kTempo);
  sequencer.SetNumBeats(kNumBeats);

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

  const auto process = [&sequencer, &oscillator, &envelope,
                        &basic_synth_instrument](float* output) {
    const auto previous_transport = sequencer.GetTransport();
    int impulse_sample = -1;
    if (previous_transport.offset_beats == 0) {
      impulse_sample = 0;
    }
    sequencer.Update(kFramesPerBuffer);
    const auto current_transport = sequencer.GetTransport();
    const float num_samples_per_beat =
        (current_transport.tempo > 0.0f)
            ? barelyapi::kSecondsFromMinutes * static_cast<float>(kSampleRate) /
                  current_transport.tempo
            : 0.0f;
    if (current_transport.bar != previous_transport.bar) {
      oscillator.SetFrequency(kBarFrequency);
      impulse_sample =
          kFramesPerBuffer - static_cast<int>(current_transport.offset_beats *
                                              num_samples_per_beat);
    } else if (current_transport.beat != previous_transport.beat) {
      oscillator.SetFrequency(kBeatFrequency);
      impulse_sample =
          kFramesPerBuffer - static_cast<int>(current_transport.offset_beats *
                                              num_samples_per_beat);
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

      const float instrument_output = basic_synth_instrument.Next();
      for (int channel = 0; channel < kNumChannels; ++channel) {
        output[kNumChannels * frame + channel] = sample;
        // Add |basic_synth_instrument| output.
        output[kNumChannels * frame + channel] += instrument_output;
      }
    }
  };
  audio_io.SetAudioProcessCallback(process);

  bool quit = false;
  const auto key_down_callback = [&quit, &sequencer, &basic_synth_instrument](
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
      case 'O':
        basic_synth_instrument.SetFloatParam(
            BasicSynthInstrumentFloatParam::kOscillatorType,
            static_cast<float>(OscillatorType::kSquare));
        break;
      case 'R':
        basic_synth_instrument.Reset();
        break;
      // Sequencer tests.
      case '1':
        sequencer.SetTempo(kTempo);
        break;
      case '2':
        sequencer.SetTempo(2.0f * kTempo);
        break;
    }
  };
  input_manager.RegisterKeyDownCallback(key_down_callback);

  const auto key_up_callback =
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
  input_manager.RegisterKeyUpCallback(key_up_callback);

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
