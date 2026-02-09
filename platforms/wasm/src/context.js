/**
 * Command types to process.
 * @enum {int}
 */
export const CommandType = Object.freeze({
  ENGINE_SET_TEMPO: 0,
  ENGINE_SET_CONTROL: 1,

  INSTRUMENT_CREATE: 2,
  INSTRUMENT_DESTROY: 4,
  INSTRUMENT_ON_NOTE_ON: 6,
  INSTRUMENT_ON_NOTE_OFF: 7,
  INSTRUMENT_SET_ALL_NOTES_OFF: 8,
  INSTRUMENT_SET_CONTROL: 9,
  INSTRUMENT_SET_NOTE_CONTROL: 10,
  INSTRUMENT_SET_NOTE_ON: 11,
  INSTRUMENT_SET_NOTE_OFF: 12,
  INSTRUMENT_SET_SAMPLE_DATA: 13,

  PERFORMER_CREATE: 14,
  PERFORMER_DESTROY: 16,
  PERFORMER_GET_PROPERTIES_SUCCESS: 18,
  PERFORMER_SET_LOOPING: 19,
  PERFORMER_SET_LOOP_BEGIN_POSITION: 20,
  PERFORMER_SET_LOOP_LENGTH: 21,
  PERFORMER_SET_POSITION: 22,
  PERFORMER_START: 23,
  PERFORMER_STOP: 24,

  TASK_CREATE: 25,
  TASK_DESTROY: 27,
  TASK_GET_PROPERTIES_SUCCESS: 29,
  TASK_ON_EVENT: 30,
  TASK_SET_DURATION: 31,
  TASK_SET_POSITION: 32,
  TASK_SET_PRIORITY: 33,
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
