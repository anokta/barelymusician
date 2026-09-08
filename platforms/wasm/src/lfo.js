import {CommandType} from './command.js'

/**
 * A representation of a low-frequency oscillator.
 */
export class Lfo {
  /**
   * @param {!Engine} engine
   * @param {number} handle
   */
  constructor(engine, handle) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {number} */
    this._handle = handle;

    /** @private {number} */
    this._phase = 0.0;

    /** @private {number} */
    this._value = 0.0;
  }

  /** Destroys the lfo. */
  destroy() {
    this._engine._lfos.delete(this._handle);
    this._engine._pushCommand({type: CommandType.LFO_DESTROY, handle: this._handle});
  }

  /**
   * Sets a control value.
   * @param {number} typeIndex
   * @param {number} value
   */
  setControl(typeIndex, value) {
    this._engine._pushCommand(
        {type: CommandType.LFO_SET_CONTROL, handle: this._handle, typeIndex, value});
  }

  /** @param {number} phase */
  setPhase(phase) {
    this._phase = phase;
    this._engine._pushCommand({type: CommandType.LFO_SET_PHASE, handle: this._handle, phase});
  }

  /** @param {number} speed */
  setSpeed(speed) {
    this._engine._pushCommand({type: CommandType.LFO_SET_SPEED, handle: this._handle, speed});
  }

  /** @return {number} */
  get handle() {
    return this._handle;
  }

  /** @return {number} */
  get phase() {
    return this._phase;
  }

  /** @return {number} */
  get value() {
    return this._value;
  }
}
