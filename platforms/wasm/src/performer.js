import {CommandType} from './context.js'

/**
 * A representation of a musical performer that can perform rhythmic tasks in real-time.
 */
export class Performer {
  /**
   * @param {!Engine} engine
   * @param {number} id
   */
  constructor(engine, id) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {number} */
    this._id = id;

    /** @private {number} */
    this._position = 0.0;
  }

  /**
   * Destroys the performer.
   */
  destroy() {
    this._engine._performers.delete(this._id);
    this._engine._pushCommand({type: CommandType.PERFORMER_DESTROY, id: this._id});
  }

  /** Starts playback. */
  start() {
    this._engine._pushCommand({type: CommandType.PERFORMER_START, id: this._id});
  }

  /** Stops playback. */
  stop() {
    this._engine._pushCommand({type: CommandType.PERFORMER_STOP, id: this._id});
  }

  /** @param {boolean} isLooping */
  setLooping(isLooping) {
    this._engine._pushCommand({type: CommandType.PERFORMER_SET_LOOPING, id: this._id, isLooping});
  }

  /** @param {number} loopBeginPosition */
  setLoopBeginPosition(loopBeginPosition) {
    this._engine._pushCommand(
        {type: CommandType.PERFORMER_SET_LOOP_BEGIN_POSITION, id: this._id, loopBeginPosition});
  }

  /** @param {number} loopLength */
  setLoopLength(loopLength) {
    this._engine._pushCommand(
        {type: CommandType.PERFORMER_SET_LOOP_LENGTH, id: this._id, loopLength});
  }

  /** @param {number} position */
  setPosition(position) {
    this._position = position;
    this._engine._pushCommand({type: CommandType.PERFORMER_SET_POSITION, id: this._id, position});
  }

  /** @return {number} */
  get id() {
    return this._id;
  }

  /** @return {number} */
  get position() {
    return this._position;
  }
}
