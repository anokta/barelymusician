barelymusician
[![ci](https://github.com/anokta/barelymusician/actions/workflows/ci.yml/badge.svg)](https://github.com/anokta/barelymusician/actions/workflows/ci.yml)
[![sponsor](https://img.shields.io/static/v1?label=sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/anokta)
==============

barelymusician is a real-time music engine for interactive systems.

It provides a modern C/C++ API to generate and perform musical sounds from scratch with sample
accurate timing.

This repository includes build targets for Windows, macOS, Linux, Android, WebAssembly, and Daisy,
as well as a Godot GDExtension, a native Unity plugin, and a VST instrument plugin.

To use in a project, simply include [barelymusician.h](include/barelymusician.h).

To use in Godot, download the latest version of
[barelymusiciangodot.zip](https://github.com/anokta/barelymusician/releases/latest/download/barelymusiciangodot.zip).

To use in Unity, download the latest version of
[barelymusician.unitypackage](https://github.com/anokta/barelymusician/releases/latest/download/barelymusician.unitypackage).

Just curious? Try the experimental web toy at [barelymusician.com](http://www.barelymusician.com).

For background about this project, see the original research paper
[here](http://www.aes.org/e-lib/browse.cfm?elib=17598), and the legacy Unity implementation
[here](https://github.com/anokta/barelyMusicianLegacy).

Example usage
-------------

```cpp
#include <barelymusician.h>

// Create the engine with a tempo of 124 beats per minute.
barely::Engine engine(/*sample_rate=*/48000);
engine.SetTempo(/*tempo=*/124.0);

// Create a new instrument with full oscillator mix.
auto instrument = engine.CreateInstrument();
instrument.SetControl(barely::InstrumentControlType::kOscMix, /*value=*/1.0f);

// Set an instrument note on.
//
// The note pitch is expressed in octaves relative to middle C as the center frequency. Fractional
// note values adjust the frequency logarithmically to ensure equally perceived pitch intervals
// within each octave.
constexpr float kC4Pitch = 0.0f;
instrument.SetNoteOn(kC4Pitch);

// Check if the instrument note is on.
const bool is_note_on = instrument.IsNoteOn(kC4Pitch);  // will return true.

// Create a new trigger that plays an instrument note every beat.
auto trigger = engine.CreateTrigger();
trigger.SetCallback([&]() {
  constexpr float kC3Pitch = -1.0f;
  instrument.SetNoteOn(kC3Pitch);
  instrument.SetNoteOff(kC3Pitch, /*offset=*/1.0f);
});
trigger.Start(/*interval=*/1.0f);

// Update the engine timestamp.
//
// Timestamp updates must occur before processing the engine with the respective timestamps.
// Otherwise, `Process` calls may be *late* in receiving relevant changes to the engine. To address
// this, `Update` should typically be called from the main thread update callback using a lookahead
// to prevent potential thread synchronization issues in real-time audio applications.
constexpr double kLookahead = 0.1;
double timestamp = 0.0;
engine.Update(timestamp + kLookahead);

// Process the next output samples of the engine.
//
// The engine processes output samples synchronously. Therefore, `Process` should typically be
// called from an audio thread process callback in real-time audio applications.
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 512;
float output_samples[kChannelCount * kFrameCount];
engine.Process(output_samples, kChannelCount, kFrameCount, timestamp);
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):

```sh
python build.py --run_demo instrument_demo
```
