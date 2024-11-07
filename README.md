barelymusician
[![ci](https://github.com/anokta/barelymusician/actions/workflows/ci.yml/badge.svg)](https://github.com/anokta/barelymusician/actions/workflows/ci.yml)
[![](https://img.shields.io/static/v1?label=sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/anokta)
==============

barelymusician is a real-time music engine for interactive systems.

It provides a modern C/C++ API to generate and perform musical sounds from scratch in a sample
accurate way.

[iOS]: ## "see issue #112 for the status of the upcoming iOS platform support"
This repository includes build targets for Linux, OSX, Windows, Android, and Daisy platforms, in
addition to a native Unity game engine plugin[*][iOS].

To use in a project, simply include [barelymusician.h](include/barelymusician.h).

To use in Unity, download the latest version of
[barelymusician.unitypackage](https://github.com/anokta/barelymusician/releases/latest/download/barelymusician.unitypackage).

For background about this project, see the original research paper
[here](http://www.aes.org/e-lib/browse.cfm?elib=17598), and the legacy Unity implementation
[here](https://github.com/anokta/barelyMusicianLegacy).

## Example usage

```cpp
#include "barelymusician.h"

// Create the musician.
barely::Musician musician(/*sample_rate=*/48000);

// Set the global tempo to 124 beats per minute.
musician.SetTempo(/*tempo=*/124.0);

// Add an instrument.
auto instrument = musician.AddInstrument();

// Set the instrument gain to half.
instrument.SetControl(barely::ControlType::kGain, /*value=*/0.5);

// Set an instrument note on.
//
// Note pitch is centered around the reference frequency, and measured in octaves. Fractional values
// adjust the frequency logarithmically to maintain perceived pitch intervals in each octave.
constexpr double kC4Pitch = 0.0;
instrument.SetNoteOn(kC4Pitch, /*intensity=*/0.25);

// Check if the instrument note is on.
const bool is_note_on = instrument.IsNoteOn(kC4Pitch);  // will return true.

// Add a performer.
auto performer = musician.AddPerformer();

// Set the performer to loop.
performer.SetLooping(/*is_looping=*/true);

// Add a looping task that plays an instrument note every beat.
auto task = performer.AddTask(
    [&]() {
      // Set an instrument note on.
      instrument.SetNoteOn(/*pitch=*/1.0);
      // Schedule a one-off task to set the instrument note off after half a beat.
      performer.ScheduleOneOffTask([&]() { instrument.SetNoteOff(/*pitch=*/1.0); },
                                   performer.GetPosition() + 0.5);
    },
    /*position=*/0.0);

// Start the performer.
performer.Start();

// Update the musician timestamp in seconds.
//
// Timestamp updates must occur before processing instruments with their respective timestamps.
// Otherwise, such `Process` calls will be *late* to receive the relevant state changes. To
// compensate for this, `Update` should typically be called from a main thread update callback with
// an additional "lookahead" to avoid potential thread synchronization issues that could arise in
// real-time audio applications.
constexpr double kLookahead = 0.1;
double timestamp = 0.0;
musician.Update(timestamp + kLookahead);

// Process the next output samples of the instrument.
//
// Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process` should
// typically be called from an audio thread process callback in real-time audio applications.
double output_samples[1024];
instrument.Process(output_samples, timestamp);
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):
```
python build.py --run_demo instrument_demo
```
