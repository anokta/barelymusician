import {MessageType} from './message.js'

/**
 * A representation of a musical performer that can perform rhythmic tasks in real-time.
 */
export class Performer {
  /**
   * @param {{
   *   audioNode: !AudioWorkletNode,
   *   idPromise: !Promise<number>
   * }} params
   */
  constructor({audioNode, idPromise}) {
    /** @private @const {!AudioWorkletNode} */
    this._audioNode = audioNode;

    /** @private @const {!Promise<number>} */
    this._idPromise = idPromise;

    /** @private {boolean} */
    this._isDestroyed = false;

    /** @private {boolean} */
    this._isLooping = false;

    /** @private {boolean} */
    this._isPlaying = false;

    /** @private {number} */
    this._loopBeginPosition = 0.0;

    /** @private {number} */
    this._loopLength = 1.0;

    /** @private {number} */
    this._position = 0.0;
  }

  /**
   * Destroys the performer.
   * @return {!Promise<void>}
   */
  async destroy() {
    if (this._isDestroyed) return;

    this._isDestroyed = true;
    await this._withId(id => {
      this._audioNode.port.postMessage({type: MessageType.PERFORMER_DESTROY, id});
    });
  }

  /** Starts playback. */
  start() {
    this._isPlaying = true;
    this._withId(id => {
      this._audioNode.port.postMessage({type: MessageType.PERFORMER_START, id});
    });
  }

  /** Stops playback. */
  stop() {
    this._isPlaying = false;
    this._withId(id => {
      this._audioNode.port.postMessage({type: MessageType.PERFORMER_STOP, id});
    });
  }

  /** @param {boolean} newIsLooping */
  set isLooping(newIsLooping) {
    if (this._isLooping === newIsLooping) return;

    this._isLooping = newIsLooping;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.PERFORMER_SET_LOOPING,
        id,
        isLooping: newIsLooping,
      });
    });
  }

  /** @param {number} newLoopBeginPosition */
  set loopBeginPosition(newLoopBeginPosition) {
    if (this._loopBeginPosition === newLoopBeginPosition) return;

    this._loopBeginPosition = newLoopBeginPosition;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.PERFORMER_SET_LOOP_BEGIN_POSITION,
        id,
        loopBeginPosition: newLoopBeginPosition,
      });
    });
  }

  /** @param {number} newLoopLength */
  set loopLength(newLoopLength) {
    if (this._loopLength === newLoopLength) return;

    this._loopLength = Math.max(newLoopLength, 0.0);
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.PERFORMER_SET_LOOP_LENGTH,
        id,
        loopLength: this._loopLength,
      });
    });
  }

  /** @param {number} newPosition */
  set position(newPosition) {
    if (this._position === newPosition) return;

    this._position = newPosition;
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.PERFORMER_SET_POSITION,
        id,
        position: newPosition,
      });
    });
  }

  /** @return {!Promise<void>} */
  get id() {
    return this._idPromise;
  }

  /** @return {boolean} */
  get isLooping() {
    return this._isLooping;
  }

  /** @return {boolean} */
  get isPlaying() {
    return this._isPlaying;
  }

  /** @return {number} */
  get loopBeginPosition() {
    return this._loopBeginPosition;
  }

  /** @return {number} */
  get loopLength() {
    return this._loopLength;
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
