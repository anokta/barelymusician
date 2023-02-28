barelymusician
[![ci](https://github.com/anokta/barelymusician/actions/workflows/ci.yml/badge.svg)](https://github.com/anokta/barelymusician/actions/workflows/ci.yml)
==============

barelymusician is a real-time music engine for interactive systems.

It provides a modern C/C++ API to generate and perform musical sounds from
scratch in a sample accurate way.

[iOS]: ## "see issue #112 for the status of the upcoming iOS platform support"
The repository currently includes build targets for Linux, OSX, Windows and
Android platforms[*][iOS], in addition to a native Unity plugin. The targets can
be built with [Bazel](https://bazel.build/) or [CMake](https://cmake.org/). To
use in a project, simply include
[barelymusician/barelymusician.h](barelymusician/barelymusician.h).

Example usage can be found in [examples](examples/demo), e.g. to run the
[instrument_demo.cpp](examples/demo/instrument_demo.cpp):
```
bazel run //examples/demo:instrument_demo
```

For background about this project, see the original research paper
[here](http://www.aes.org/e-lib/browse.cfm?elib=17598), with the legacy Unity
implementation [here](https://github.com/anokta/barelyMusicianLegacy).
