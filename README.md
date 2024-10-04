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

To use in a project, simply include
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
// Import the synth instrument.
#include "barelymusician/instruments/synth_instrument.h"

// Create a musician.
barely::Musician musician(/*frame_rate=*/48000);

// Set the global tempo to 124 beats per minute.
musician.SetTempo(/*tempo=*/124.0);

// Create a synth instrument.
barely::Instrument instrument(musician, barely::SynthInstrument::GetDefinition());

// Set the instrument gain to half.
instrument.SetControl(barely::SynthInstrument::Control::kGain, /*value=*/0.5);

// Set an instrument note on.
//
// Note values typically follow the MIDI Standard Tuning for convenience, where `pitch` represents
// the MIDI note number, and `intensity` represents the normalized MIDI note velocity. However, this
// is not a strict rule, as the `pitch` and `intensity` values can be interpreted in any desired way
// by a custom instrument with a custom tuning.
instrument.SetNoteOn(/*pitch=*/60, /*intensity=*/0.25);

// Check if the instrument note is on.
const bool is_note_on = instrument.IsNoteOn(/*note=*/60);  // will return true.

// Create a performer.
barely::Performer performer(musician);

// Set the performer to loop.
performer.SetLooping(/*is_looping=*/true);

// Create a looping task that plays an instrument note every beat.
Task task(
    performer,
    [&]() {
      // Set an instrument note on.
      instrument.SetNoteOn(/*pitch=*/62);
      // Schedule a one-off task to set the instrument note off after half a beat.
      performer.ScheduleOneOffTask([&]() { instrument.SetNoteOff(/*pitch=*/62); },
                                   performer.GetPosition() + 0.5);
    });

// Start the performer.
performer.Start();

// Update the musician timestamp in seconds.
//
// Timestamp updates must occur before processing instruments with their respective timestamps.
// Otherwise, such `Process` calls will be *late* to receive the relevant state changes. To
// compensate for this, `Update` should typically be called from a main thread update callback with
// an additional "lookahead" to avoid potential thread synchronization issues that could arise in
// real-time audio applications.
const double lookahead = 0.1;
double timestamp = 0.0;
musician.Update(timestamp + lookahead);

// Process the next output samples of the instrument.
//
// Instruments process raw PCM audio samples in a synchronous call. Therefore, `Process` should
// typically be called from an audio thread process callback in real-time audio applications.
const int channel_count = 2;
const int frame_count = 1024;
std::vector<double> output_samples(channel_count * frame_count, 0.0);
instrument.Process(output_samples.data(), channel_count, frame_count, timestamp);
```

Further examples can be found in [examples/demo](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):
```
python build.py --run_demo instrument_demo
```
