/**
 * Command types.
 * @enum {int}
 */
export const CommandType = Object.freeze({
  ENGINE_SET_CONTROL: 0,
  ENGINE_SET_SPEED: 1,

  INSTRUMENT_CREATE: 2,
  INSTRUMENT_DESTROY: 3,
  INSTRUMENT_SET_ALL_NOTES_OFF: 4,
  INSTRUMENT_SET_CONTROL: 5,
  INSTRUMENT_SET_NOTE_CONTROL: 6,
  INSTRUMENT_SET_NOTE_ON: 7,
  INSTRUMENT_SET_NOTE_OFF: 8,
  INSTRUMENT_SET_SAMPLE_DATA: 9,

  LFO_CREATE: 10,
  LFO_DESTROY: 11,
  LFO_SET_CONTROL: 12,
  LFO_SET_PHASE: 13,
  LFO_SET_SPEED: 14,

  PERFORMER_CREATE: 15,
  PERFORMER_DESTROY: 16,
  PERFORMER_SET_LOOP_BEGIN_POSITION: 17,
  PERFORMER_SET_LOOP_LENGTH: 18,
  PERFORMER_SET_LOOPING: 19,
  PERFORMER_SET_POSITION: 20,
  PERFORMER_SET_SPEED: 21,
  PERFORMER_START: 22,
  PERFORMER_STOP: 23,
  PERFORMER_SYNC_TO: 24,

  TASK_CREATE: 25,
  TASK_DESTROY: 26,
  TASK_SET_COMMANDS: 27,
  TASK_SET_DURATION: 28,
  TASK_SET_POSITION: 29,
  TASK_SET_PRIORITY: 30,
});

/**
 * Event callback types.
 * @enum {int}
 */
export const EventCallbackType = Object.freeze({
  INSTRUMENT_ON_ALL_NOTES_OFF: 0,
  INSTRUMENT_ON_NOTE_OFF: 1,
  INSTRUMENT_ON_NOTE_ON: 2,
  TASK_ON_BEGIN: 3,
  TASK_ON_END: 4,
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
      setSpeed: (speed) => ({type: CommandType.ENGINE_SET_SPEED, speed}),
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
  lfo(handle) {
    return {
      setControl: (typeIndex, value) =>
          ({type: CommandType.LFO_SET_CONTROL, handle, typeIndex, value}),
      setPhase: (phase) => ({type: CommandType.LFO_SET_PHASE, handle, phase}),
      setSpeed: (speed) => ({type: CommandType.LFO_SET_SPEED, handle, speed}),
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
      setSpeed: (speed) => ({type: CommandType.PERFORMER_SET_SPEED, handle, speed}),
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
