barelymusician
[![ci](https://github.com/anokta/barelymusician/actions/workflows/ci.yml/badge.svg)](https://github.com/anokta/barelymusician/actions/workflows/ci.yml)
[![sponsor](https://img.shields.io/static/v1?label=sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/anokta)
==============

barelymusician is a real-time music engine for interactive systems.

It provides a modern C/C++ API to generate and perform musical sounds from scratch in a sample
accurate way.

[Try out the web demo](http://barelymusician.com) to get started!

This repository includes build targets for Windows, macOS, Linux, Android, WebAssembly, and Daisy
platforms, as well as a native Unity game engine plugin and a VST instrument plugin.

To use in a project, simply include [barelymusician.h](include/barelymusician.h).

To use in Unity, download the latest version of
[barelymusician.unitypackage](https://github.com/anokta/barelymusician/releases/latest/download/barelymusician.unitypackage).

For background about this project, see the original research paper
[here](http://www.aes.org/e-lib/browse.cfm?elib=17598), and the legacy Unity implementation
[here](https://github.com/anokta/barelyMusicianLegacy).

Example usage
-------------

```cpp
#include <barelymusician.h>

// Create the engine.
barely::Engine engine(/*sample_rate=*/48000);

// Set the global tempo.
engine.SetTempo(/*tempo=*/124.0);

// Create a new instrument.
auto instrument = engine.CreateInstrument();

// Set the instrument to use the full oscillator mix.
instrument.SetControl(barely::ControlType::kOscMix, /*value=*/1.0f);

// Set an instrument note on.
//
// The note pitch is centered around the reference frequency and measured in octaves. Fractional
// note values adjust the frequency logarithmically to ensure equally perceived pitch intervals
// within each octave.
constexpr float kC4Pitch = 0.0f;
instrument.SetNoteOn(kC4Pitch);

// Check if the instrument note is on.
const bool is_note_on = instrument.IsNoteOn(kC4Pitch);  // will return true.

// Create a new performer.
auto performer = engine.CreatePerformer();

// Set the performer to looping.
performer.SetLooping(/*is_looping=*/true);

// Create a new task that plays an instrument note every beat.
auto task = performer.CreateTask(/*position=*/0.0, /*duration=*/1.0, [&](barely::TaskState state) {
  constexpr float kC3Pitch = -1.0f;
  if (state == barely::TaskState::kBegin) {
    instrument.SetNoteOn(kC3Pitch);
  } else if (state == barely::TaskState::kEnd) {
    instrument.SetNoteOff(kC3Pitch);
  }
});

// Start the performer.
performer.Start();

// Update the engine timestamp in seconds.
//
// Timestamp updates must occur before processing instruments with their respective timestamps.
// Otherwise, `Process` calls may be *late* in receiving the relevant changes to the instruments. To
// address this, `Update` should typically be called from the main thread update callback using a
// lookahead to prevent potential thread synchronization issues in real-time audio applications.
constexpr double kLookahead = 0.1;
double timestamp = 0.0;
engine.Update(timestamp + kLookahead);

// Process the next output samples of the instrument.
//
// Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process` should
// typically be called from an audio thread process callback in real-time audio applications.
float output_samples[1024];
instrument.Process(output_samples, timestamp);
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):

```sh
python build.py --run_demo instrument_demo
```
