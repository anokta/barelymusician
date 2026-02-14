/**
 * Command types.
 * @enum {int}
 */
export const CommandType = Object.freeze({
  ENGINE_SET_CONTROL: 0,
  ENGINE_SET_TEMPO: 1,

  INSTRUMENT_CREATE: 2,
  INSTRUMENT_DESTROY: 3,
  INSTRUMENT_SET_ALL_NOTES_OFF: 4,
  INSTRUMENT_SET_CONTROL: 5,
  INSTRUMENT_SET_NOTE_CONTROL: 6,
  INSTRUMENT_SET_NOTE_ON: 7,
  INSTRUMENT_SET_NOTE_OFF: 8,
  INSTRUMENT_SET_SAMPLE_DATA: 9,

  PERFORMER_CREATE: 10,
  PERFORMER_DESTROY: 11,
  PERFORMER_SET_LOOP_BEGIN_POSITION: 12,
  PERFORMER_SET_LOOP_LENGTH: 13,
  PERFORMER_SET_LOOPING: 14,
  PERFORMER_SET_POSITION: 15,
  PERFORMER_START: 16,
  PERFORMER_STOP: 17,
  PERFORMER_SYNC_TO: 18,

  TASK_CREATE: 19,
  TASK_DESTROY: 20,
  TASK_SET_COMMANDS: 21,
  TASK_SET_DURATION: 22,
  TASK_SET_POSITION: 23,
  TASK_SET_PRIORITY: 24,
});

/**
 * Event callback types.
 * @enum {int}
 */
export const EventCallbackType = Object.freeze({
  INSTRUMENT_ON_NOTE_BEGIN: 0,
  INSTRUMENT_ON_NOTE_END: 1,
  TASK_ON_BEGIN: 2,
  TASK_ON_END: 3,
});

/**
 * Message types to communicate with `barelymusician-processor`.
 * @enum {int}
 */
export const MessageType = Object.freeze({
  INIT_SUCCESS: 0,
  UPDATE: 1,
  UPDATE_SUCCESS: 2,
});

/** Stateless command factory. */
class CommandFactory {
  engine() {
    return {
      setControl: (typeIndex, value) => ({type: CommandType.ENGINE_SET_CONTROL, typeIndex, value}),
      setTempo: (tempo) => ({type: CommandType.ENGINE_SET_TEMPO, tempo}),
    };
  }

  /** @param {number} handle */
  instrument(handle) {
    return {
      setAllNotesOff: () => ({type: CommandType.INSTRUMENT_SET_ALL_NOTES_OFF, handle}),
      setControl: (typeIndex, value) =>
          ({type: CommandType.INSTRUMENT_SET_CONTROL, handle, typeIndex, value}),
      setNoteControl: (pitch, typeIndex, value) =>
          ({type: CommandType.INSTRUMENT_SET_NOTE_CONTROL, handle, pitch, typeIndex, value}),
      setNoteOff: (pitch) => ({type: CommandType.INSTRUMENT_SET_NOTE_OFF, handle, pitch}),
      setNoteOn: (pitch, gain = 1.0, pitchShift = 0.0) =>
          ({type: CommandType.INSTRUMENT_SET_NOTE_ON, handle, pitch, gain, pitchShift}),
    };
  }

  /** @param {number} handle */
  performer(handle) {
    return {
      setLoopBeginPosition: (loopBeginPosition) =>
          ({type: CommandType.PERFORMER_SET_LOOP_BEGIN_POSITION, handle, loopBeginPosition}),
      setLoopLength: (loopLength) =>
          ({type: CommandType.PERFORMER_SET_LOOP_LENGTH, handle, loopLength}),
      setLooping: (isLooping) => ({type: CommandType.PERFORMER_SET_LOOPING, handle, isLooping}),
      setPosition: (position) => ({type: CommandType.PERFORMER_SET_POSITION, handle, position}),
      start: () => ({type: CommandType.PERFORMER_START, handle}),
      stop: () => ({type: CommandType.PERFORMER_STOP, handle}),
      syncTo: (otherHandle, offset = 0.0) =>
          ({type: CommandType.PERFORMER_SYNC_TO, handle, otherHandle, offset}),
    };
  }

  /** @param {number} handle */
  task(handle) {
    return {
      setDuration: (duration) => ({type: CommandType.TASK_SET_DURATION, handle, duration}),
      setPosition: (position) => ({type: CommandType.TASK_SET_POSITION, handle, position}),
      setPriority: (priority) => ({type: CommandType.TASK_SET_PRIORITY, handle, priority}),
    };
  }
}

export const Command = new CommandFactory();
