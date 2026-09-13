#ifndef BARELYMUSICIAN_ENGINE_CMD_H_
#define BARELYMUSICIAN_ENGINE_CMD_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <variant>

#include "core/constants.h"

namespace barely {

struct EngineControlCmd {
  double value = 0.0;
  BarelyEngineControlType type = BarelyEngineControlType_kCount;
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
  double value = 0.0;
  uint32_t instrument_index = kInvalidIndex;
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;
};

struct NoteControlCmd {
  double pitch = 0.0;
  double value = 0.0;
  uint32_t instrument_index = kInvalidIndex;
  BarelyNoteControlType type = BarelyNoteControlType_kCount;
};

struct NoteOffCmd {
  double pitch = 0.0;
  uint32_t instrument_index = kInvalidIndex;
};

struct NoteOnCmd {
  double pitch = 0.0;
  uint32_t instrument_index = kInvalidIndex;
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
