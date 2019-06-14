#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message.h"

namespace barelyapi {

// Returns instrument note off message.
//
// @param index Note off index.
// @param timestamp Message timestamp.
// @return Note off message.
Message BuildNoteOffMessage(float index, int timestamp);

// Returns instrument note on message.
//
// @param index Note on index.
// @param intensity Note on intensity.
// @param timestamp Message timestamp.
// @return Note on message.
Message BuildNoteOnMessage(float index, float intensity, int timestamp);

// Converts a note index value into the corresponding pitch frequency.
//
// @param index Note index.
// @return Frequency in Hz.
float FrequencyFromNoteIndex(float index);

// Processes the given instrument |message|.
//
// @param message Message to process.
// @param instrument Instrument to process the message for.
void ProcessMessage(const Message& message, Instrument* instrument);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
