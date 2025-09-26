#ifndef BARELYMUSICIAN_DSP_MESSAGE_H_
#define BARELYMUSICIAN_DSP_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstddef>
#include <optional>
#include <variant>

#include "dsp/biquad_filter.h"
#include "dsp/sample_data.h"

namespace barely {

/// Engine control message.
struct EngineControlMessage {
  /// Type.
  EngineControlType type;

  /// Value.
  float value;
};

/// Instrument control message.
struct InstrumentControlMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Type.
  InstrumentControlType type;

  /// Value.
  float value;
};

/// Instrument filter control message.
struct InstrumentFilterControlMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Filter coefficients.
  BiquadFilter::Coefficients coeffs;
};

/// Note control message.
struct NoteControlMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Pitch.
  float pitch;

  /// Type.
  NoteControlType type;

  /// Value.
  float value;
};

/// Note off message.
struct NoteOffMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Pitch.
  float pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Pitch.
  float pitch;

  /// Array of note controls.
  std::array<float, BarelyNoteControlType_kCount> controls;
};

/// Sample data message.
struct SampleDataMessage {
  /// Instrument handle.
  BarelyInstrumentHandle instrument;

  /// Sample data.
  SampleData sample_data;
};

/// Message alias.
using Message =
    std::variant<EngineControlMessage, InstrumentControlMessage, InstrumentFilterControlMessage,
                 NoteControlMessage, NoteOffMessage, NoteOnMessage, SampleDataMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_MESSAGE_H_
