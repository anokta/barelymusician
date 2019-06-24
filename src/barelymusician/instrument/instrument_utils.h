#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"

namespace barelyapi {

// Converts a note index value into the corresponding pitch frequency.
//
// @param index Note index.
// @return Frequency in Hz.
float FrequencyFromNoteIndex(float index);

// Processes the next |output| buffer with the given |instrument| and
// |message_buffer|.
//
// @param instrument Instrument to process.
// @param messages Messages to process.
// @param output Output buffer.
// @param num_channels Number of output channels.
// @param num_frames Number of output frames.
void Process(Instrument* instrument, const MessageBuffer::Iterator& messages,
             float* output, int num_channels, int num_frames);

// Pushes note off message to the given |message_buffer|.
//
// @param index Note off index.
// @param timestamp Message timestamp.
// @param message_buffer Message queue to push the note off message.
void PushNoteOffMessage(float index, int timestamp,
                        MessageBuffer* message_buffer);

// Pushes note on message to the given |message_buffer|.
//
// @param index Note on index.
// @param intensity Note on intensity.
// @param timestamp Message timestamp.
// @param message_buffer Message queue to push the note on message.
void PushNoteOnMessage(float index, float intensity, int timestamp,
                       MessageBuffer* message_buffer);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_UTILS_H_
