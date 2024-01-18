barelymusician
[![ci](https://github.com/anokta/barelymusician/actions/workflows/ci.yml/badge.svg)](https://github.com/anokta/barelymusician/actions/workflows/ci.yml)
[![](https://img.shields.io/static/v1?label=sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/anokta)
==============

barelymusician is a real-time music engine for interactive systems.

It provides a modern C/C++ API to generate and perform musical sounds from scratch in a sample
accurate way.

[iOS]: ## "see issue #112 for the status of the upcoming iOS platform support"
This repository includes build targets for Linux, OSX, Windows, Android, and Daisy platforms, in
addition to a native Unity game engine plugin[*][iOS]. The library targets can be built with
[Bazel](https://bazel.build/) or [CMake](https://cmake.org/). To use in a project, simply include
[barelymusician/barelymusician.h](barelymusician/barelymusician.h).

To use in Unity, download the latest version of
[barelymusician.unitypackage](https://github.com/anokta/barelymusician/releases/latest/download/barelymusician.unitypackage).

For background about this project, see the original research paper
[here](http://www.aes.org/e-lib/browse.cfm?elib=17598), and the legacy Unity implementation
[here](https://github.com/anokta/barelyMusicianLegacy).

## Example usage

```cpp
// Import the core engine.
#include "barelymusician/barelymusician.h"
// Import `barely::LowPassEffect`.
#include "barelymusician/effects/low_pass_effect.h"
// Import `barely::SynthInstrument`.
#include "barelymusician/instruments/synth_instrument.h"

// Create a new musician.
barely::Musician musician(/*frame_rate=*/48000);

// Set the global tempo to 124 beats per minute.
musician.SetTempo(/*tempo=*/124);

// Create a synth instrument.
auto instrument = musician.CreateInstrument<barely::SynthInstrument>();

// Set the synth instrument gain to 0.5.
instrument.SetControl(barely::SynthInstrument::Control::kGain, /*value=*/Rational(1, 2),
                      /*slope_per_beat=*/0);

// Set the A3 synth instrument note pitch on with a 0.25 intensity.
//
// @note Pitch values are normalized by octaves, where each integer value of one shifts an octave by
// one, and zero represents the A4 (middle A) pitch at 440 hertz in a typical instrument definition.
// However, this is not a strict rule, since `pitch` and `intensity` can be interpreted in any
// desired way by a custom instrument.
const auto d4_pitch = barely::Rational(-7, 12);
instrument.SetNoteOn(d4_pitch, /*intensity=*/barely::Rational(1, 2));

// Check if the instrument note is on.
const bool is_note_on = instrument.IsNoteOn(d4_pitch);  // will return true.

// Add a low-pass effect to the instrument.
auto effect = instrument.CreateEffect<barely::LowPassEffect>();

// Set the low-pass cutoff frequency to increase by 100 hertz per beat.
effect.SetControl(barely::LowPassEffect::Control::kCutoffFrequency, /*value=*/0,
                  /*slope_per_beat=*/100);

// Update the musician Timestamp in frames.
//
// @note Timestamp updates must happen prior to processing of instruments with respective
// timestamps. Otherwise, such `Process` calls will be *late* to receive any relevant state changes.
// To compensate, `Update` should typically be called from a main thread update callback, with an
// additional "lookahead", in order to avoid any potential thread synchronization issues that could
// occur in real-time audio applications.
const auto lookahead = 480;
auto timestamp = 0;
musician.Update(timestamp + lookahead);

// Process the next output samples of the synth instrument.
//
// @note Instruments expect raw PCM audio samples to be processed with a synchronous call.
// Therefore, `Process` should typically be called from an audio thread process callback in
// real-time audio applications.
const int channel_count = 2;
const int frame_count = 1024;
std::vector<float> output_samples(channel_count * frame_count, 0.0f);
instrument.Process(output_samples.data(), channel_count, frame_count, timestamp);

// Create a performer.
auto performer = musician.CreatePerformer();

// Set the performer to loop.
performer.SetLooping(/*is_looping=*/true);

// Create a looping performer task that plays the A4 synth instrument note at half of a beat for a
// duration of quarter beats.
auto task = performer.CreateTask(
    [&]() {
      // Set the A4 note pitch on.
      instrument.SetNoteOn(0);
      // Schedule a one-off task to set the A4 note pitch off after quarter beats.
      performer.ScheduleOneOffTask([&]() { instrument.SetNoteOff(0); },
                                   performer.GetPosition() + Rational(1, 4));
    },
    /*position=*/Rational(1, 2));

// Start the performer.
performer.Start();
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):
```
bazel run //examples/demo:instrument_demo
```
