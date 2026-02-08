import {MessageType} from './message.js'

/**
 * A representation of a musical performer that can perform rhythmic tasks in real-time.
 */
export class Performer {
  /**
   * @param {!Engine} engine
   * @param {!Promise<number>} idPromise
   */
  constructor(engine, idPromise) {
    /** @private @const {!Engine} */
    this._engine = engine;

    /** @private @const {!Promise<number>} */
    this._idPromise = idPromise;

    /** @private {boolean} */
    this._isDestroyed = false;

    /** @private {number} */
    this._position = 0.0;
  }

  /**
   * Destroys the performer.
   * @return {!Promise<void>}
   */
  async destroy() {
    if (this._isDestroyed) return;
    await this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_DESTROY, id});
    });
    this._isDestroyed = true;
  }

  /** Starts playback. */
  start() {
    this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_START, id});
    });
  }

  /** Stops playback. */
  stop() {
    this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_STOP, id});
    });
  }

  /** @param {boolean} isLooping */
  setLooping(isLooping) {
    this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_SET_LOOPING, id, isLooping});
    });
  }

  /** @param {number} loopBeginPosition */
  setLoopBeginPosition(loopBeginPosition) {
    this._withId(id => {
      this._engine._pushMessage(
          {type: MessageType.PERFORMER_SET_LOOP_BEGIN_POSITION, id, loopBeginPosition});
    });
  }

  /** @param {number} loopLength */
  setLoopLength(loopLength) {
    this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_SET_LOOP_LENGTH, id, loopLength});
    });
  }

  /** @param {number} position */
  setPosition(position) {
    this._position = position;
    this._withId(id => {
      this._engine._pushMessage({type: MessageType.PERFORMER_SET_POSITION, id, position});
    });
  }

  /** @return {!Promise<void>} */
  get id() {
    return this._idPromise;
  }

  /** @return {number} */
  get position() {
    return this._position;
  }

  /**
   * @param {function(number):(void|!Promise<void>)} fn
   * @return {!Promise<void>}
   * @private
   */
  async _withId(fn) {
    if (this._isDestroyed) return;
    const id = await this._idPromise;
    await fn(id);
  }
}
