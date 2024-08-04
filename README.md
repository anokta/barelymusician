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
// Import the low-pass effect.
#include "barelymusician/effects/low_pass_effect.h"
// Import the synth instrument.
#include "barelymusician/instruments/synth_instrument.h"

// Create a musician.
barely::Musician musician;

// Set the global tempo to 124 beats per minute.
musician.SetTempo(/*tempo=*/124.0);

// Create a synth instrument.
barely::Instrument instrument(musician, barely::SynthInstrument::GetDefinition(),
                              /*frame_rate=*/48000);

// Set the instrument gain to 0.5.
instrument.GetControl(barely::SynthInstrument::Control::kGain).SetValue(/*value=*/0.5);

// Start a note with an A3 pitch and a 0.25 instensity.
//
// @note Pitch values are normalized by octaves, where each 1.0 value change shifts one octave, and
// 0.0 represents the A4 (middle A) pitch at 440 hertz in a typical instrument definition. However,
// this is not a strict rule, since `pitch` and `intensity` can be interpreted in any desired way by
// a custom instrument.
const double a3_pitch = -1.0;
const barely::Note note(instrument, a3_pitch, /*intensity=*/0.25);

// Create a low-pass effect.
barely::Effect effect(musician, barely::LowPassEffect::GetDefinition(), kFrameRate);

// Set the effect cutoff frequency to 1kHz.
effect.GetControl(barely::LowPassEffect::Control::kCutoffFrequency).SetValue(/*value=*/1000.0);

// Update the musician timestamp in seconds.
//
// @note Timestamp updates must happen prior to processing of instruments with respective
// timestamps. Otherwise, such `Process` calls will be *late* to receive any relevant state changes.
// To compensate, `Update` should typically be called from a main thread update callback, with an
// additional "lookahead", in order to avoid any potential thread synchronization issues that could
// occur in real-time audio applications.
const double lookahead = 0.1;
double timestamp = 0.0;
musician.Update(timestamp + lookahead);

// Process the next output samples of the instrument.
//
// @note Instruments expect raw PCM audio samples to be processed with a synchronous call.
// Therefore, `Process` should typically be called from an audio thread process callback in
// real-time audio applications.
const int channel_count = 2;
const int frame_count = 1024;
std::vector<double> output_samples(channel_count * frame_count, 0.0);
instrument.Process(output_samples.data(), channel_count, frame_count, timestamp);

// Process the effect on the instrument output.
effect.Process(output_samples.data(), channel_count, frame_count, timestamp);

// Create a performer.
barely::Performer performer(musician);

// Set the performer to loop.
performer.SetLooping(/*is_looping=*/true);

// Create a looping task that plays an note at the position 0.5 beats for a duration of 0.25 beats.
Task task(
    performer,
    [&]() {
      // Play an instrument note with an A4 pitch for 0.25 beats.
      const double a4_pitch = 0.0;
      performer.ScheduleOneOffTask(
          [note_ptr = barely::Note(instrument, a4_pitch).Release()]() {
            NotePtr::Destroy(note_ptr);
          },
          performer.GetPosition() + 0.25);
    },
    /*position=*/0.5);

// Start the performer.
performer.Start();
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):
```
bazel run //examples/demo:instrument_demo
```
