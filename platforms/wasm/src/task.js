import {MessageType} from './message.js'

/**
 * Task event types.
 * @enum {number}
 */
export const TaskEventType = {
  BEGIN: 0,
  END: 1,
  COUNT: 2,
};

/**
 * A representation of a recurring task that can be performed by a musical
 * performer in real-time.
 */
export class Task {
  /**
   * @param {{
   *   audioNode: !AudioWorkletNode,
   *   idPromise: !Promise<number>,
   *   position: number,
   *   duration: number,
   *   priority: number,
   *   eventCallback: function(number):void
   * }} params
   */
  constructor({audioNode, idPromise, position, duration, priority, eventCallback}) {
    /** @private @const {!AudioWorkletNode} */
    this._audioNode = audioNode;

    /** @private @const {!Promise<number>} */
    this._idPromise = idPromise;

    /** @private {boolean} */
    this._isDestroyed = false;

    /** @private {boolean} */
    this._isActive = false;

    /** @public */
    this.eventCallback = eventCallback;
  }

  /**
   * Destroys the task.
   * @return {!Promise<void>}
   */
  async destroy() {
    if (this._isDestroyed) return;
    await this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_DESTROY,
        id,
      });
    });
    this._isDestroyed = true;
  }

  /** @param {number} duration */
  setDuration(duration) {
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_DURATION,
        id,
        duration,
      });
    });
  }

  /** @param {number} position */
  setPosition(position) {
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_POSITION,
        id,
        position,
      });
    });
  }

  /** @param {number} priority */
  setPriority(priority) {
    this._withId(id => {
      this._audioNode.port.postMessage({
        type: MessageType.TASK_SET_PRIORITY,
        id,
        priority,
      });
    });
  }

  /** @return {!Promise<void>} */
  get id() {
    return this._idPromise;
  }

  /** @return {boolean} */
  get isActive() {
    return this._isActive;
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
