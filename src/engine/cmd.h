#ifndef BARELYMUSICIAN_ENGINE_CMD_H_
#define BARELYMUSICIAN_ENGINE_CMD_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <variant>

#include "core/constants.h"

namespace barely {

struct EngineControlCmd {
  BarelyEngineControlType type = BarelyEngineControlType_kCount;
  float value = 0.0f;
};

struct EngineSeedCmd {
  int32_t seed = 0;
};

struct InstrumentCreateCmd {
  uint32_t instrument_index = kInvalidIndex;
};

struct InstrumentDestroyCmd {
  uint32_t instrument_index = kInvalidIndex;
};

struct InstrumentControlCmd {
  uint32_t instrument_index = kInvalidIndex;
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;
  float value;
};

struct NoteControlCmd {
  uint32_t instrument_index = kInvalidIndex;
  float pitch = 0.0f;
  BarelyNoteControlType type = BarelyNoteControlType_kCount;
  float value = 0.0f;
};

struct NoteOffCmd {
  uint32_t instrument_index = kInvalidIndex;
  float pitch = 0.0f;
};

struct NoteOnCmd {
  uint32_t instrument_index = kInvalidIndex;
  float pitch = 0.0f;
};

struct SampleDataCmd {
  uint32_t instrument_index = kInvalidIndex;
  uint32_t first_slice_index = kInvalidIndex;
};

using Cmd =
    std::variant<EngineControlCmd, EngineSeedCmd, InstrumentCreateCmd, InstrumentDestroyCmd,
                 InstrumentControlCmd, NoteControlCmd, NoteOffCmd, NoteOnCmd, SampleDataCmd>;

template <typename... CmdTypes>
struct CmdVisitor : CmdTypes... {  // NOLINT(misc-multiple-inheritance)
  using CmdTypes::operator()...;
};
template <typename... CmdTypes>
CmdVisitor(CmdTypes...) -> CmdVisitor<CmdTypes...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_CMD_H_
