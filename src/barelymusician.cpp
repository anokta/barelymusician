#include "barelymusician.h"

#include <iostream>

extern "C" {

struct BarelySystem {};

BarelyHandle BarelyCreateSystem() { return new BarelySystem(); }

void BarelyDestroySystem(BarelyHandle handle) { delete handle; }

BarelyInstrumentId BarelyCreateInstrument(
    BarelyHandle handle, BarelyInstrumentDefinition definition) {
  std::cout << "Called unimplemented function: "
            << "BarelyCreateInstrument(" << handle << ", " << definition.name
            << ")" << std::endl;
  return kBarelyInvalidId;
}

BarelyStatus BarelyDestroyInstrument(BarelyHandle handle,
                                     BarelyInstrumentId id) {
  std::cout << "Called unimplemented function: "
            << "BarelyDestroyInstrument(" << handle << ", " << id << ")"
            << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetInstrumentControl(const BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float* control_value) {
  std::cout << "Called unimplemented function: "
            << "BarelyGetInstrumentControl(" << handle << ", " << id << ", "
            << control_id << "," << control_value << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyIsInstrumentNoteOn(const BarelyHandle handle,
                                      BarelyInstrumentId id, float note_index,
                                      bool* is_note_on) {
  std::cout << "Called unimplemented function: "
            << "BarelyIsInstrumentNoteOn(" << handle << ", " << id << ", "
            << note_index << "," << is_note_on << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyProcessInstrument(BarelyHandle handle, BarelyInstrumentId id,
                                     BarelyTimestamp timestamp,
                                     BarelyBuffer output) {
  std::cout << "Called unimplemented function: "
            << "BarelyProcessInstrument(" << handle << ", " << id << ", "
            << timestamp << ", " << output.data << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentControl(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        BarelyInstrumentControlId control_id,
                                        float control_value) {
  std::cout << "Called unimplemented function: "
            << "BarelySetInstrumentControl(" << handle << ", " << id << ", "
            << control_id << "," << control_value << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyInstrumentId id, float note_index,
                                       float note_intensity) {
  std::cout << "Called unimplemented function: "
            << "BarelySetInstrumentNoteOn(" << handle << ", " << id << ", "
            << note_index << "," << note_intensity << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyInstrumentId id,
                                        float note_index) {
  std::cout << "Called unimplemented function: "
            << "BarelySetInstrumentNoteOff(" << handle << ", " << id << ", "
            << note_index << ")" << std::endl;
  return kBarelyStatus_Unimplemented;
}

}  // extern "C"
