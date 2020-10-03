#include "barelymusician.h"

#include <iostream>

namespace {

template <typename... Args>
void PrintUnimplementedError(const std::string& name, Args const&... args) {
  std::cout << "Unimplemented function : " << name << " (";
  ((std::cout << " " << args), ...) << " )" << std::endl;
}

}  // namespace

extern "C" {

struct BarelySystem {};

BarelyHandle BarelyCreateSystem() { return new BarelySystem(); }

void BarelyDestroySystem(BarelyHandle handle) { delete handle; }

BarelyInstrumentId BarelyCreateInstrument(
    BarelyHandle handle, BarelyInstrumentDefinition definition) {
  PrintUnimplementedError("BarelyCreateInstrument", handle, definition.name);
  return kBarelyInvalidId;
}

BarelyStatus BarelyDestroyInstrument(BarelyHandle handle,
                                     BarelyInstrumentId id) {
  PrintUnimplementedError("BarelyDestroyInstrument", handle, id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetInstrumentControl(const BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float* control_value) {
  PrintUnimplementedError("BarelyGetInstrumentControl", handle, id, control_id,
                          control_value);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetPlaybackPosition(BarelyHandle handle, double* position) {
  PrintUnimplementedError("BarelyGetPlaybackPosition", handle, position);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetPlaybackTempo(BarelyHandle handle, double* tempo) {
  PrintUnimplementedError("BarelyGetPlaybackTempo", handle, tempo);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyIsInstrumentNoteOn(const BarelyHandle handle,
                                      BarelyInstrumentId id, float note_index,
                                      bool* is_note_on) {
  PrintUnimplementedError("BarelyIsInstrumentNoteOn", handle, id, note_index,
                          is_note_on);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyProcessInstrument(BarelyHandle handle, BarelyInstrumentId id,
                                     BarelyTimestamp timestamp,
                                     BarelyBuffer output) {
  PrintUnimplementedError("BarelyIsInstrumentNoteOn", handle, id, timestamp,
                          output.data);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentControl(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float control_value) {
  PrintUnimplementedError("BarelySetInstrumentControl", handle, id, control_id,
                          control_value);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyInstrumentId id, float note_index,
                                       float note_intensity) {
  PrintUnimplementedError("BarelySetInstrumentNoteOn", handle, id, note_index,
                          note_intensity);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        float note_index) {
  PrintUnimplementedError("BarelySetInstrumentNoteOn", handle, id, note_index);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle, double position) {
  PrintUnimplementedError("BarelySetPlaybackPosition", handle, position);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo) {
  PrintUnimplementedError("BarelySetPlaybackTempo", handle, tempo);
  return kBarelyStatus_Unimplemented;
}

}  // extern "C"
