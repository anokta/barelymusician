#include "barelymusician/api/performer.h"

#include "barelymusician/api/conductor.h"
#include "barelymusician/api/status.h"
#include "barelymusician/conductor/transport.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/performer/sequence.h"

extern "C" {

// TODO: temp workaround for testing.
struct BarelyNote {
  int id;
};

/// Performer.
struct BarelyPerformer {
  BarelyInstrumentHandle instrument_handle;

  barelyapi::Sequence sequence;

  // TODO: temp workaround for testing.
  int count = 0;
};

BarelyStatus BarelyPerformer_AddNote(BarelyPerformerHandle handle,
                                     BarelyNoteDefinition definition,
                                     double position,
                                     BarelyNoteHandle* out_note_handle) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_note_handle) return BarelyStatus_kInvalidArgument;

  // TODO: temp workaround for testing.
  const int note_id = ++handle->count;
  *out_note_handle = new BarelyNote{note_id};

  handle->sequence.AddNote(note_id, position, definition);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_AddParameterAutomation(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationDefinition definition, double position,
    BarelyParameterAutomationHandle* out_parameter_automation_handle) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_parameter_automation_handle) return BarelyStatus_kInvalidArgument;

  definition, position, out_parameter_automation_handle;
  return BarelyStatus_kUnimplemented;
}
BarelyStatus BarelyPerformer_Create(BarelyPerformerHandle* out_handle) {
  if (!out_handle) return BarelyStatus_kInvalidArgument;

  *out_handle = new BarelyPerformer();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_Destroy(BarelyPerformerHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  delete handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetBeginOffset(BarelyPerformerHandle handle,
                                            double* out_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_begin_offset) return BarelyStatus_kInvalidArgument;

  *out_begin_offset = handle->sequence.GetBeginOffset();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetBeginPosition(BarelyPerformerHandle handle,
                                              double* out_begin_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_begin_position) return BarelyStatus_kInvalidArgument;

  *out_begin_position = handle->sequence.GetBeginPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetEndPosition(BarelyPerformerHandle handle,
                                            double* out_end_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_end_position) return BarelyStatus_kInvalidArgument;

  *out_end_position = handle->sequence.GetEndPosition();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetInstrument(
    BarelyPerformerHandle handle,
    BarelyInstrumentHandle* out_instrument_handle) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_instrument_handle) return BarelyStatus_kInvalidArgument;

  *out_instrument_handle = handle->instrument_handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetLoopBeginOffset(BarelyPerformerHandle handle,
                                                double* out_loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_loop_begin_offset) return BarelyStatus_kInvalidArgument;

  *out_loop_begin_offset = handle->sequence.GetLoopBeginOffset();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetLoopLength(BarelyPerformerHandle handle,
                                           double* out_loop_length) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_loop_length) return BarelyStatus_kInvalidArgument;

  *out_loop_length = handle->sequence.GetLoopLength();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_GetNoteDefinition(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  note_handle;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_GetNotePosition(BarelyPerformerHandle handle,
                                             BarelyNoteHandle note_handle,
                                             double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  note_handle;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_GetParameterAutomationDefinition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition* out_definition) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_definition) return BarelyStatus_kInvalidArgument;

  parameter_automation_handle;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_GetParameterAutomationPosition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double* out_position) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_position) return BarelyStatus_kInvalidArgument;

  parameter_automation_handle;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_IsEmpty(BarelyPerformerHandle handle,
                                     bool* out_is_empty) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_empty) return BarelyStatus_kInvalidArgument;

  *out_is_empty = handle->sequence.IsEmpty();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_IsLooping(BarelyPerformerHandle handle,
                                       bool* out_is_looping) {
  if (!handle) return BarelyStatus_kNotFound;
  if (!out_is_looping) return BarelyStatus_kInvalidArgument;

  *out_is_looping = handle->sequence.IsLooping();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_Perform(BarelyPerformerHandle handle,
                                     BarelyConductorHandle conductor_handle,
                                     double begin_position,
                                     double end_position) {
  if (!handle) return BarelyStatus_kNotFound;

  const auto& conductor =
      *reinterpret_cast<barelyapi::Transport*>(conductor_handle);
  auto& sequence = handle->sequence;
  std::multimap<double, barelyapi::Event> events;
  sequence.SetEventCallback([&](double position, barelyapi::Event event) {
    events.emplace(position, std::move(event));
  });
  sequence.Process(begin_position, end_position);

  if (handle->instrument_handle) {
    auto& instrument =
        *reinterpret_cast<barelyapi::Instrument*>(handle->instrument_handle);
    for (auto& [position, event] : events) {
      instrument.ProcessEvent(std::move(event),
                              conductor.GetTimestamp(position));
    }
  }
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_RemoveAllNotes(BarelyPerformerHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.RemoveAllNotes();
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_RemoveAllNotesAtPosition(
    BarelyPerformerHandle handle, double position) {
  if (!handle) return BarelyStatus_kNotFound;

  position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_RemoveAllNotesAtRange(BarelyPerformerHandle handle,
                                                   double begin_position,
                                                   double end_position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.RemoveAllNotes(begin_position, end_position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_RemoveAllParameterAutomations(
    BarelyPerformerHandle handle) {
  if (!handle) return BarelyStatus_kNotFound;

  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_RemoveAllParameterAutomationsAtPosition(
    BarelyPerformerHandle handle, double position) {
  if (!handle) return BarelyStatus_kNotFound;

  position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_RemoveAllParameterAutomationsAtRange(
    BarelyPerformerHandle handle, double begin_position, double end_position) {
  if (!handle) return BarelyStatus_kNotFound;

  begin_position, end_position;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_RemoveNote(BarelyPerformerHandle handle,
                                        BarelyNoteHandle note_handle) {
  if (!handle) return BarelyStatus_kNotFound;

  // TODO: temp workaround for testing.
  if (handle->sequence.RemoveNote(note_handle->id)) {
    delete note_handle;
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_RemoveParameterAutomation(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle) {
  if (!handle) return BarelyStatus_kNotFound;

  parameter_automation_handle;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_SetBeginOffset(BarelyPerformerHandle handle,
                                            double begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetBeginOffset(begin_offset);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetBeginPosition(BarelyPerformerHandle handle,
                                              double begin_position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetBeginPosition(begin_position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetEndPosition(BarelyPerformerHandle handle,
                                            double end_position) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetEndPosition(end_position);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetInstrument(
    BarelyPerformerHandle handle, BarelyInstrumentHandle instrument_handle) {
  if (!handle) return BarelyStatus_kNotFound;

  // TODO: stop existing notes -- needs timestamp!

  handle->instrument_handle = instrument_handle;
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetLoopBeginOffset(BarelyPerformerHandle handle,
                                                double loop_begin_offset) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetLoopBeginOffset(loop_begin_offset);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetLoopLength(BarelyPerformerHandle handle,
                                           double loop_length) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetLoopLength(loop_length);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetLooping(BarelyPerformerHandle handle,
                                        bool is_looping) {
  if (!handle) return BarelyStatus_kNotFound;

  handle->sequence.SetLooping(is_looping);
  return BarelyStatus_kOk;
}

BarelyStatus BarelyPerformer_SetNoteDefinition(
    BarelyPerformerHandle handle, BarelyNoteHandle note_handle,
    BarelyNoteDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;

  // TODO: temp workaround for testing.
  if (handle->sequence.SetNoteDefinition(note_handle->id, definition)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetNotePosition(BarelyPerformerHandle handle,
                                             BarelyNoteHandle note_handle,
                                             double position) {
  if (!handle) return BarelyStatus_kNotFound;

  // TODO: temp workaround for testing.
  if (handle->sequence.SetNotePosition(note_handle->id, position)) {
    return BarelyStatus_kOk;
  }
  return BarelyStatus_kNotFound;
}

BarelyStatus BarelyPerformer_SetParameterAutomationDefinition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    BarelyParameterAutomationDefinition definition) {
  if (!handle) return BarelyStatus_kNotFound;

  parameter_automation_handle, definition;
  return BarelyStatus_kUnimplemented;
}

BarelyStatus BarelyPerformer_SetParameterAutomationPosition(
    BarelyPerformerHandle handle,
    BarelyParameterAutomationHandle parameter_automation_handle,
    double position) {
  if (!handle) return BarelyStatus_kNotFound;

  parameter_automation_handle, position;
  return BarelyStatus_kUnimplemented;
}

}  // extern "C"
