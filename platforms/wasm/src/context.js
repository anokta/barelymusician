/**
 * Command types to process.
 * @enum {int}
 */
export const CommandType = Object.freeze({
  ENGINE_SET_TEMPO: 0,
  ENGINE_SET_CONTROL: 1,

  INSTRUMENT_CREATE: 2,
  INSTRUMENT_DESTROY: 3,
  INSTRUMENT_ON_NOTE_ON: 4,
  INSTRUMENT_ON_NOTE_OFF: 5,
  INSTRUMENT_SET_ALL_NOTES_OFF: 6,
  INSTRUMENT_SET_CONTROL: 7,
  INSTRUMENT_SET_NOTE_CONTROL: 8,
  INSTRUMENT_SET_NOTE_ON: 9,
  INSTRUMENT_SET_NOTE_OFF: 10,
  INSTRUMENT_SET_SAMPLE_DATA: 11,

  PERFORMER_CREATE: 12,
  PERFORMER_DESTROY: 13,
  PERFORMER_SET_LOOPING: 14,
  PERFORMER_SET_LOOP_BEGIN_POSITION: 15,
  PERFORMER_SET_LOOP_LENGTH: 16,
  PERFORMER_SET_POSITION: 17,
  PERFORMER_START: 18,
  PERFORMER_STOP: 19,

  TASK_CREATE: 20,
  TASK_DESTROY: 21,
  TASK_ON_EVENT: 22,
  TASK_SET_DURATION: 23,
  TASK_SET_POSITION: 24,
  TASK_SET_PRIORITY: 25,
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

/**
 * barelymusician context.
 */
export class Context {
  /**
   * @param {!Module} module
   * @param {!Engine} engine
   */
  constructor(module, engine) {
    /** @private @const {!Module} */
    this._module = module;

    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private {!Array<!Object>} */
    this._pendingCommands = [];
  }

  /**
   * Sets a control value of the engine.
   * @param {EngineControlType} type
   * @param {number} value
   */
  engineSetControl(type, value) {
    this._module._BarelyEngine_SetControl(this._engine, type, value);
  }

  /**
   * Sets the tempo of the engine in beats per minute.
   * @param {number} tempo
   */
  engineSetTempo(tempo) {
    this._module._BarelyEngine_SetTempo(this._engine, tempo);
  }

  /**
   * Sets the position of a performer.
   * @param {number} performerId
   * @param {number} position
   */
  performerSetPosition(performerId, position) {
    this._module._BarelyPerformer_SetPosition(this._engine, performerId, position);
  }

  /**
   * Starts a performer.
   * @param {number} performerId
   */
  performerStart(performerId) {
    this._module._BarelyPerformer_Start(this._engine, performerId);
  }

  /**
   * Stops a performer.
   * @param {number} performerId
   */
  performerStop(performerId) {
    this._module._BarelyPerformer_Stop(this._engine, performerId);
  }
};
