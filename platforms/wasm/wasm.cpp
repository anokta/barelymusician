#include <barelymusician.h>

#include "emscripten/bind.h"

using ::barely::ControlType;
using ::barely::Engine;
using ::barely::Instrument;

EMSCRIPTEN_BINDINGS(barelymusician) {
  emscripten::class_<Engine>("Engine")
      .constructor<int, float>()
      .function("generateRandomNumber",
                static_cast<double (Engine::*)()>(&Engine::GenerateRandomNumber))
      .function("getTimestamp", &Engine::GetTimestamp)
      .function("update", &Engine::Update)
      .property("seed", &Engine::GetSeed, &Engine::SetSeed)
      .property("tempo", &Engine::GetTempo, &Engine::SetTempo);

  emscripten::class_<Instrument>("Instrument")
      .function("isNoteOn", &Instrument::IsNoteOn)
      .function("setAllNotesOff", &Instrument::SetAllNotesOff)
      .function("setControl",
                static_cast<void (Instrument::*)(ControlType, float)>(&Instrument::SetControl))
      .function("setNoteOff", &Instrument::SetNoteOff)
      .function("setNoteOn",
                static_cast<void (Instrument::*)(float, float, float)>(&Instrument::SetNoteOn));
}
