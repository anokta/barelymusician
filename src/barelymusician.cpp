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
  PrintUnimplementedError("BarelyCreateInstrument", handle, &definition);
  return kBarelyInvalidId;
}

BarelyInstrumentId BarelyCreatePerformer(BarelyHandle handle) {
  PrintUnimplementedError("BarelyCreatePerformer", handle);
  return kBarelyInvalidId;
}

BarelyStatus BarelyDestroyInstrument(BarelyHandle handle,
                                     BarelyInstrumentId instrument_id) {
  PrintUnimplementedError("BarelyDestroyInstrument", handle, instrument_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyDestroyPerformer(BarelyHandle handle,
                                    BarelyPerformerId performer_id) {
  PrintUnimplementedError("BarelyDestroyPerformer", handle, performer_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetInstrumentControl(const BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        BarelyInstrumentControlId control_id,
                                        float* control_value) {
  PrintUnimplementedError("BarelyGetInstrumentControl", handle, instrument_id,
                          control_id, control_value);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetPerformerInstrument(const BarelyHandle handle,
                                          BarelyPerformerId performer_id,
                                          BarelyInstrumentId* instrument_id) {
  PrintUnimplementedError("BarelySetPerformerInstrument", handle, performer_id,
                          instrument_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetPlaybackPosition(const BarelyHandle handle,
                                       BarelyPosition* position) {
  PrintUnimplementedError("BarelyGetPlaybackPosition", handle, position);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetPlaybackTempo(const BarelyHandle handle, double* tempo) {
  PrintUnimplementedError("BarelyGetPlaybackTempo", handle, tempo);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetEnergy(const BarelyHandle handle, float* energy) {
  PrintUnimplementedError("BarelyGetEnergy", handle, energy);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyGetStress(const BarelyHandle handle, float* stress) {
  PrintUnimplementedError("BarelyGetStress", handle, stress);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyIsInstrumentNoteOn(const BarelyHandle handle,
                                      BarelyInstrumentId instrument_id,
                                      float note_index, bool* is_note_on) {
  PrintUnimplementedError("BarelyIsInstrumentNoteOn", handle, instrument_id,
                          note_index, is_note_on);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyIsPlaying(const BarelyHandle handle, bool* is_playing) {
  PrintUnimplementedError("BarelyIsPlaying", handle, is_playing);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyPerformNote(BarelyHandle handle,
                               BarelyPerformerId performer_id,
                               BarelyNote note) {
  PrintUnimplementedError("BarelyPerformNote", handle, performer_id, &note);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyProcessInstrument(BarelyHandle handle,
                                     BarelyInstrumentId instrument_id,
                                     BarelyTimestamp timestamp,
                                     BarelyBuffer output) {
  PrintUnimplementedError("BarelyIsInstrumentNoteOn", handle, instrument_id,
                          timestamp, &output);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelyResetAllInstrumentControls(
    BarelyHandle handle, BarelyInstrumentId instrument_id) {
  PrintUnimplementedError("BarelyResetAllInstrumentControls", handle,
                          instrument_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetAllInstrumentNotesOff(BarelyHandle handle,
                                            BarelyInstrumentId instrument_id) {
  PrintUnimplementedError("BarelySetAllInstrumentNotesOff", handle,
                          instrument_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetBeatCallback(BarelyHandle handle,
                                   BarelyBeatCallback beat_callback) {
  PrintUnimplementedError("BarelySetBeatCallback", handle, beat_callback);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetConductor(BarelyHandle handle,
                                BarelyConductorDefinition definition) {
  PrintUnimplementedError("BarelySetConductor", handle, &definition);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentControl(BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        BarelyInstrumentControlId control_id,
                                        float control_value) {
  PrintUnimplementedError("BarelySetInstrumentControl", handle, instrument_id,
                          control_id, control_value);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOn(BarelyHandle handle,
                                       BarelyInstrumentId instrument_id,
                                       float note_index, float note_intensity) {
  PrintUnimplementedError("BarelySetInstrumentNoteOn", handle, instrument_id,
                          note_index, note_intensity);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetInstrumentNoteOff(BarelyHandle handle,
                                        BarelyInstrumentId instrument_id,
                                        float note_index) {
  PrintUnimplementedError("BarelySetInstrumentNoteOn", handle, instrument_id,
                          note_index);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetNoteOnCallback(BarelyHandle handle,
                                     BarelyNoteOnCallback note_on_callback) {
  PrintUnimplementedError("BarelySetNoteOnCallback", handle, note_on_callback);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetNoteOffCallback(BarelyHandle handle,
                                      BarelyNoteOffCallback note_off_callback) {
  PrintUnimplementedError("BarelySetNoteOffCallback", handle,
                          note_off_callback);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetPerformerInstrument(BarelyHandle handle,
                                          BarelyPerformerId performer_id,
                                          BarelyInstrumentId instrument_id) {
  PrintUnimplementedError("BarelySetPerformerInstrument", handle, performer_id,
                          instrument_id);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetPlaybackPosition(BarelyHandle handle,
                                       BarelyPosition position) {
  PrintUnimplementedError("BarelySetPlaybackPosition", handle, position);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetPlaybackTempo(BarelyHandle handle, double tempo) {
  PrintUnimplementedError("BarelySetPlaybackTempo", handle, tempo);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetEnergy(BarelyHandle handle, float energy) {
  PrintUnimplementedError("BarelySetEnergy", handle, energy);
  return kBarelyStatus_Unimplemented;
}

BarelyStatus BarelySetStress(BarelyHandle handle, float stress) {
  PrintUnimplementedError("BarelySetStress", handle, stress);
  return kBarelyStatus_Unimplemented;
}

}  // extern "C"
