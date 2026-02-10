import {CommandType} from './context.js'

/**
 * A representation of a musical performer that can perform rhythmic tasks in real-time.
 */
export class Performer {
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
    this._position = 0.0;
  }

  /**
   * Destroys the performer.
   */
  destroy() {
    this._engine._performers.delete(this._handle);
    this._engine._pushCommand({type: CommandType.PERFORMER_DESTROY, handle: this._handle});
  }

  /** Starts playback. */
  start() {
    this._engine._pushCommand({type: CommandType.PERFORMER_START, handle: this._handle});
  }

  /** Stops playback. */
  stop() {
    this._engine._pushCommand({type: CommandType.PERFORMER_STOP, handle: this._handle});
  }

  /** @param {boolean} isLooping */
  setLooping(isLooping) {
    this._engine._pushCommand(
        {type: CommandType.PERFORMER_SET_LOOPING, handle: this._handle, isLooping});
  }

  /** @param {number} loopBeginPosition */
  setLoopBeginPosition(loopBeginPosition) {
    this._engine._pushCommand({
      type: CommandType.PERFORMER_SET_LOOP_BEGIN_POSITION,
      handle: this._handle,
      loopBeginPosition,
    });
  }

  /** @param {number} loopLength */
  setLoopLength(loopLength) {
    this._engine._pushCommand(
        {type: CommandType.PERFORMER_SET_LOOP_LENGTH, handle: this._handle, loopLength});
  }

  /** @param {number} position */
  setPosition(position) {
    this._position = position;
    this._engine._pushCommand(
        {type: CommandType.PERFORMER_SET_POSITION, handle: this._handle, position});
  }

  /** @return {number} */
  get handle() {
    return this._handle;
  }

  /** @return {number} */
  get position() {
    return this._position;
  }
}
